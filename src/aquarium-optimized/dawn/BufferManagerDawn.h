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

class ContextDawn;

class RingBufferDawn : public RingBuffer
{
  public:
    RingBufferDawn(ContextDawn *context, size_t size);

    bool push(wgpu::Buffer &destBuffer, size_t dest_offset, void* pixels, size_t size);
    bool reset(size_t size) override;
    void flush() override;
    void destory() override;

    wgpu::CreateBufferMappedResult getCreateBufferMappedResult() const
    {
        return mBufferMappedResult;
    }
    wgpu::Buffer getStagingBuffer() const { return mStagingBuffer; }

  private:
    static void MapWriteCallback(WGPUBufferMapAsyncStatus status,
                                 void *data,
                                 uint64_t,
                                 void *userdata);
    void *MapWriteAsyncAndWait(const wgpu::Buffer &buffer);

    bool mSync;
    wgpu::CreateBufferMappedResult mBufferMappedResult;
    wgpu::Buffer mStagingBuffer;
    wgpu::CommandEncoder mEncoder;
    ContextDawn *mContext;
    void *mappedData;
};

class BufferManagerDawn: public BufferManager
{
  public:
    BufferManagerDawn(ContextDawn *context) : mContext(context) {}

    RingBufferDawn *allocate(size_t size) override;

  private:
    ContextDawn *mContext;
};
