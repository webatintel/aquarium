//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// BufferGL.h: Defines the buffer wrapper of OpenGL.

#pragma once
#ifndef BUFFERGL_H
#define BUFFERGL_H 1

#ifdef EGL_EGL_PROTOTYPES
#include <angle_gl.h>
#include <memory>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "EGL/eglext_angle.h"
#include "EGL/eglplatform.h"
#include "EGLWindow.h"
#elif __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#else
#include "glad/glad.h"
#endif

#include <vector>

#include "ContextGL.h"

#include "../Buffer.h"

class ContextGL;

class BufferGL : public Buffer
{
  public:
    BufferGL(ContextGL *context,
             int totalCmoponents,
             int numComponents,
             bool isIndex,
             unsigned int type,
             bool normalize);
    ~BufferGL() override;

    unsigned int getBuffer() const { return mBuf; }
    int getNumComponents() const { return mNumComponents; }
    int getTotalComponents() const { return mTotoalComponents; }
    int getNumberElements() const { return mNumElements; }
    unsigned int getType() const { return mType; }
    bool getNormalize() const { return mNormalize; }
    int getStride() const { return mStride; }
    void *getOffset() const { return mOffset; }
    unsigned int getTarget() const { return mTarget; }
    void loadBuffer(const std::vector<float> &buf);
    void loadBuffer(const std::vector<unsigned short> &buf);

  private:
    ContextGL *mContext;
    unsigned int mBuf;
    unsigned int mTarget;
    int mNumComponents;
    int mTotoalComponents;
    int mNumElements;
    unsigned int mType;
    bool mNormalize;
    int mStride;
    void *mOffset;
};

#endif
