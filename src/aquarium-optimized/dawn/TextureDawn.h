//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// TextureDawn.h: Wrap textures of Dawn.

#pragma once
#ifndef TEXTUREDAWN_H
#define TEXTUREDAWN_H

#include <dawn/webgpu_cpp.h>

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

    const wgpu::Texture &getTextureId() const { return mTexture; }
    const wgpu::Sampler &getSampler() const { return mSampler; }
    wgpu::TextureDimension getTextureDimension() { return mTextureDimension; }
    wgpu::TextureViewDimension getTextureViewDimension() { return mTextureViewDimension; }
    wgpu::TextureView getTextureView() { return mTextureView; }

    void loadTexture() override;

  private:
    wgpu::TextureDimension mTextureDimension;  // texture 2D or CubeMap
    wgpu::TextureViewDimension mTextureViewDimension;
    wgpu::Texture mTexture;
    wgpu::Sampler mSampler;
    wgpu::TextureFormat mFormat;
    wgpu::TextureView mTextureView;
    std::vector<unsigned char *> mPixelVec;
    std::vector<unsigned char *> mResizedVec;
    ContextDawn *mContext;
};

#endif // !TEXTUREDAWN_H