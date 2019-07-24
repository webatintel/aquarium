//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// TextureDawn.h: Wrap textures of Dawn.

#pragma once
#ifndef TEXTUREDAWN_H
#define TEXTUREDAWN_H

#include <dawn/dawncpp.h>

#include "../Texture.h"

class ContextDawn;

class TextureDawn : public Texture
{
  public:
    ~TextureDawn() override;
    TextureDawn(ContextDawn *context, const std::string &name, const std::string &url);
    TextureDawn(ContextDawn *context,
                const std::string &name,
                const std::vector<std::string> &urls);

    const dawn::Texture &getTextureId() const { return mTexture; }
    const dawn::Sampler &getSampler() const { return mSampler; }
    dawn::TextureDimension getTextureDimension() { return mTextureDimension; }
    dawn::TextureViewDimension getTextureViewDimension() { return mTextureViewDimension; }
    dawn::TextureView getTextureView() { return mTextureView; }

    void loadTexture() override;

  private:
    dawn::TextureDimension mTextureDimension;  // texture 2D or CubeMap
    dawn::TextureViewDimension mTextureViewDimension;
    dawn::Texture mTexture;
    dawn::Sampler mSampler;
    dawn::TextureFormat mFormat;
    dawn::TextureView mTextureView;
    std::vector<unsigned char *> mPixelVec;
    std::vector<unsigned char *> mResizedVec;
    ContextDawn *mContext;
};

#endif // !TEXTUREDAWN_H