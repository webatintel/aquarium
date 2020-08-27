//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// BufferGL.cpp: Implements the index or vertex buffer wrappers and resource bindings of OpenGL.

#include "BufferGL.h"

#include "common/AQUARIUM_ASSERT.h"

BufferGL::BufferGL(ContextGL *context,
                   int totalCmoponents,
                   int numComponents,
                   bool isIndex,
                   unsigned int type,
                   bool normalize)
    : mContext(context),
      mBuf(0),
      mTarget(isIndex ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER),
      mNumComponents(numComponents),
      mTotoalComponents(totalCmoponents),
      mNumElements(mTotoalComponents / numComponents),
      mType(type),
      mNormalize(normalize),
      mStride(0),
      mOffset(nullptr)
{
    mBuf = mContext->generateBuffer();
}

void BufferGL::loadBuffer(const std::vector<float> &buf)
{
    mContext->bindBuffer(mTarget, mBuf);
    mContext->uploadBuffer(mTarget, buf);
}

void BufferGL::loadBuffer(const std::vector<unsigned short> &buf)
{
    mContext->bindBuffer(mTarget, mBuf);
    mContext->uploadBuffer(mTarget, buf);
}

BufferGL::~BufferGL()
{
    mContext->deleteBuffer(mBuf);
}
