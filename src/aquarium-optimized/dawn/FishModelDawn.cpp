//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelDawn.cpp: Implements fish model of Dawn.

#include "BufferDawn.h" 
#include "FishModelDawn.h"

FishModelDawn::FishModelDawn(const Context *context,
                             Aquarium *aquarium,
                             MODELGROUP type,
                             MODELNAME name,
                             bool blend)
    : FishModel(type, name, blend), mFishPers(nullptr), mBindGroupPers(nullptr), mAquarium(aquarium)
{
    mContextDawn = static_cast<const ContextDawn *>(context);

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

    mVertexInputDescriptor.cBuffers[0].attributeCount    = 1;
    mVertexInputDescriptor.cBuffers[0].stride            = mPositionBuffer->getDataSize();
    mVertexInputDescriptor.cAttributes[0].format         = dawn::VertexFormat::Float3;
    mVertexInputDescriptor.cAttributes[0].shaderLocation = 0;
    mVertexInputDescriptor.cAttributes[0].offset         = 0;
    mVertexInputDescriptor.cBuffers[0].attributes        = &mVertexInputDescriptor.cAttributes[0];
    mVertexInputDescriptor.cBuffers[1].attributeCount    = 1;
    mVertexInputDescriptor.cBuffers[1].stride            = mNormalBuffer->getDataSize();
    mVertexInputDescriptor.cAttributes[1].format         = dawn::VertexFormat::Float3;
    mVertexInputDescriptor.cAttributes[1].shaderLocation = 1;
    mVertexInputDescriptor.cAttributes[1].offset         = 0;
    mVertexInputDescriptor.cBuffers[1].attributes        = &mVertexInputDescriptor.cAttributes[1];
    mVertexInputDescriptor.cBuffers[2].attributeCount    = 1;
    mVertexInputDescriptor.cBuffers[2].stride            = mTexCoordBuffer->getDataSize();
    mVertexInputDescriptor.cAttributes[2].format         = dawn::VertexFormat::Float2;
    mVertexInputDescriptor.cAttributes[2].shaderLocation = 2;
    mVertexInputDescriptor.cAttributes[2].offset         = 0;
    mVertexInputDescriptor.cBuffers[2].attributes        = &mVertexInputDescriptor.cAttributes[2];
    mVertexInputDescriptor.cBuffers[3].attributeCount    = 1;
    mVertexInputDescriptor.cBuffers[3].stride            = mTangentBuffer->getDataSize();
    mVertexInputDescriptor.cAttributes[3].format         = dawn::VertexFormat::Float3;
    mVertexInputDescriptor.cAttributes[3].shaderLocation = 3;
    mVertexInputDescriptor.cAttributes[3].offset         = 0;
    mVertexInputDescriptor.cBuffers[3].attributes        = &mVertexInputDescriptor.cAttributes[3];
    mVertexInputDescriptor.cBuffers[4].attributeCount    = 1;
    mVertexInputDescriptor.cBuffers[4].stride            = mBiNormalBuffer->getDataSize();
    mVertexInputDescriptor.cAttributes[4].format         = dawn::VertexFormat::Float3;
    mVertexInputDescriptor.cAttributes[4].shaderLocation = 4;
    mVertexInputDescriptor.cAttributes[4].offset         = 0;
    mVertexInputDescriptor.cBuffers[4].attributes        = &mVertexInputDescriptor.cAttributes[4];
    mVertexInputDescriptor.bufferCount                   = 5;
    mVertexInputDescriptor.indexFormat                   = dawn::IndexFormat::Uint16;

    if (mSkyboxTexture && mReflectionTexture)
    {
        mGroupLayoutModel = mContextDawn->MakeBindGroupLayout({
            {0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
            {1, dawn::ShaderStageBit::Fragment, dawn::BindingType::UniformBuffer},
            {2, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler},
            {3, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler},
            {4, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
            {5, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
            {6, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
            {7, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
        });
    }
    else
    {
        mGroupLayoutModel = mContextDawn->MakeBindGroupLayout({
            {0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
            {1, dawn::ShaderStageBit::Fragment, dawn::BindingType::UniformBuffer},
            {2, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler},
            {3, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
            {4, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
        });
    }

    if (mEnableDynamicBufferOffset)
    {
        mGroupLayoutPer = mContextDawn->MakeBindGroupLayout({
            {0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer, true},
        });
    }
    else
    {
        mGroupLayoutPer = mContextDawn->MakeBindGroupLayout({
            {0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
        });
    }

    mPipelineLayout = mContextDawn->MakeBasicPipelineLayout({
        mContextDawn->groupLayoutGeneral,
        mContextDawn->groupLayoutWorld,
        mGroupLayoutModel,
        mGroupLayoutPer,
    });

    mPipeline = mContextDawn->createRenderPipeline(mPipelineLayout, mProgramDawn,
                                                   mVertexInputDescriptor, mBlend);

    mFishVertexBuffer = mContextDawn->createBufferFromData(
        &mFishVertexUniforms, sizeof(FishVertexUniforms),
        dawn::BufferUsageBit::CopyDst | dawn::BufferUsageBit::Uniform);
    mLightFactorBuffer = mContextDawn->createBufferFromData(
        &mLightFactorUniforms, sizeof(LightFactorUniforms),
        dawn::BufferUsageBit::CopyDst | dawn::BufferUsageBit::Uniform);

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

    reallocResource();
}

void FishModelDawn::prepareForDraw()
{
    const Fish &fishInfo = fishTable[mName - MODELNAME::MODELSMALLFISHA];
    mCurInstance         = mAquarium->fishCount[fishInfo.modelName - MODELNAME::MODELSMALLFISHA];
    if (mCurInstance != mPreInstance)
    {
        destoryFishResource();
        reallocResource();
        mPreInstance = mCurInstance;
    }
}

void FishModelDawn::draw()
{
    if (mCurInstance == 0)
        return;

    uint64_t vertexBufferOffsets[1] = {0};

    mContextDawn->setBufferData(mFishPersBuffer, 0, sizeof(FishPer) * mCurInstance, mFishPers);

    dawn::RenderPassEncoder pass = mContextDawn->getRenderPass();
    pass.SetPipeline(mPipeline);
    pass.SetBindGroup(0, mContextDawn->bindGroupGeneral, 0, nullptr);
    pass.SetBindGroup(1, mContextDawn->bindGroupWorld, 0, nullptr);
    pass.SetBindGroup(2, mBindGroupModel, 0, nullptr);
    pass.SetVertexBuffers(0, 1, &mPositionBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(1, 1, &mNormalBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(2, 1, &mTexCoordBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(3, 1, &mTangentBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(4, 1, &mBiNormalBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetIndexBuffer(mIndicesBuffer->getBuffer(), 0);

    if (mEnableDynamicBufferOffset)
    {
        for (int i = 0; i < mCurInstance; i++)
        {
            uint64_t offset = 256u * i;
            pass.SetBindGroup(3, mBindGroupPers[0], 1, &offset);
            pass.DrawIndexed(mIndicesBuffer->getTotalComponents(), 1, 0, 0, 0);
        }
    }
    else
    {
        for (int i = 0; i < mCurInstance; i++)
        {
            pass.SetBindGroup(3, mBindGroupPers[i], 0, nullptr);
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
    mFishPers[index].worldPosition[0] = x;
    mFishPers[index].worldPosition[1] = y;
    mFishPers[index].worldPosition[2] = z;
    mFishPers[index].nextPosition[0]  = nextX;
    mFishPers[index].nextPosition[1]  = nextY;
    mFishPers[index].nextPosition[2]  = nextZ;
    mFishPers[index].scale            = scale;
    mFishPers[index].time             = time;
}

void FishModelDawn::reallocResource()
{
    if (mCurInstance == 0)
        return;

    mFishPers = new FishPer[mCurInstance];
    if (mEnableDynamicBufferOffset)
    {
        mBindGroupPers = new dawn::BindGroup[1];
    }
    else
    {
        mBindGroupPers = new dawn::BindGroup[mCurInstance];
    }

    mFishPersBuffer = mContextDawn->createBufferFromData(
        mFishPers, sizeof(FishPer) * mCurInstance,
        dawn::BufferUsageBit::CopyDst | dawn::BufferUsageBit::Uniform);

    if (mEnableDynamicBufferOffset)
    {
        mBindGroupPers[0] = mContextDawn->makeBindGroup(mGroupLayoutPer,
                                                        {{0, mFishPersBuffer, 0, sizeof(FishPer)}});
    }
    else
    {
        for (int i = 0; i < mCurInstance; i++)
        {
            mBindGroupPers[i] = mContextDawn->makeBindGroup(
                mGroupLayoutPer, {{0, mFishPersBuffer, sizeof(FishPer) * i, sizeof(FishPer)}});
        }
    }
}

void FishModelDawn::destoryFishResource()
{
    mFishPersBuffer = nullptr;
    if (mFishPers != nullptr)
    {
        delete mFishPers;
        mFishPers = nullptr;
    }
    if (mEnableDynamicBufferOffset)
    {
        if (mBindGroupPers != nullptr)
        {
            if (mBindGroupPers[0].Get() != nullptr)
            {
                mBindGroupPers[0] = nullptr;
            }
        }
    }
    else
    {
        if (mBindGroupPers != nullptr)
        {
            for (int i = 0; i < mPreInstance; i++)
            {
                if (mBindGroupPers[i].Get() != nullptr)
                {
                    mBindGroupPers[i] = nullptr;
                }
            }
        }
    }

    mBindGroupPers = nullptr;
}

FishModelDawn::~FishModelDawn()
{
    destoryFishResource();

    mPipeline          = nullptr;
    mGroupLayoutModel  = nullptr;
    mGroupLayoutPer    = nullptr;
    mPipelineLayout    = nullptr;
    mBindGroupModel    = nullptr;
    mFishVertexBuffer  = nullptr;
    mLightFactorBuffer = nullptr;
}
