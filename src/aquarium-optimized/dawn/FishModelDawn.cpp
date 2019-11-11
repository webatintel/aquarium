//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelDawn.cpp: Implements fish model of Dawn.

#include "BufferDawn.h" 
#include "FishModelDawn.h"

#include <iostream>

FishModelDawn::FishModelDawn(Context *context,
                             Aquarium *aquarium,
                             MODELGROUP type,
                             MODELNAME name,
                             bool blend)
    : FishModel(type, name, blend), mAquarium(aquarium), mFishPerOffset(0)
{
    mContextDawn = static_cast<ContextDawn *>(context);

    mEnableDynamicBufferOffset =
        aquarium->toggleBitset.test(static_cast<size_t>(TOGGLE::ENABLEDYNAMICBUFFEROFFSET));

    mLightFactorUniforms.shininess      = 5.0f;
    mLightFactorUniforms.specularFactor = 0.3f;

    const Fish &fishInfo              = fishTable[name - MODELNAME::MODELSMALLFISHA];
    mFishVertexUniforms.fishLength     = fishInfo.fishLength;
    mFishVertexUniforms.fishBendAmount = fishInfo.fishBendAmount;
    mFishVertexUniforms.fishWaveLength = fishInfo.fishWaveLength;

    mCurInstance = mAquarium->fishCount[fishInfo.modelName - MODELNAME::MODELSMALLFISHA];
    mPreInstance = mCurInstance;
}

void FishModelDawn::init()
{
    mProgramDawn = static_cast<ProgramDawn *>(mProgram);

    mDiffuseTexture    = static_cast<TextureDawn *>(textureMap["diffuse"]);
    mNormalTexture     = static_cast<TextureDawn *>(textureMap["normalMap"]);
    mReflectionTexture = static_cast<TextureDawn *>(textureMap["reflectionMap"]);
    mSkyboxTexture     = static_cast<TextureDawn *>(textureMap["skybox"]);

    mPositionBuffer = static_cast<BufferDawn *>(bufferMap["position"]);
    mNormalBuffer   = static_cast<BufferDawn *>(bufferMap["normal"]);
    mTexCoordBuffer = static_cast<BufferDawn *>(bufferMap["texCoord"]);
    mTangentBuffer  = static_cast<BufferDawn *>(bufferMap["tangent"]);
    mBiNormalBuffer = static_cast<BufferDawn *>(bufferMap["binormal"]);
    mIndicesBuffer  = static_cast<BufferDawn *>(bufferMap["indices"]);

    mVertexStateDescriptor.cVertexBuffers[0].attributeCount = 1;
    mVertexStateDescriptor.cVertexBuffers[0].arrayStride    = mPositionBuffer->getDataSize();
    mVertexStateDescriptor.cAttributes[0].format            = wgpu::VertexFormat::Float3;
    mVertexStateDescriptor.cAttributes[0].shaderLocation    = 0;
    mVertexStateDescriptor.cAttributes[0].offset            = 0;
    mVertexStateDescriptor.cVertexBuffers[0].attributes = &mVertexStateDescriptor.cAttributes[0];
    mVertexStateDescriptor.cVertexBuffers[1].attributeCount = 1;
    mVertexStateDescriptor.cVertexBuffers[1].arrayStride    = mNormalBuffer->getDataSize();
    mVertexStateDescriptor.cAttributes[1].format            = wgpu::VertexFormat::Float3;
    mVertexStateDescriptor.cAttributes[1].shaderLocation    = 1;
    mVertexStateDescriptor.cAttributes[1].offset            = 0;
    mVertexStateDescriptor.cVertexBuffers[1].attributes = &mVertexStateDescriptor.cAttributes[1];
    mVertexStateDescriptor.cVertexBuffers[2].attributeCount = 1;
    mVertexStateDescriptor.cVertexBuffers[2].arrayStride    = mTexCoordBuffer->getDataSize();
    mVertexStateDescriptor.cAttributes[2].format            = wgpu::VertexFormat::Float2;
    mVertexStateDescriptor.cAttributes[2].shaderLocation    = 2;
    mVertexStateDescriptor.cAttributes[2].offset            = 0;
    mVertexStateDescriptor.cVertexBuffers[2].attributes = &mVertexStateDescriptor.cAttributes[2];
    mVertexStateDescriptor.cVertexBuffers[3].attributeCount = 1;
    mVertexStateDescriptor.cVertexBuffers[3].arrayStride    = mTangentBuffer->getDataSize();
    mVertexStateDescriptor.cAttributes[3].format            = wgpu::VertexFormat::Float3;
    mVertexStateDescriptor.cAttributes[3].shaderLocation    = 3;
    mVertexStateDescriptor.cAttributes[3].offset            = 0;
    mVertexStateDescriptor.cVertexBuffers[3].attributes = &mVertexStateDescriptor.cAttributes[3];
    mVertexStateDescriptor.cVertexBuffers[4].attributeCount = 1;
    mVertexStateDescriptor.cVertexBuffers[4].arrayStride    = mBiNormalBuffer->getDataSize();
    mVertexStateDescriptor.cAttributes[4].format            = wgpu::VertexFormat::Float3;
    mVertexStateDescriptor.cAttributes[4].shaderLocation    = 4;
    mVertexStateDescriptor.cAttributes[4].offset            = 0;
    mVertexStateDescriptor.cVertexBuffers[4].attributes = &mVertexStateDescriptor.cAttributes[4];
    mVertexStateDescriptor.vertexBufferCount            = 5;
    mVertexStateDescriptor.indexFormat                  = wgpu::IndexFormat::Uint16;

    if (mSkyboxTexture && mReflectionTexture)
    {
        mGroupLayoutModel = mContextDawn->MakeBindGroupLayout({
            {0, wgpu::ShaderStage::Vertex, wgpu::BindingType::UniformBuffer},
            {1, wgpu::ShaderStage::Fragment, wgpu::BindingType::UniformBuffer},
            {2, wgpu::ShaderStage::Fragment, wgpu::BindingType::Sampler},
            {3, wgpu::ShaderStage::Fragment, wgpu::BindingType::Sampler},
            {4, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture, false, false,
             wgpu::TextureViewDimension::e2D, wgpu::TextureComponentType::Float},
            {5, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture, false, false,
             wgpu::TextureViewDimension::e2D, wgpu::TextureComponentType::Float},
            {6, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture, false, false,
             wgpu::TextureViewDimension::e2D, wgpu::TextureComponentType::Float},
            {7, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture, false, false,
             wgpu::TextureViewDimension::Cube, wgpu::TextureComponentType::Float},
        });
    }
    else
    {
        mGroupLayoutModel = mContextDawn->MakeBindGroupLayout({
            {0, wgpu::ShaderStage::Vertex, wgpu::BindingType::UniformBuffer},
            {1, wgpu::ShaderStage::Fragment, wgpu::BindingType::UniformBuffer},
            {2, wgpu::ShaderStage::Fragment, wgpu::BindingType::Sampler},
            {3, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture, false, false,
             wgpu::TextureViewDimension::e2D, wgpu::TextureComponentType::Float},
            {4, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture, false, false,
             wgpu::TextureViewDimension::e2D, wgpu::TextureComponentType::Float},
        });
    }

    mPipelineLayout = mContextDawn->MakeBasicPipelineLayout({
        mContextDawn->groupLayoutGeneral,
        mContextDawn->groupLayoutWorld,
        mGroupLayoutModel,
        mContextDawn->groupLayoutFishPer,
    });

    mPipeline = mContextDawn->createRenderPipeline(mPipelineLayout, mProgramDawn,
                                                   mVertexStateDescriptor, mBlend);

    mFishVertexBuffer =
        mContextDawn->createBufferFromData(&mFishVertexUniforms, sizeof(FishVertexUniforms),
                                           wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);
    mLightFactorBuffer =
        mContextDawn->createBufferFromData(&mLightFactorUniforms, sizeof(LightFactorUniforms),
                                           wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);

    // Fish models includes small, medium and big. Some of them contains reflection and skybox
    // texture, but some doesn't.
    if (mSkyboxTexture && mReflectionTexture)
    {
        mBindGroupModel = mContextDawn->makeBindGroup(
            mGroupLayoutModel, {{0, mFishVertexBuffer, 0, sizeof(FishVertexUniforms)},
                                {1, mLightFactorBuffer, 0, sizeof(LightFactorUniforms)},
                                {2, mReflectionTexture->getSampler()},
                                {3, mSkyboxTexture->getSampler()},
                                {4, mDiffuseTexture->getTextureView()},
                                {5, mNormalTexture->getTextureView()},
                                {6, mReflectionTexture->getTextureView()},
                                {7, mSkyboxTexture->getTextureView()}});
    }
    else
    {
        mBindGroupModel = mContextDawn->makeBindGroup(
            mGroupLayoutModel, {{0, mFishVertexBuffer, 0, sizeof(FishVertexUniforms)},
                                {1, mLightFactorBuffer, 0, sizeof(LightFactorUniforms)},
                                {2, mDiffuseTexture->getSampler()},
                                {3, mDiffuseTexture->getTextureView()},
                                {4, mNormalTexture->getTextureView()}});
    }

    mContextDawn->setBufferData(mLightFactorBuffer, 0, sizeof(LightFactorUniforms),
                                &mLightFactorUniforms);
    mContextDawn->setBufferData(mFishVertexBuffer, 0, sizeof(FishVertexUniforms),
                                &mFishVertexUniforms);
}

void FishModelDawn::prepareForDraw()
{
    mFishPerOffset = 0;
    for (int i = 0; i < mName - MODELNAME::MODELSMALLFISHA; i++)
    {
        const Fish &fishInfo = fishTable[i];
        mFishPerOffset += mAquarium->fishCount[fishInfo.modelName - MODELNAME::MODELSMALLFISHA];
    }

    const Fish &fishInfo = fishTable[mName - MODELNAME::MODELSMALLFISHA];
    mCurInstance         = mAquarium->fishCount[fishInfo.modelName - MODELNAME::MODELSMALLFISHA];
}

void FishModelDawn::draw()
{
    if (mCurInstance == 0)
        return;

    wgpu::RenderPassEncoder pass = mContextDawn->getRenderPass();
    pass.SetPipeline(mPipeline);
    pass.SetBindGroup(0, mContextDawn->bindGroupGeneral, 0, nullptr);
    pass.SetBindGroup(1, mContextDawn->bindGroupWorld, 0, nullptr);
    pass.SetBindGroup(2, mBindGroupModel, 0, nullptr);
    pass.SetVertexBuffer(0, mPositionBuffer->getBuffer());
    pass.SetVertexBuffer(1, mNormalBuffer->getBuffer());
    pass.SetVertexBuffer(2, mTexCoordBuffer->getBuffer());
    pass.SetVertexBuffer(3, mTangentBuffer->getBuffer());
    pass.SetVertexBuffer(4, mBiNormalBuffer->getBuffer());
    pass.SetIndexBuffer(mIndicesBuffer->getBuffer(), 0);

    if (mEnableDynamicBufferOffset)
    {
        for (int i = 0; i < mCurInstance; i++)
        {
            uint32_t offset = 256u * (i + mFishPerOffset);
            pass.SetBindGroup(3, mContextDawn->bindGroupFishPers[0], 1, &offset);
            pass.DrawIndexed(mIndicesBuffer->getTotalComponents(), 1, 0, 0, 0);
        }
    }
    else
    {
        for (int i = 0; i < mCurInstance; i++)
        {
            pass.SetBindGroup(3, mContextDawn->bindGroupFishPers[i + mFishPerOffset], 0, nullptr);
            pass.DrawIndexed(mIndicesBuffer->getTotalComponents(), 1, 0, 0, 0);
        }
    }
}

void FishModelDawn::updatePerInstanceUniforms(const WorldUniforms &worldUniforms) {}

void FishModelDawn::updateFishPerUniforms(float x,
                                          float y,
                                          float z,
                                          float nextX,
                                          float nextY,
                                          float nextZ,
                                          float scale,
                                          float time,
                                          int index)
{
    index += mFishPerOffset;
    mContextDawn->fishPers[index].worldPosition[0] = x;
    mContextDawn->fishPers[index].worldPosition[1] = y;
    mContextDawn->fishPers[index].worldPosition[2] = z;
    mContextDawn->fishPers[index].nextPosition[0]  = nextX;
    mContextDawn->fishPers[index].nextPosition[1]  = nextY;
    mContextDawn->fishPers[index].nextPosition[2]  = nextZ;
    mContextDawn->fishPers[index].scale            = scale;
    mContextDawn->fishPers[index].time             = time;
}

FishModelDawn::~FishModelDawn()
{
    mPipeline          = nullptr;
    mGroupLayoutModel  = nullptr;
    mPipelineLayout    = nullptr;
    mBindGroupModel    = nullptr;
    mFishVertexBuffer  = nullptr;
    mLightFactorBuffer = nullptr;
}
