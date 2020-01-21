#include "BufferManagerDawn.h"

#include <iostream>
#include <thread>
#include "common/AQUARIUM_ASSERT.h"

RingBufferDawn::RingBufferDawn(BufferManagerDawn *bufferManager, size_t size)
    : RingBuffer(size), mBufferManager(bufferManager), mappedData(nullptr), mPixels(nullptr)
{
    reset(size);
}

bool RingBufferDawn::push(const wgpu::CommandEncoder &encoder,
                          wgpu::Buffer &destBuffer,
                          size_t dest_offset,
                          void *pixels,
                          size_t size)
{
    mTail = mHead + size;
    if (mTail > mSize)
    {
        return false;
    }

    memcpy(mPixels, pixels, size);
    encoder.CopyBufferToBuffer(mBufferMappedResult.buffer, mHead, destBuffer, dest_offset, size);

    mHead = mTail;
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

BufferManagerDawn::BufferManagerDawn(ContextDawn *context) : mContext(context)
{
    mEncoder = context->createCommandEncoder();
}

BufferManagerDawn::~BufferManagerDawn()
{
    mEncoder = nullptr;
}

// Allocate new buffer from buffer pool.
RingBufferDawn *BufferManagerDawn::allocate(size_t size, bool sync)
{
    // TODO(yizhou): Refactor ring buffer allocation. Get the last one and check if the ring buffer
    // is full. If the buffer can hold extra size space, use the last one directly.
    // If update data by sync method, create new buffer to upload every frame.
    // If updaye data by async method, get new buffer from pool if available. If no available
    // buffer, create a new buffer.
    mSync = sync;

    RingBufferDawn *ringBuffer = nullptr;
    if (sync)
    {
        if (mUsedSize + size > mBufferPoolSize)
        {
            return nullptr;
        }

        ringBuffer = new RingBufferDawn(this, size);
        mEnqueuedBufferList.emplace_back(ringBuffer);
    }
    else
    {
        while (!mMappedBufferList.empty())
        {
            ringBuffer = static_cast<RingBufferDawn *>(mMappedBufferList.front());
            mMappedBufferList.pop();
            if (ringBuffer->getSize() < size)
            {
                mUsedSize += ringBuffer->getSize();
                ringBuffer->destory();
                ringBuffer = nullptr;
            }
            else
            {
                break;
            }
        }

        if (ringBuffer == nullptr)
        {
            if (mUsedSize + size > mBufferPoolSize)
            {
                return nullptr;
            }
            mUsedSize += size;
            std::cout << mUsedSize << std::endl;
            ringBuffer = new RingBufferDawn(this, size);
        }

        if (mEnqueuedBufferList.empty() || mEnqueuedBufferList.back() != ringBuffer)
        {
            mEnqueuedBufferList.emplace_back(ringBuffer);
        }
    }

    return ringBuffer;
}

void BufferManagerDawn::flush()
{
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
            RingBufferDawn *ringBuffer = static_cast<RingBufferDawn *>(mEnqueuedBufferList[index]);
            ringBuffer->reMap();
        }
    }
    else
    {
        // All buffers are used once in buffer sync mode.
        for (size_t index = 0; index < mEnqueuedBufferList.size(); index++)
        {
            RingBufferDawn *ringBuffer = static_cast<RingBufferDawn *>(mEnqueuedBufferList[index]);
            delete ringBuffer;
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
