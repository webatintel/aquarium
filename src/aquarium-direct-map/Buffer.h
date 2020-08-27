//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Buffer.h : Define Buffer Class and upload buffers to gpu.

#ifndef BUFFER_H
#define BUFFER_H

#include <vector>

#include "glad/glad.h"

#include "AttribBuffer.h"

class Buffer
{
public:
  Buffer() {}
  Buffer(const AttribBuffer &array, GLenum target);
  ~Buffer();

  GLuint getBuffer() const { return mBuf; }
  int getNumComponents() const { return mNumComponents; }
  int getNumElements() const { return mNumElements; }
  int getTotalComponents() const { return mTotalComponents; }
  GLenum getType() const { return mType; }
  bool getNormalize() const { return mNormalize; }
  GLsizei getStride() const { return mStride; }
  void *getOffset() const { return mOffset; }

private:
  GLuint mBuf;
  int mNumComponents;
  int mNumElements;
  int mTotalComponents;
  GLenum mType;
  bool mNormalize;
  GLsizei mStride;
  void *mOffset;
};

#endif  // BUFFER_H
