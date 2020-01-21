//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// BufferManager.h: Implements buffer pool to manage buffer allocation and
// recycle.
#pragma once

#include <dawn/webgpu_cpp.h>
#include <vector>
#include "../BufferManager.h"
#include "ContextDawn.h"

class BufferManagerDawn;
class ContextDawn;

class RingBufferDawn : public RingBuffer
{
  public:
    RingBufferDawn(BufferManagerDawn *bufferManager, size_t size);
    ~RingBufferDawn() {}

    bool push(const wgpu::CommandEncoder &encoder,
              wgpu::Buffer &destBuffer,
              size_t dest_offset,
              void *pixels,
              size_t size);
    bool reset(size_t size) override;
    void flush() override;
    void destory() override;
    void reMap();

  private:
    static void MapWriteCallback(WGPUBufferMapAsyncStatus status,
                                 void *data,
                                 uint64_t,
                                 void *userdata);

    wgpu::CreateBufferMappedResult mBufferMappedResult;

    BufferManagerDawn *mBufferManager;
    void *mappedData;
    void *mPixels;
};

class BufferManagerDawn: public BufferManager
{
  public:
    BufferManagerDawn(ContextDawn *context);
    ~BufferManagerDawn();

    RingBufferDawn *allocate(size_t size, bool sync) override;
    void flush() override;
    void destroyBufferPool() override;

    wgpu::CommandEncoder mEncoder;
    ContextDawn *mContext;
    bool mSync;
};
