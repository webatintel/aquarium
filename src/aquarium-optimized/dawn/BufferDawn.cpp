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
    : mUsage(isIndex ? dawn::BufferUsage::Index : dawn::BufferUsage::Vertex),
      mTotoalComponents(totalCmoponents),
      mStride(0),
      mOffset(nullptr)
{
    mSize = numComponents * sizeof(float);
    // Create buffer for vertex buffer. Because float is multiple of 4 bytes, dummy padding isnt' needed.
    int bufferSize = sizeof(float) * static_cast<int>(buffer->size());
    mBuf           = context->createBuffer(bufferSize, mUsage | dawn::BufferUsage::CopyDst);

    dawn::CreateBufferMappedResult result = context->CreateBufferMapped(
        dawn::BufferUsage::MapWrite | dawn::BufferUsage::CopySrc, bufferSize);
    memcpy(result.data, buffer->data(), bufferSize);
    result.buffer.Unmap();

    dawn::CommandBuffer command =
        context->copyBufferToBuffer(result.buffer, 0, mBuf, 0, bufferSize);
    context->mCommandBuffers.emplace_back(command);
}

BufferDawn::BufferDawn(ContextDawn *context,
                       int totalCmoponents,
                       int numComponents,
                       std::vector<unsigned short> *buffer,
                       bool isIndex)
    : mUsage(isIndex ? dawn::BufferUsage::Index : dawn::BufferUsage::Vertex),
      mTotoalComponents(totalCmoponents),
      mStride(0),
      mOffset(nullptr)
{
    mSize = numComponents * sizeof(unsigned short);
    // Create buffer for index buffer. Because unsigned short is multiple of 2 bytes, in order to align
    // with 4 bytes of dawn metal, dummy padding need to be added.
    if (mTotoalComponents % 2 != 0)
    {
        buffer->push_back(0.0f);
    }

    int bufferSize = sizeof(unsigned short) * static_cast<int>(buffer->size());
    mBuf           = context->createBuffer(bufferSize, mUsage | dawn::BufferUsage::CopyDst);

    dawn::CreateBufferMappedResult result = context->CreateBufferMapped(
        dawn::BufferUsage::MapWrite | dawn::BufferUsage::CopySrc, bufferSize);
    memcpy(result.data, buffer->data(), bufferSize);
    result.buffer.Unmap();

    dawn::CommandBuffer command =
        context->copyBufferToBuffer(result.buffer, 0, mBuf, 0, bufferSize);
    context->mCommandBuffers.emplace_back(command);
}

BufferDawn::~BufferDawn()
{
    mBuf = nullptr;
}
