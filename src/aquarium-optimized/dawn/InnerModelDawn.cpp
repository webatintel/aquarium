//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// InnerModelDawn.cpp: Implements inner model of Dawn.

#include "InnerModelDawn.h"

InnerModelDawn::InnerModelDawn(const Context *context,
                               Aquarium *aquarium,
                               MODELGROUP type,
                               MODELNAME name,
                               bool blend)
    : Model(type, name, blend)
{
    mContextDawn = static_cast<const ContextDawn *>(context);

    mInnerUniforms.eta             = 1.0f;
    mInnerUniforms.tankColorFudge  = 0.796f;
    mInnerUniforms.refractionFudge = 3.0f;
}

InnerModelDawn::~InnerModelDawn()
{
    mPipeline         = nullptr;
    mGroupLayoutModel = nullptr;
    mGroupLayoutPer   = nullptr;
    mPipelineLayout   = nullptr;
    mBindGroupModel   = nullptr;
    mBindGroupPer     = nullptr;
    mInnerBuffer      = nullptr;
    mViewBuffer       = nullptr;
}

void InnerModelDawn::init()
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

    mGroupLayoutModel = mContextDawn->MakeBindGroupLayout({
        {0, wgpu::ShaderStage::Fragment, wgpu::BindingType::UniformBuffer},
        {1, wgpu::ShaderStage::Fragment, wgpu::BindingType::Sampler},
        {2, wgpu::ShaderStage::Fragment, wgpu::BindingType::Sampler},
        {3, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture, false, false,
         wgpu::TextureViewDimension::e2D, wgpu::TextureComponentType::Float},
        {4, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture, false, false,
         wgpu::TextureViewDimension::e2D, wgpu::TextureComponentType::Float},
        {5, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture, false, false,
         wgpu::TextureViewDimension::e2D, wgpu::TextureComponentType::Float},
        {6, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture, false, false,
         wgpu::TextureViewDimension::Cube, wgpu::TextureComponentType::Float},
    });

    mGroupLayoutPer = mContextDawn->MakeBindGroupLayout({
        {0, wgpu::ShaderStage::Vertex, wgpu::BindingType::UniformBuffer},
    });

    mPipelineLayout = mContextDawn->MakeBasicPipelineLayout({
        mContextDawn->groupLayoutGeneral,
        mContextDawn->groupLayoutWorld,
        mGroupLayoutModel,
        mGroupLayoutPer,
    });

    mPipeline = mContextDawn->createRenderPipeline(mPipelineLayout, mProgramDawn,
                                                   mVertexStateDescriptor, mBlend);

    mInnerBuffer =
        mContextDawn->createBufferFromData(&mInnerUniforms, sizeof(mInnerUniforms),
                                           wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);
    mViewBuffer =
        mContextDawn->createBufferFromData(&mWorldUniformPer, sizeof(WorldUniforms),
                                           wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);

    mBindGroupModel =
        mContextDawn->makeBindGroup(mGroupLayoutModel, {{0, mInnerBuffer, 0, sizeof(InnerUniforms)},
                                                        {1, mReflectionTexture->getSampler()},
                                                        {2, mSkyboxTexture->getSampler()},
                                                        {3, mDiffuseTexture->getTextureView()},
                                                        {4, mNormalTexture->getTextureView()},
                                                        {5, mReflectionTexture->getTextureView()},
                                                        {6, mSkyboxTexture->getTextureView()}});

    mBindGroupPer =
        mContextDawn->makeBindGroup(mGroupLayoutPer, {
                                                         {0, mViewBuffer, 0, sizeof(WorldUniforms)},
                                                     });

    mContextDawn->setBufferData(mInnerBuffer, 0, sizeof(InnerUniforms), &mInnerUniforms);
}

void InnerModelDawn::prepareForDraw() {}

void InnerModelDawn::draw()
{
    wgpu::RenderPassEncoder pass = mContextDawn->getRenderPass();
    pass.SetPipeline(mPipeline);
    pass.SetBindGroup(0, mContextDawn->bindGroupGeneral, 0, nullptr);
    pass.SetBindGroup(1, mContextDawn->bindGroupWorld, 0, nullptr);
    pass.SetBindGroup(2, mBindGroupModel, 0, nullptr);
    pass.SetBindGroup(3, mBindGroupPer, 0, nullptr);
    pass.SetVertexBuffer(0, mPositionBuffer->getBuffer());
    pass.SetVertexBuffer(1, mNormalBuffer->getBuffer());
    pass.SetVertexBuffer(2, mTexCoordBuffer->getBuffer());
    pass.SetVertexBuffer(3, mTangentBuffer->getBuffer());
    pass.SetVertexBuffer(4, mBiNormalBuffer->getBuffer());
    pass.SetIndexBuffer(mIndicesBuffer->getBuffer(), 0);
    pass.DrawIndexed(mIndicesBuffer->getTotalComponents(), 1, 0, 0, 0);
}

void InnerModelDawn::updatePerInstanceUniforms(const WorldUniforms &worldUniforms)
{
    std::memcpy(&mWorldUniformPer, &worldUniforms, sizeof(WorldUniforms));

    mContextDawn->setBufferData(mViewBuffer, 0, sizeof(WorldUniforms), &mWorldUniformPer);
}
