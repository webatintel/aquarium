//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "TextureD3D12.h"

#include <algorithm>
#include <cmath>

#include "ContextD3D12.h"

TextureD3D12::~TextureD3D12() {
}

TextureD3D12::TextureD3D12(ContextD3D12 *context,
                           const std::string &name,
                           const std::string &url)
    : Texture(name, url, true),
      mTextureDimension(D3D12_RESOURCE_DIMENSION_TEXTURE2D),
      mTextureViewDimension(D3D12_SRV_DIMENSION_TEXTURE2D),
      mFormat(DXGI_FORMAT_R8G8B8A8_UNORM),
      mSrvDesc({}),
      mContext(context) {
}

TextureD3D12::TextureD3D12(ContextD3D12 *context,
                           const std::string &name,
                           const std::vector<std::string> &urls)
    : Texture(name, urls, false),
      mTextureDimension(D3D12_RESOURCE_DIMENSION_TEXTURE2D),
      mTextureViewDimension(D3D12_SRV_DIMENSION_TEXTURECUBE),
      mFormat(DXGI_FORMAT_R8G8B8A8_UNORM),
      mSrvDesc({}),
      mContext(context) {
}

void TextureD3D12::loadTexture() {
  loadImage(mUrls, &mPixelVec);

  if (mTextureViewDimension == D3D12_SRV_DIMENSION_TEXTURECUBE) {
    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels           = 1;
    textureDesc.Format              = mFormat;
    textureDesc.Width               = mWidth;
    textureDesc.Height              = mHeight;
    textureDesc.Flags               = D3D12_RESOURCE_FLAG_NONE;
    textureDesc.DepthOrArraySize    = 6;
    textureDesc.SampleDesc.Count    = 1;
    textureDesc.SampleDesc.Quality  = 0;
    textureDesc.Dimension           = mTextureDimension;

    mContext->createTexture(
        textureDesc, mPixelVec, mTexture, mTextureUploadHeap, mWidth, mHeight,
        4u, textureDesc.MipLevels, textureDesc.DepthOrArraySize);
  } else {
    generateMipmap(mPixelVec[0], mWidth, mHeight, 0, mResizedVec, mWidth,
                   mHeight, 0, 4, false);

    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels           = static_cast<uint16_t>(std::floor(
                                std::log2(std::max(mWidth, mHeight)))) +
                            1;
    textureDesc.Format             = mFormat;
    textureDesc.Width              = mWidth;
    textureDesc.Height             = mHeight;
    textureDesc.Flags              = D3D12_RESOURCE_FLAG_NONE;
    textureDesc.DepthOrArraySize   = 1;
    textureDesc.SampleDesc.Count   = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension          = mTextureDimension;

    mContext->createTexture(
        textureDesc, mResizedVec, mTexture, mTextureUploadHeap, mWidth, mHeight,
        4u, textureDesc.MipLevels, textureDesc.DepthOrArraySize);
  }
}

// Allocate descriptors sequentially on deascriptor heap to bind root signature,
// create srv before binding resources.
void TextureD3D12::createSrvDescriptor() {
  mSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
  mSrvDesc.Format                  = mFormat;
  mSrvDesc.ViewDimension           = mTextureViewDimension;
  if (mTextureViewDimension == D3D12_SRV_DIMENSION_TEXTURECUBE) {
    mSrvDesc.Texture2D.MipLevels = 1;
  } else {

    mSrvDesc.Texture2D.MipLevels = static_cast<uint32_t>(std::floor(
                                       std::log2(std::max(mWidth, mHeight)))) +
                                   1;
  }

  mContext->buildSrvDescriptor(mTexture.Get(), mSrvDesc, &mTextureGPUHandle);
}
