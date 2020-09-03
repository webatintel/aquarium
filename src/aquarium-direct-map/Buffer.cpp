//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Buffer.cpp: Implement the index or vertex buffer wrappers and resource
// bindings of OpenGL.

#include "Buffer.h"

#include <iostream>

#include "common/AQUARIUM_ASSERT.h"

Buffer::Buffer(const AttribBuffer &attribBuffer, GLenum target)
    : mBuf(0),
      mNumComponents(attribBuffer.getNumComponents()),
      mNumElements(attribBuffer.getNumElements()),
      mTotalComponents(0),
      mType(0),
      mNormalize(true),
      mStride(0),
      mOffset(nullptr)

{
  glGenBuffers(1, &mBuf);

  glBindBuffer(target, mBuf);

  mTotalComponents = mNumComponents * mNumElements;

  auto bufferFloat = attribBuffer.getBufferFloat();
  auto bufferUShort = attribBuffer.getBufferUShort();

  if (attribBuffer.getType() == "Float32Array") {
    mType = GL_FLOAT;
    mNormalize = false;
    glBufferData(target, sizeof(GLfloat) * bufferFloat.size(),
                 bufferFloat.data(), GL_STATIC_DRAW);
  } else if (attribBuffer.getType() == "Uint16Array") {
    mType = GL_UNSIGNED_SHORT;
    glBufferData(target, sizeof(GLushort) * bufferUShort.size(),
                 bufferUShort.data(), GL_STATIC_DRAW);
  } else {
    std::cout << "bindBufferData undefined type." << std::endl;
  }

  ASSERT(glGetError() == GL_NO_ERROR);
}

Buffer::~Buffer() {
  glDeleteBuffers(1, &mBuf);
}
