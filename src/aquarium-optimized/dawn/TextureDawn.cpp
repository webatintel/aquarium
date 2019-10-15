//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// TextureDawn.cpp: Wrap textures of Dawn. Load image files and wrap into a Dawn texture.

#include <algorithm>
#include <cmath>

#include "ContextDawn.h"
#include "TextureDawn.h"

#include "common/AQUARIUM_ASSERT.h"

TextureDawn::~TextureDawn() {

    DestoryImageData(mPixelVec);
    DestoryImageData(mResizedVec);
    mTextureView = nullptr;
    mTexture     = nullptr;
    mSampler     = nullptr;
}

TextureDawn::TextureDawn(ContextDawn *context, const std::string &name, const std::string &url)
    : Texture(name, url, true),
      mTextureDimension(dawn::TextureDimension::e2D),
      mTextureViewDimension(dawn::TextureViewDimension::e2D),
      mTexture(nullptr),
      mSampler(nullptr),
      mFormat(dawn::TextureFormat::RGBA8Unorm),
      mTextureView(nullptr),
      mContext(context)
{
}

TextureDawn::TextureDawn(ContextDawn *context,
                         const std::string &name,
                         const std::vector<std::string> &urls)
    : Texture(name, urls, false),
      mTextureDimension(dawn::TextureDimension::e2D),
      mTextureViewDimension(dawn::TextureViewDimension::Cube),
      mFormat(dawn::TextureFormat::RGBA8Unorm),
      mContext(context)
{
}

void TextureDawn::loadTexture()
{
    dawn::SamplerDescriptor samplerDesc;
    const int kPadding = 256;
    loadImage(mUrls, &mPixelVec);

    if (mTextureViewDimension == dawn::TextureViewDimension::Cube)
    {
        dawn::TextureDescriptor descriptor;
        descriptor.dimension = mTextureDimension;
        descriptor.size.width = mWidth;
        descriptor.size.height = mHeight;
        descriptor.size.depth = 1;
        descriptor.arrayLayerCount = 6;
        descriptor.sampleCount = 1;
        descriptor.format = mFormat;
        descriptor.mipLevelCount   = 1;
        descriptor.usage           = dawn::TextureUsage::CopyDst | dawn::TextureUsage::Sampled;
        mTexture                   = mContext->createTexture(descriptor);

        for (unsigned int i = 0; i < 6; i++)
        {
            dawn::CreateBufferMappedResult result = mContext->CreateBufferMapped(
                dawn::BufferUsage::CopySrc | dawn::BufferUsage::MapWrite, mWidth * mHeight * 4);
            memcpy(result.data, mPixelVec[i], mWidth * mHeight * 4);
            result.buffer.Unmap();

            dawn::BufferCopyView bufferCopyView =
                mContext->createBufferCopyView(result.buffer, 0, mWidth * 4, mHeight);
            dawn::TextureCopyView textureCopyView =
                mContext->createTextureCopyView(mTexture, 0, i, {0, 0, 0});
            dawn::Extent3D copySize = { static_cast<uint32_t>(mWidth), static_cast<uint32_t>(mHeight), 1 };
            mContext->mCommandBuffers.emplace_back(
                mContext->copyBufferToTexture(bufferCopyView, textureCopyView, copySize));
        }

        dawn::TextureViewDescriptor viewDescriptor;
        viewDescriptor.nextInChain = nullptr;
        viewDescriptor.dimension = dawn::TextureViewDimension::Cube;
        viewDescriptor.format = mFormat;
        viewDescriptor.baseMipLevel = 0;
        viewDescriptor.mipLevelCount   = 1;
        viewDescriptor.baseArrayLayer = 0;
        viewDescriptor.arrayLayerCount = 6;

        mTextureView = mTexture.CreateView(&viewDescriptor);

        samplerDesc.addressModeU = dawn::AddressMode::ClampToEdge;
        samplerDesc.addressModeV = dawn::AddressMode::ClampToEdge;
        samplerDesc.addressModeW = dawn::AddressMode::ClampToEdge;
        samplerDesc.minFilter = dawn::FilterMode::Linear;
        samplerDesc.magFilter = dawn::FilterMode::Linear;
        samplerDesc.mipmapFilter = dawn::FilterMode::Nearest;
        samplerDesc.lodMinClamp  = 0.0f;
        samplerDesc.lodMaxClamp  = 1000.0f;
        samplerDesc.compare = dawn::CompareFunction::Never;

        mSampler = mContext->createSampler(samplerDesc);
    }
    else  // dawn::TextureViewDimension::e2D
    {
        int resizedWidth;
        if (mWidth % kPadding == 0)
        {
            resizedWidth = mWidth;
        }
        else
        {
            resizedWidth = (mWidth / 256 + 1) * 256;
        }
        generateMipmap(mPixelVec[0], mWidth, mHeight, 0, mResizedVec, resizedWidth, mHeight, 0, 4,
                       true);

        dawn::TextureDescriptor descriptor;
        descriptor.dimension = mTextureDimension;
        descriptor.size.width  = resizedWidth;
        descriptor.size.height = mHeight;
        descriptor.size.depth = 1;
        descriptor.arrayLayerCount = 1;
        descriptor.sampleCount = 1;
        descriptor.format = mFormat;
        descriptor.mipLevelCount   = static_cast<uint32_t>(std::floor(
                                       static_cast<float>(std::log2(std::min(mWidth, mHeight))))) +
                                   1;
        descriptor.usage = dawn::TextureUsage::CopyDst | dawn::TextureUsage::Sampled;
        mTexture         = mContext->createTexture(descriptor);

        int count = 0;
        for (unsigned int i = 0; i < descriptor.mipLevelCount; ++i, ++count)
        {
            int height                 = mHeight >> i;
            int width                  = resizedWidth >> i;
            if (height == 0)
            {
                height = 1;
            }

            dawn::CreateBufferMappedResult result = mContext->CreateBufferMapped(
                dawn::BufferUsage::CopySrc | dawn::BufferUsage::MapWrite,
                resizedWidth * height * 4);
            memcpy(result.data, mResizedVec[i], resizedWidth * height * 4);
            result.buffer.Unmap();

            dawn::BufferCopyView bufferCopyView =
                mContext->createBufferCopyView(result.buffer, 0, resizedWidth * 4, height);
            dawn::TextureCopyView textureCopyView =
                mContext->createTextureCopyView(mTexture, i, 0, {0, 0, 0});
            dawn::Extent3D copySize = {static_cast<uint32_t>(width),
                                       static_cast<uint32_t>(height),
                                       1};
            mContext->mCommandBuffers.emplace_back(
                mContext->copyBufferToTexture(bufferCopyView, textureCopyView, copySize));
        }

        dawn::TextureViewDescriptor viewDescriptor;
        viewDescriptor.nextInChain = nullptr;
        viewDescriptor.dimension = dawn::TextureViewDimension::e2D;
        viewDescriptor.format = mFormat;
        viewDescriptor.baseMipLevel = 0;
        viewDescriptor.mipLevelCount =
            static_cast<uint32_t>(
                std::floor(static_cast<float>(std::log2(std::min(mWidth, mHeight))))) +
            1;
        viewDescriptor.baseArrayLayer = 0;
        viewDescriptor.arrayLayerCount = 1;

        mTextureView = mTexture.CreateView(&viewDescriptor);

        samplerDesc.addressModeU = dawn::AddressMode::ClampToEdge;
        samplerDesc.addressModeV = dawn::AddressMode::ClampToEdge;
        samplerDesc.addressModeW = dawn::AddressMode::ClampToEdge;
        samplerDesc.minFilter = dawn::FilterMode::Linear;
        samplerDesc.magFilter = dawn::FilterMode::Linear;
        samplerDesc.lodMinClamp  = 0.0f;
        samplerDesc.lodMaxClamp  = 1000.0f;
        samplerDesc.compare = dawn::CompareFunction::Never;

        if (isPowerOf2(mWidth) && isPowerOf2(mHeight))
        {
            samplerDesc.mipmapFilter = dawn::FilterMode::Linear;
        }
        else
        {
            samplerDesc.mipmapFilter = dawn::FilterMode::Nearest;
        }

        mSampler = mContext->createSampler(samplerDesc);
    }

    // TODO(yizhou): check if the pixel destory should delay or fence
}

