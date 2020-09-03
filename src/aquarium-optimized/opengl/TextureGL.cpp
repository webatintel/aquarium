//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// TextureGL.cpp. Wrap textures of OpenGL. Load image files and wrap into an
// OpenGL texture.

#include "TextureGL.h"

#include "common/AQUARIUM_ASSERT.h"

// initializs texture 2d
TextureGL::TextureGL(ContextGL *context, std::string name, std::string url)
    : Texture(name, url, true),
      mTarget(GL_TEXTURE_2D),
      mFormat(GL_RGBA),
      mContext(context) {
  mTextureId = context->generateTexture();
}

// initializs cube map
TextureGL::TextureGL(ContextGL *context,
                     std::string name,
                     const std::vector<std::string> &urls)
    : Texture(name, urls, false),
      mTarget(GL_TEXTURE_CUBE_MAP),
      mFormat(GL_RGBA),
      mContext(context) {
  ASSERT(urls.size() == 6);
  mTextureId = context->generateTexture();
}

void TextureGL::loadTexture() {
  mContext->bindTexture(mTarget, mTextureId);

  std::vector<unsigned char *> pixelVec;
  loadImage(mUrls, &pixelVec);

  if (mTarget == GL_TEXTURE_CUBE_MAP) {
    for (unsigned int i = 0; i < 6; i++) {
      mContext->uploadTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mFormat,
                              mWidth, mHeight, pixelVec[i]);
    }

    mContext->setParameter(mTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    mContext->setParameter(mTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    mContext->setParameter(mTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    mContext->setParameter(mTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  } else  // GL_TEXTURE_2D
  {
    mContext->uploadTexture(mTarget, mFormat, mWidth, mHeight, pixelVec[0]);

    if (isPowerOf2(mWidth) && isPowerOf2(mHeight)) {
      mContext->setParameter(mTarget, GL_TEXTURE_MIN_FILTER,
                             GL_LINEAR_MIPMAP_LINEAR);
      mContext->generateMipmap(mTarget);
    } else {
      mContext->setParameter(mTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      mContext->setParameter(mTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      mContext->setParameter(mTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    mContext->setParameter(mTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  DestoryImageData(pixelVec);
}

TextureGL::~TextureGL() {
  mContext->deleteTexture(mTextureId);
}
