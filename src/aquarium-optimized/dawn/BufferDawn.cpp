//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// BufferDawn.cpp: Implements the index or vertex buffers wrappers and resource bindings of dawn.

#include "BufferDawn.h"
#include "ContextDawn.h"

// Copy size must be a multiple of 4 bytes on dawn mac backend.
BufferDawn::BufferDawn(ContextDawn *context,
                       int totalCmoponents,
                       int numComponents,
                       std::vector<float> *buffer,
                       bool isIndex)
    : mUsageBit(isIndex ? dawn::BufferUsageBit::Index : dawn::BufferUsageBit::Vertex),
      mTotoalComponents(totalCmoponents),
      mStride(0),
      mOffset(nullptr)
{
    mSize = numComponents * sizeof(float);
    if (mTotoalComponents % 4 != 0)
    {
        int dummyCount = 4 - mTotoalComponents % 4;
        for (int i = 0; i < dummyCount; i++)
        {
            buffer->push_back(0.0f);
        }
    }
    mBuf = context->createBufferFromData(
        buffer->data(), sizeof(float) * static_cast<int>(buffer->size()), mUsageBit);
}

BufferDawn::BufferDawn(ContextDawn *context,
                       int totalCmoponents,
                       int numComponents,
                       std::vector<unsigned short> *buffer,
                       bool isIndex)
    : mUsageBit(isIndex ? dawn::BufferUsageBit::Index : dawn::BufferUsageBit::Vertex),
      mTotoalComponents(totalCmoponents),
      mStride(0),
      mOffset(nullptr)
{
    mSize = numComponents * sizeof(unsigned short);
    if (mTotoalComponents % 4 != 0)
    {
        int dummyCount = 4 - mTotoalComponents % 4;
        for (int i = 0; i < dummyCount; i++)
        {
            buffer->push_back(0.0f);
        }
    }
    mBuf = context->createBufferFromData(
        buffer->data(), sizeof(unsigned short) * static_cast<int>(buffer->size()), mUsageBit);
}

BufferDawn::~BufferDawn()
{
    mBuf = nullptr;
}
