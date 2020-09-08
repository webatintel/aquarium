//
// Copyright (c) 2020 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// BufferManager.h: Implements buffer pool to manage buffer allocation and
// recycle.

#ifndef BUFFERMANAGERDAWN_H
#define BUFFERMANAGERDAWN_H

#include <vector>

#include "dawn/webgpu_cpp.h"

#include "../BufferManager.h"
#include "ContextDawn.h"

class BufferManagerDawn;
class ContextDawn;

class RingBufferDawn : public RingBuffer {
public:
  RingBufferDawn(BufferManagerDawn *bufferManager, size_t size);
  ~RingBufferDawn() override {}

  bool push(const wgpu::CommandEncoder &encoder,
            const wgpu::Buffer &destBuffer,
            size_t src_offset,
            size_t dest_offset,
            void *pixels,
            size_t size);
  bool reset(size_t size) override;
  void flush() override;
  void destory() override;
  void reMap();
  size_t allocate(size_t size) override;

private:
  static void MapWriteCallback(WGPUBufferMapAsyncStatus status,
                               void *data,
                               uint64_t,
                               void *userdata);

  wgpu::Buffer mBuf;

  BufferManagerDawn *mBufferManager;
  void *mappedData;
  void *mPixels;
};

class BufferManagerDawn : public BufferManager {
public:
  BufferManagerDawn(ContextDawn *context, bool sync);
  ~BufferManagerDawn();

  RingBufferDawn *allocate(size_t size, size_t *offset) override;
  void flush() override;
  void destroyBufferPool() override;

  wgpu::CommandEncoder mEncoder;
  ContextDawn *mContext;
  bool mSync;
};

#endif  // BUFFERMANAGERDAWN_H
