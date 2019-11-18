//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// BufferManager.h: Implements buffer pool to manage buffer allocation and
// recycle.
#pragma once

#include <vector>
#include "Context.h"

constexpr size_t BUFFER_POOL_MAX_SIZE = 1048576;

class RingBuffer
{
  public:
    RingBuffer(size_t size) : mHead(0), mTail(size), mSize(size) {}
    ~RingBuffer() {}

    size_t getSize() const { return mSize; }

    virtual bool reset(size_t size) { return false; }
    virtual void flush() {}
    virtual void destory() {}

  protected:
    size_t mHead;
    size_t mTail;
    size_t mSize;
};

class BufferManager
{
  public:
    BufferManager();
    ~BufferManager();

    size_t GetSize() const { return mBufferPoolSize; }
    bool resetBuffer(RingBuffer *ringBuffer, size_t size);
    bool destoryBuffer(RingBuffer *ringBuffer);
    void destroyBufferPool();
    void flush();

    virtual RingBuffer *allocate(size_t size) { return nullptr; }

  protected:
    std::vector<RingBuffer *> mBufferPool;
    size_t mBufferPoolSize;
    size_t mUsedSize;

  private:
    size_t find(RingBuffer *ringBuffer);
};
