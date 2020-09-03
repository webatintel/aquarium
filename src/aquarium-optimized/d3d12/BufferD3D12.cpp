//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "BufferD3D12.h"

#include "ContextD3D12.h"

BufferD3D12::BufferD3D12(ContextD3D12 *context,
                         int totalCmoponents,
                         int numComponents,
                         const std::vector<float> &buffer,
                         bool isIndex)
    : mIsIndex(isIndex),
      mTotoalComponents(totalCmoponents),
      mStride(0),
      mOffset(nullptr) {
  mSize = totalCmoponents * sizeof(float);
  mBuffer =
      context->createDefaultBuffer(buffer.data(), mSize, mSize, mUploadBuffer);

  // Initialize the vertex buffer view.
  mVertexBufferView.BufferLocation = mBuffer->GetGPUVirtualAddress();
  mVertexBufferView.StrideInBytes  = numComponents * sizeof(float);
  mVertexBufferView.SizeInBytes    = mSize;
}

BufferD3D12::BufferD3D12(ContextD3D12 *context,
                         int totalCmoponents,
                         int numComponents,
                         const std::vector<unsigned short> &buffer,
                         bool isIndex)
    : mIsIndex(isIndex),
      mTotoalComponents(totalCmoponents),
      mStride(0),
      mOffset(nullptr) {
  mSize = totalCmoponents * sizeof(unsigned short);
  mBuffer =
      context->createDefaultBuffer(buffer.data(), mSize, mSize, mUploadBuffer);

  // Initialize the vertex buffer view.
  mIndexBufferView.BufferLocation = mBuffer->GetGPUVirtualAddress();
  mIndexBufferView.SizeInBytes    = mSize;
  mIndexBufferView.Format         = DXGI_FORMAT_R16_UINT;
}
