//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// SeaweenModelDawn: Implements seaweed model of Dawn.

#include "SeaweedModelDawn.h"

SeaweedModelDawn::SeaweedModelDawn(const Context* context, Aquarium* aquarium, MODELGROUP type, MODELNAME name, bool blend)
    : SeaweedModel(type, name, blend), instance(0)
{
    mContextDawn = static_cast<const ContextDawn *>(context);
    mAquarium   = aquarium;

    mLightFactorUniforms.shininess      = 50.0f;
    mLightFactorUniforms.specularFactor = 1.0f;
}

SeaweedModelDawn::~SeaweedModelDawn()
{
    mPipeline          = nullptr;
    mGroupLayoutModel  = nullptr;
    mGroupLayoutPer    = nullptr;
    mPipelineLayout    = nullptr;
    mBindGroupModel    = nullptr;
    mBindGroupPer      = nullptr;
    mLightFactorBuffer = nullptr;
    mViewBuffer        = nullptr;
    mTimeBuffer        = nullptr;
}

void SeaweedModelDawn::init()
{
    mProgramDawn = static_cast<ProgramDawn *>(mProgram);

    mDiffuseTexture    = static_cast<TextureDawn *>(textureMap["diffuse"]);
    mNormalTexture     = static_cast<TextureDawn *>(textureMap["normalMap"]);
    mReflectionTexture = static_cast<TextureDawn *>(textureMap["reflectionMap"]);
    mSkyboxTexture     = static_cast<TextureDawn *>(textureMap["skybox"]);

    mPositionBuffer = static_cast<BufferDawn *>(bufferMap["position"]);
    mNormalBuffer   = static_cast<BufferDawn *>(bufferMap["normal"]);
    mTexCoordBuffer = static_cast<BufferDawn *>(bufferMap["texCoord"]);
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
    mVertexInputDescriptor.bufferCount                   = 3;
    mVertexInputDescriptor.indexFormat                   = dawn::IndexFormat::Uint16;

    mGroupLayoutModel = mContextDawn->MakeBindGroupLayout({
        {0, dawn::ShaderStage::Fragment, dawn::BindingType::UniformBuffer},
        {1, dawn::ShaderStage::Fragment, dawn::BindingType::Sampler},
        {2, dawn::ShaderStage::Fragment, dawn::BindingType::SampledTexture, false, false,
         dawn::TextureViewDimension::e2D, dawn::TextureComponentType::Float},
    });

    mGroupLayoutPer = mContextDawn->MakeBindGroupLayout({
        {0, dawn::ShaderStage::Vertex, dawn::BindingType::UniformBuffer},
        {1, dawn::ShaderStage::Vertex, dawn::BindingType::UniformBuffer},
    });

    mPipelineLayout = mContextDawn->MakeBasicPipelineLayout({
        mContextDawn->groupLayoutGeneral,
        mContextDawn->groupLayoutWorld,
        mGroupLayoutModel,
        mGroupLayoutPer,
    });

    mPipeline = mContextDawn->createRenderPipeline(mPipelineLayout, mProgramDawn,
                                                   mVertexInputDescriptor, mBlend);

    mLightFactorBuffer =
        mContextDawn->createBufferFromData(&mLightFactorUniforms, sizeof(mLightFactorUniforms),
                                           dawn::BufferUsage::CopyDst | dawn::BufferUsage::Uniform);
    mTimeBuffer =
        mContextDawn->createBufferFromData(&mSeaweedPer, sizeof(mSeaweedPer) * 4,
                                           dawn::BufferUsage::CopyDst | dawn::BufferUsage::Uniform);
    mViewBuffer =
        mContextDawn->createBufferFromData(&mWorldUniformPer, sizeof(WorldUniformPer),
                                           dawn::BufferUsage::CopyDst | dawn::BufferUsage::Uniform);

    mBindGroupModel = mContextDawn->makeBindGroup(
        mGroupLayoutModel, {
                               {0, mLightFactorBuffer, 0, sizeof(LightFactorUniforms)},
                               {1, mDiffuseTexture->getSampler()},
                               {2, mDiffuseTexture->getTextureView()},
                           });

    mBindGroupPer = mContextDawn->makeBindGroup(mGroupLayoutPer,
                                                {
                                                    {0, mViewBuffer, 0, sizeof(WorldUniformPer)},
                                                    {1, mTimeBuffer, 0, sizeof(SeaweedPer)},
                                                });

    mContextDawn->setBufferData(mLightFactorBuffer, 0, sizeof(mLightFactorUniforms),
                                &mLightFactorUniforms);
}

void SeaweedModelDawn::prepareForDraw()
{
    mContextDawn->setBufferData(mViewBuffer, 0, sizeof(WorldUniformPer), &mWorldUniformPer);
    mContextDawn->setBufferData(mTimeBuffer, 0, sizeof(SeaweedPer), &mSeaweedPer);
}

void SeaweedModelDawn::draw()
{
    dawn::RenderPassEncoder pass = mContextDawn->getRenderPass();
    pass.SetPipeline(mPipeline);
    pass.SetBindGroup(0, mContextDawn->bindGroupGeneral, 0, nullptr);
    pass.SetBindGroup(1, mContextDawn->bindGroupWorld, 0, nullptr);
    pass.SetBindGroup(2, mBindGroupModel, 0, nullptr);
    pass.SetBindGroup(3, mBindGroupPer, 0, nullptr);
    pass.SetVertexBuffer(0, mPositionBuffer->getBuffer());
    pass.SetVertexBuffer(1, mNormalBuffer->getBuffer());
    pass.SetVertexBuffer(2, mTexCoordBuffer->getBuffer());
    pass.SetIndexBuffer(mIndicesBuffer->getBuffer(), 0);
    pass.DrawIndexed(mIndicesBuffer->getTotalComponents(), instance, 0, 0, 0);
    instance = 0;
}

void SeaweedModelDawn::updatePerInstanceUniforms(const WorldUniforms &worldUniforms)
{
    mWorldUniformPer.worldUniforms[instance] = worldUniforms;
    mSeaweedPer.time[instance]               = mAquarium->g.mclock + instance;

    instance++;
}

void SeaweedModelDawn::updateSeaweedModelTime(float time)
{
}

