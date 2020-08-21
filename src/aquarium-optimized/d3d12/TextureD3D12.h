//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// TextureD3D12.h: Wrap textures of D3D12.

#ifndef TEXTURED3D12_H
#define TEXTURED3D12_H

#include <vector>

#include "stdafx.h"

#include "../Texture.h"

using Microsoft::WRL::ComPtr;

class ContextD3D12;

class TextureD3D12 : public Texture
{
  public:
    ~TextureD3D12() override;
    TextureD3D12(ContextD3D12 *context, const std::string &name, const std::string &url);
    TextureD3D12(ContextD3D12 *context,
                 const std::string &name,
                 const std::vector<std::string> &urls);

    D3D12_RESOURCE_DIMENSION getTextureDimension() { return mTextureDimension; }
    D3D12_SRV_DIMENSION getTextureViewDimension() { return mTextureViewDimension; }
    D3D12_GPU_DESCRIPTOR_HANDLE getTextureGPUHandle() { return mTextureGPUHandle; }
    void loadTexture() override;
    void createSrvDescriptor();

  private:
    D3D12_RESOURCE_DIMENSION mTextureDimension;
    D3D12_SRV_DIMENSION mTextureViewDimension;
    DXGI_FORMAT mFormat;
    ComPtr<ID3D12Resource> mTexture;
    ComPtr<ID3D12Resource> mTextureUploadHeap;
    D3D12_SHADER_RESOURCE_VIEW_DESC mSrvDesc;
    D3D12_GPU_DESCRIPTOR_HANDLE mTextureGPUHandle;

    std::vector<unsigned char *> mPixelVec;
    std::vector<unsigned char *> mResizedVec;
    ContextD3D12 *mContext;
};

#endif  // TEXTURED3D12_H
