//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelDawn.cpp: Implements fish model of Dawn.

#include "BufferDawn.h"
#include "FishModelInstancedDrawDawn.h"

FishModelInstancedDrawDawn::FishModelInstancedDrawDawn(const Context *context,
                                                       Aquarium *aquarium,
                                                       MODELGROUP type,
                                                       MODELNAME name,
                                                       bool blend)
    : FishModel(type, name, blend), instance(0)
{
    mContextDawn = static_cast<const ContextDawn *>(context);

    mLightFactorUniforms.shininess      = 5.0f;
    mLightFactorUniforms.specularFactor = 0.3f;

    const Fish &fishInfo              = fishTable[name - MODELNAME::MODELSMALLFISHAINSTANCEDDRAWS];
    mFishVertexUniforms.fishLength     = fishInfo.fishLength;
    mFishVertexUniforms.fishBendAmount = fishInfo.fishBendAmount;
    mFishVertexUniforms.fishWaveLength = fishInfo.fishWaveLength;

    instance = aquarium->fishCount[fishInfo.modelName - MODELNAME::MODELSMALLFISHA];
    mFishPers = new FishPer[instance];
}

void FishModelInstancedDrawDawn::init()
{
    if (instance == 0)
        return;

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

    mFishPersBuffer = mContextDawn->createBuffer(
        sizeof(FishPer) * instance, dawn::BufferUsage::Vertex | dawn::BufferUsage::CopyDst);

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
    mVertexInputDescriptor.cAttributes[4].offset         = offsetof(FishPer, worldPosition);
    mVertexInputDescriptor.cBuffers[4].attributes        = &mVertexInputDescriptor.cAttributes[4];
    mVertexInputDescriptor.cBuffers[5].attributeCount    = 4;
    mVertexInputDescriptor.cBuffers[5].stride            = sizeof(FishPer);
    mVertexInputDescriptor.cAttributes[5].format         = dawn::VertexFormat::Float3;
    mVertexInputDescriptor.cAttributes[5].shaderLocation = 5;
    mVertexInputDescriptor.cAttributes[5].offset         = 0;
    mVertexInputDescriptor.cAttributes[6].format         = dawn::VertexFormat::Float;
    mVertexInputDescriptor.cAttributes[6].shaderLocation = 6;
    mVertexInputDescriptor.cAttributes[6].offset         = offsetof(FishPer, scale);
    mVertexInputDescriptor.cAttributes[7].format         = dawn::VertexFormat::Float3;
    mVertexInputDescriptor.cAttributes[7].shaderLocation = 7;
    mVertexInputDescriptor.cAttributes[7].offset         = offsetof(FishPer, nextPosition);
    mVertexInputDescriptor.cAttributes[8].format         = dawn::VertexFormat::Float;
    mVertexInputDescriptor.cAttributes[8].shaderLocation = 8;
    mVertexInputDescriptor.cAttributes[9].offset         = offsetof(FishPer, time);
    mVertexInputDescriptor.cBuffers[5].attributes        = &mVertexInputDescriptor.cAttributes[5];
    mVertexInputDescriptor.cBuffers[5].stepMode          = dawn::InputStepMode::Instance;
    mVertexInputDescriptor.bufferCount                   = 6;
    mVertexInputDescriptor.indexFormat                   = dawn::IndexFormat::Uint16;

    if (mSkyboxTexture && mReflectionTexture)
    {
        mGroupLayoutModel = mContextDawn->MakeBindGroupLayout({
            {0, dawn::ShaderStage::Vertex, dawn::BindingType::UniformBuffer},
            {1, dawn::ShaderStage::Fragment, dawn::BindingType::UniformBuffer},
            {2, dawn::ShaderStage::Fragment, dawn::BindingType::Sampler},
            {3, dawn::ShaderStage::Fragment, dawn::BindingType::Sampler},
            {4, dawn::ShaderStage::Fragment, dawn::BindingType::SampledTexture, false, false,
             dawn::TextureViewDimension::e2D, dawn::TextureComponentType::Float},
            {5, dawn::ShaderStage::Fragment, dawn::BindingType::SampledTexture, false, false,
             dawn::TextureViewDimension::e2D, dawn::TextureComponentType::Float},
            {6, dawn::ShaderStage::Fragment, dawn::BindingType::SampledTexture, false, false,
             dawn::TextureViewDimension::e2D, dawn::TextureComponentType::Float},
            {7, dawn::ShaderStage::Fragment, dawn::BindingType::SampledTexture, false, false,
             dawn::TextureViewDimension::Cube, dawn::TextureComponentType::Float},
        });
    }
    else
    {
        mGroupLayoutModel = mContextDawn->MakeBindGroupLayout({
            {0, dawn::ShaderStage::Vertex, dawn::BindingType::UniformBuffer},
            {1, dawn::ShaderStage::Fragment, dawn::BindingType::UniformBuffer},
            {2, dawn::ShaderStage::Fragment, dawn::BindingType::Sampler},
            {3, dawn::ShaderStage::Fragment, dawn::BindingType::SampledTexture, false, false,
             dawn::TextureViewDimension::e2D, dawn::TextureComponentType::Float},
            {4, dawn::ShaderStage::Fragment, dawn::BindingType::SampledTexture, false, false,
             dawn::TextureViewDimension::e2D, dawn::TextureComponentType::Float},
        });
    }

    mGroupLayoutPer = mContextDawn->MakeBindGroupLayout({
        {0, dawn::ShaderStage::Vertex, dawn::BindingType::UniformBuffer},
    });

    mPipelineLayout = mContextDawn->MakeBasicPipelineLayout({
        mContextDawn->groupLayoutGeneral,
        mContextDawn->groupLayoutWorld,
        mGroupLayoutModel,
        mGroupLayoutPer,
    });

    mPipeline = mContextDawn->createRenderPipeline(mPipelineLayout, mProgramDawn,
                                                   mVertexInputDescriptor, mBlend);

    mFishVertexBuffer =
        mContextDawn->createBufferFromData(&mFishVertexUniforms, sizeof(FishVertexUniforms),
                                           dawn::BufferUsage::CopyDst | dawn::BufferUsage::Uniform);
    mLightFactorBuffer =
        mContextDawn->createBufferFromData(&mLightFactorUniforms, sizeof(LightFactorUniforms),
                                           dawn::BufferUsage::CopyDst | dawn::BufferUsage::Uniform);

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

void FishModelInstancedDrawDawn::prepareForDraw() {}

void FishModelInstancedDrawDawn::draw()
{
    if (instance == 0)
        return;

    mContextDawn->setBufferData(mFishPersBuffer, 0, sizeof(FishPer) * instance, mFishPers);

    dawn::RenderPassEncoder pass = mContextDawn->getRenderPass();
    pass.SetPipeline(mPipeline);
    pass.SetBindGroup(0, mContextDawn->bindGroupGeneral, 0, nullptr);
    pass.SetBindGroup(1, mContextDawn->bindGroupWorld, 0, nullptr);
    pass.SetBindGroup(2, mBindGroupModel, 0, nullptr);
    pass.SetVertexBuffer(0, mPositionBuffer->getBuffer());
    pass.SetVertexBuffer(1, mNormalBuffer->getBuffer());
    pass.SetVertexBuffer(2, mTexCoordBuffer->getBuffer());
    pass.SetVertexBuffer(3, mTangentBuffer->getBuffer());
    pass.SetVertexBuffer(4, mBiNormalBuffer->getBuffer());
    pass.SetVertexBuffer(5, mFishPersBuffer);
    pass.SetIndexBuffer(mIndicesBuffer->getBuffer(), 0);
    pass.DrawIndexed(mIndicesBuffer->getTotalComponents(), instance, 0, 0, 0);
}

void FishModelInstancedDrawDawn::updatePerInstanceUniforms(const WorldUniforms &worldUniforms) {}

void FishModelInstancedDrawDawn::updateFishPerUniforms(float x,
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

FishModelInstancedDrawDawn::~FishModelInstancedDrawDawn()
{
    mPipeline          = nullptr;
    mGroupLayoutModel  = nullptr;
    mGroupLayoutPer    = nullptr;
    mPipelineLayout    = nullptr;
    mBindGroupModel    = nullptr;
    mBindGroupPer      = nullptr;
    mFishVertexBuffer  = nullptr;
    mLightFactorBuffer = nullptr;
    mFishPersBuffer    = nullptr;
    delete mFishPers;
}
