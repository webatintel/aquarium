//
// Copyright (c) 2020 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "BufferManagerDawn.h"

#include <iostream>
#include <thread>

#include "common/AQUARIUM_ASSERT.h"

RingBufferDawn::RingBufferDawn(BufferManagerDawn *bufferManager, size_t size)
    : RingBuffer(size),
      mBufferManager(bufferManager),
      mappedData(nullptr),
      mPixels(nullptr)
{
  reset(size);
}

bool RingBufferDawn::push(const wgpu::CommandEncoder &encoder,
                          const wgpu::Buffer &destBuffer,
                          size_t src_offset,
                          size_t dest_offset,
                          void *pixels,
                          size_t size)
{
  memcpy(static_cast<unsigned char *>(mPixels) + src_offset, pixels, size);
  encoder.CopyBufferToBuffer(mBufferMappedResult.buffer, src_offset, destBuffer,
                             dest_offset, size);
  return true;
}

// Reset current buffer and reuse the buffer.
bool RingBufferDawn::reset(size_t size)
{
  if (size > mSize)
    return false;

  mHead = 0;
  mTail = 0;

  mBufferMappedResult = mBufferManager->mContext->CreateBufferMapped(
      wgpu::BufferUsage::MapWrite | wgpu::BufferUsage::CopySrc, mSize);
  mPixels = mBufferMappedResult.data;

  return true;
}

void RingBufferDawn::MapWriteCallback(WGPUBufferMapAsyncStatus status,
                                      void *data,
                                      uint64_t,
                                      void *userdata)
{
  ASSERT(status == WGPUBufferMapAsyncStatus_Success);
  ASSERT(data != nullptr);

  RingBufferDawn *ringBuffer = static_cast<RingBufferDawn *>(userdata);
  ringBuffer->mappedData     = data;

  ringBuffer->mBufferManager->mMappedBufferList.push(ringBuffer);
}

void RingBufferDawn::flush()
{
  mHead = 0;
  mTail = 0;

  mBufferMappedResult.buffer.Unmap();
}

void RingBufferDawn::destory()
{
  mBufferMappedResult.buffer = nullptr;
}

void RingBufferDawn::reMap()
{
  mBufferMappedResult.buffer.MapWriteAsync(MapWriteCallback, this);
}

size_t RingBufferDawn::allocate(size_t size)
{
  mTail += size;
  ASSERT(mTail < mSize);

  return mTail - size;
}

BufferManagerDawn::BufferManagerDawn(ContextDawn *context, bool sync)
    : mContext(context), mSync(sync)
{
  mEncoder = context->createCommandEncoder();
}

BufferManagerDawn::~BufferManagerDawn()
{
  mEncoder = nullptr;
}

// Allocate new buffer from buffer pool.
RingBufferDawn *BufferManagerDawn::allocate(size_t size, size_t *offset)
{
  // If update data by sync method, create new buffer to upload every frame.
  // If updaye data by async method, get new buffer from pool if available. If
  // no available buffer and size is enough in the buffer pool, create a new
  // buffer. If size reach the limit of the buffer pool, force wait for the
  // buffer on mapping. Get the last one and check if the ring buffer is full.
  // If the buffer can hold extra size space, use the last one directly.
  // TODO(yizhou): Return nullptr if size reach the limit or no available
  // buffer, this means small bubbles in some of the ring buffers and we haven't
  // deal with the problem now.

  RingBufferDawn *ringBuffer = nullptr;
  size_t cur_offset          = 0;
  if (mSync)
  {
    // Upper limit
    if (mUsedSize + size > mBufferPoolSize)
    {
      return nullptr;
    }

    ringBuffer = new RingBufferDawn(this, size);
    mEnqueuedBufferList.emplace_back(ringBuffer);
  }
  else  // Buffer mapping async
  {
    while (!mMappedBufferList.empty())
    {
      ringBuffer = static_cast<RingBufferDawn *>(mMappedBufferList.front());
      if (ringBuffer->getAvailableSize() < size)
      {
        mMappedBufferList.pop();
        ringBuffer = nullptr;
      }
      else
      {
        break;
      }
    }

    if (ringBuffer == nullptr)
    {
      if (mCount < BUFFER_MAX_COUNT)
      {
        mUsedSize += size;
        ringBuffer = new RingBufferDawn(this, BUFFER_PER_ALLOCATE_SIZE);
        mMappedBufferList.push(ringBuffer);
        mCount++;
      }
      else if (mMappedBufferList.size() + mEnqueuedBufferList.size() < mCount)
      {
        // Force wait for the buffer remapping
        while (mMappedBufferList.empty())
        {
          mContext->WaitABit();
        }

        ringBuffer = static_cast<RingBufferDawn *>(mMappedBufferList.front());
        if (ringBuffer->getAvailableSize() < size)
        {
          mMappedBufferList.pop();
          ringBuffer = nullptr;
        }
      }
      else  // Upper limit
      {
        return nullptr;
      }
    }

    if (mEnqueuedBufferList.empty() || mEnqueuedBufferList.back() != ringBuffer)
    {
      mEnqueuedBufferList.emplace_back(ringBuffer);
    }

    // allocate size in the ring buffer
    cur_offset = ringBuffer->allocate(size);
    *offset    = cur_offset;
  }

  return ringBuffer;
}

void BufferManagerDawn::flush()
{
  // The front buffer in MappedBufferList will be remap after submit, pop the
  // buffer from MappedBufferList.
  if (!mMappedBufferList.empty() &&
      mEnqueuedBufferList.back() == mMappedBufferList.front())
  {
    mMappedBufferList.pop();
  }

  for (auto buffer : mEnqueuedBufferList)
  {
    buffer->flush();
  }

  wgpu::CommandBuffer copy = mEncoder.Finish();
  mContext->queue.Submit(1, &copy);

  // Async function
  if (!mSync)
  {
    for (size_t index = 0; index < mEnqueuedBufferList.size(); index++)
    {
      RingBufferDawn *ringBuffer =
          static_cast<RingBufferDawn *>(mEnqueuedBufferList[index]);
      ringBuffer->reMap();
    }
  }
  else
  {
    // All buffers are used once in buffer sync mode.
    for (size_t index = 0; index < mEnqueuedBufferList.size(); index++)
    {
      delete mEnqueuedBufferList[index];
    }
    mUsedSize = 0;
  }

  mEnqueuedBufferList.clear();
  mEncoder = mContext->createCommandEncoder();
}

void BufferManagerDawn::destroyBufferPool()
{
  if (!mSync)
  {
    return;
  }

  for (auto ringBuffer : mEnqueuedBufferList)
  {
    ringBuffer->destory();
  }
  mEnqueuedBufferList.clear();
}
