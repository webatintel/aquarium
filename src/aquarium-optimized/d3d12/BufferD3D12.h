//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// BufferD3D12.h: Defines the buffer wrapper of D3D12, abstracting the vetex and index buffer
// binding.

#ifndef BUFFERD3D12_H
#define BUFFERD3D12_H

#include <vector>

#include "stdafx.h"
using Microsoft::WRL::ComPtr;

#include "../Buffer.h"

class ContextD3D12;

class BufferD3D12 : public Buffer
{
  public:
    BufferD3D12(ContextD3D12 *context,
                int totalCmoponents,
                int numComponents,
                const std::vector<float> &buffer,
                bool isIndex);
    BufferD3D12(ContextD3D12 *context,
                int totalCmoponents,
                int numComponents,
                const std::vector<unsigned short> &buffer,
                bool isIndex);

    ComPtr<ID3D12Resource> getBuffer() const { return mBuffer; }

    int getTotalComponents() const { return mTotoalComponents; }

    uint32_t getStride() const { return mStride; }
    void *getOffset() const { return mOffset; }
    bool getIsIndex() const { return mIsIndex; }
    int getDataSize() { return mSize; }

    D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
    D3D12_INDEX_BUFFER_VIEW mIndexBufferView;

  private:
    ComPtr<ID3D12Resource> mBuffer;
    ComPtr<ID3D12Resource> mUploadBuffer;
    bool mIsIndex;
    int mTotoalComponents;
    uint32_t mStride;
    void *mOffset;
    int mSize;
};

#endif  // BUFFERD3D12_H
