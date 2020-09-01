//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// TextureGL.h: Define Texture  wrapper class of OpenGL.

#ifndef TEXTUREGL_H
#define TEXTUREGL_H

#include <string>
#include <unordered_map>
#include <vector>

#ifdef EGL_EGL_PROTOTYPES
#include <memory>

#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "EGL/eglext_angle.h"
#include "EGL/eglplatform.h"
#include "EGLWindow.h"
#include "angle_gl.h"
#else
#include "glad/glad.h"
#endif

#include "../Texture.h"
#include "ContextGL.h"

class ContextGL;

class TextureGL : public Texture
{
  public:
    ~TextureGL() override;
    TextureGL(ContextGL *context, std::string name, std::string url);
    TextureGL(ContextGL *context, std::string name, const std::vector<std::string> &urls);

    unsigned int getTextureId() const { return mTextureId; }
    unsigned int getTarget() const { return mTarget; }
    void setTextureId(unsigned int texId) { mTextureId = texId; }

    void loadTexture() override;

  private:
    unsigned int mTarget;
    unsigned int mTextureId;
    unsigned int mFormat;
    ContextGL *mContext;
};

#endif  // TEXTUREGL_H
