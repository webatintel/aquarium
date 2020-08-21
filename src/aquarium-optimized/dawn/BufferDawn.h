//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// BufferDawn.h: Defines the buffer wrapper of dawn, abstracting the vetex and index buffer binding.

#ifndef BUFFERDAWN_H
#define BUFFERDAWN_H

#include <dawn/webgpu_cpp.h>
#include <vector>

#include "../Buffer.h"

class ContextDawn;

class BufferDawn : public Buffer
{
  public:
    BufferDawn(ContextDawn *context,
               int totalCmoponents,
               int numComponents,
               std::vector<float> *buffer,
               bool isIndex);
    BufferDawn(ContextDawn *context,
               int totalCmoponents,
               int numComponents,
               std::vector<unsigned short> *buffer,
               bool isIndex);
    ~BufferDawn() override;

    const wgpu::Buffer &getBuffer() const { return mBuf; }
    int getTotalComponents() const { return mTotoalComponents; }

    uint32_t getStride() const { return mStride; }
    const void *getOffset() const { return mOffset; }
    wgpu::BufferUsage getUsageBit() const { return mUsage; }
    int getDataSize() { return mSize; }

  private:
    wgpu::Buffer mBuf;
    wgpu::BufferUsage mUsage;
    int mTotoalComponents;
    uint32_t mStride;
    void *mOffset;
    int mSize;
};

#endif  // BUFFERDAWN_H
