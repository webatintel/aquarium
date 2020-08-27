//
// Copyright (c) 2020 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// BufferManager.h: Implements buffer pool to manage buffer allocation and
// recycle.

#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include <queue>
#include <vector>

#include "Context.h"

constexpr size_t BUFFER_POOL_MAX_SIZE    = 409600000;
constexpr size_t BUFFER_MAX_COUNT        = 10;
constexpr size_t BUFFER_PER_ALLOCATE_SIZE = BUFFER_POOL_MAX_SIZE / BUFFER_MAX_COUNT;

class RingBuffer
{
  public:
    RingBuffer(size_t size) : mHead(0), mTail(size), mSize(size) {}
    virtual ~RingBuffer() {}

    size_t getSize() const { return mSize; }
    size_t getAvailableSize() const { return mSize - mTail; }

    virtual bool reset(size_t size) { return false; }
    virtual void flush() {}
    virtual void destory() {}
    virtual size_t allocate(size_t size)
    {
        return 0;
    }  // allocate size in a RingBuffer, return offset of the buffer

  protected:
    size_t mHead;
    size_t mTail;
    size_t mSize;
};

class BufferManager
{
  public:
    BufferManager();
    virtual ~BufferManager();

    size_t GetSize() const { return mBufferPoolSize; }
    bool resetBuffer(RingBuffer *ringBuffer, size_t size);
    bool destoryBuffer(RingBuffer *ringBuffer);
    virtual void destroyBufferPool() {}
    virtual void flush();

    virtual RingBuffer *allocate(size_t size, size_t *offset) { return nullptr; }

    std::queue<RingBuffer *> mMappedBufferList;

  protected:
    std::vector<RingBuffer *> mEnqueuedBufferList;
    size_t mBufferPoolSize;
    size_t mUsedSize;
    size_t mCount;

  private:
    size_t find(RingBuffer *ringBuffer);
};

#endif  // BUFFERMANAGER_H
