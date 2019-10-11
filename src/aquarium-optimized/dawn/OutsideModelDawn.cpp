//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#include "OutsideModelDawn.h"

OutsideModelDawn::OutsideModelDawn(const Context *context,
                                   Aquarium *aquarium,
                                   MODELGROUP type,
                                   MODELNAME name,
                                   bool blend)
    : Model(type, name, blend)
{
    mContextDawn = static_cast<const ContextDawn *>(context);

    mLightFactorUniforms.shininess      = 50.0f;
    mLightFactorUniforms.specularFactor = 0.0f;
}

OutsideModelDawn::~OutsideModelDawn()
{
    mPipeline          = nullptr;
    mGroupLayoutModel  = nullptr;
    mGroupLayoutPer    = nullptr;
    mPipelineLayout    = nullptr;
    mBindGroupModel    = nullptr;
    mBindGroupPer      = nullptr;
    mLightFactorBuffer = nullptr;
    mViewBuffer        = nullptr;
}

void OutsideModelDawn::init()
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

    mGroupLayoutPer = mContextDawn->MakeBindGroupLayout({
        {0, dawn::ShaderStage::Vertex, dawn::BindingType::UniformBuffer},
    });

    // Outside models use diffuse shaders.
    mGroupLayoutModel = mContextDawn->MakeBindGroupLayout({
        {0, dawn::ShaderStage::Fragment, dawn::BindingType::UniformBuffer},
        {1, dawn::ShaderStage::Fragment, dawn::BindingType::Sampler},
        {2, dawn::ShaderStage::Fragment, dawn::BindingType::SampledTexture},
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
    mViewBuffer =
        mContextDawn->createBufferFromData(&mWorldUniformPer, sizeof(WorldUniforms) * 20,
                                           dawn::BufferUsage::CopyDst | dawn::BufferUsage::Uniform);

    mBindGroupModel = mContextDawn->makeBindGroup(
        mGroupLayoutModel, {
                               {0, mLightFactorBuffer, 0, sizeof(LightFactorUniforms)},
                               {1, mDiffuseTexture->getSampler()},
                               {2, mDiffuseTexture->getTextureView()},
                           });

    mBindGroupPer =
        mContextDawn->makeBindGroup(mGroupLayoutPer, {
                                                         {0, mViewBuffer, 0, sizeof(WorldUniforms)},
                                                     });

    mContextDawn->setBufferData(mLightFactorBuffer, 0, sizeof(LightFactorUniforms),
                                &mLightFactorUniforms);
}

void OutsideModelDawn::prepareForDraw() {}

void OutsideModelDawn::draw()
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
    // diffuseShader doesn't have to input tangent buffer or binormal buffer.
    if (mTangentBuffer && mBiNormalBuffer)
    {
        pass.SetVertexBuffer(3, mTangentBuffer->getBuffer());
        pass.SetVertexBuffer(4, mBiNormalBuffer->getBuffer());
    }
    pass.SetIndexBuffer(mIndicesBuffer->getBuffer(), 0);
    pass.DrawIndexed(mIndicesBuffer->getTotalComponents(), 1, 0, 0, 0);
}

void OutsideModelDawn::updatePerInstanceUniforms(const WorldUniforms &worldUniforms)
{
    memcpy(&mWorldUniformPer, &worldUniforms, sizeof(WorldUniforms));

    mContextDawn->setBufferData(mViewBuffer, 0, sizeof(WorldUniforms), &mWorldUniformPer);
}
