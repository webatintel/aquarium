//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "GenericModelDawn.h"

#include "../Aquarium.h"

GenericModelDawn::GenericModelDawn(const Context *context,
                                   Aquarium *aquarium,
                                   MODELGROUP type,
                                   MODELNAME name,
                                   bool blend)
    : Model(type, name, blend), instance(0)
{
    mContextDawn = static_cast<const ContextDawn *>(context);

    mLightFactorUniforms.shininess      = 50.0f;
    mLightFactorUniforms.specularFactor = 1.0f;
}

GenericModelDawn::~GenericModelDawn()
{
    mPipeline          = nullptr;
    mGroupLayoutModel  = nullptr;
    mGroupLayoutPer    = nullptr;
    mPipelineLayout    = nullptr;
    mBindGroupModel    = nullptr;
    mBindGroupPer      = nullptr;
    mLightFactorBuffer = nullptr;
    mWorldBuffer       = nullptr;
}

void GenericModelDawn::init()
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

    // Generic models use reflection, normal or diffuse shaders, of which groupLayouts are
    // diiferent in texture binding.  MODELGLOBEBASE use diffuse shader though it contains
    // normal and reflection textures.
    if (mNormalTexture && mName != MODELNAME::MODELGLOBEBASE)
    {
        mVertexStateDescriptor.cVertexBuffers[0].attributeCount = 1;
        mVertexStateDescriptor.cVertexBuffers[0].arrayStride    = mPositionBuffer->getDataSize();
        mVertexStateDescriptor.cAttributes[0].format            = wgpu::VertexFormat::Float3;
        mVertexStateDescriptor.cAttributes[0].shaderLocation    = 0;
        mVertexStateDescriptor.cAttributes[0].offset            = 0;
        mVertexStateDescriptor.cVertexBuffers[0].attributes =
            &mVertexStateDescriptor.cAttributes[0];
        mVertexStateDescriptor.cVertexBuffers[1].attributeCount = 1;
        mVertexStateDescriptor.cVertexBuffers[1].arrayStride    = mNormalBuffer->getDataSize();
        mVertexStateDescriptor.cAttributes[1].format            = wgpu::VertexFormat::Float3;
        mVertexStateDescriptor.cAttributes[1].shaderLocation    = 1;
        mVertexStateDescriptor.cAttributes[1].offset            = 0;
        mVertexStateDescriptor.cVertexBuffers[1].attributes =
            &mVertexStateDescriptor.cAttributes[1];
        mVertexStateDescriptor.cVertexBuffers[2].attributeCount = 1;
        mVertexStateDescriptor.cVertexBuffers[2].arrayStride    = mTexCoordBuffer->getDataSize();
        mVertexStateDescriptor.cAttributes[2].format            = wgpu::VertexFormat::Float2;
        mVertexStateDescriptor.cAttributes[2].shaderLocation    = 2;
        mVertexStateDescriptor.cAttributes[2].offset            = 0;
        mVertexStateDescriptor.cVertexBuffers[2].attributes =
            &mVertexStateDescriptor.cAttributes[2];
        mVertexStateDescriptor.cVertexBuffers[3].attributeCount = 1;
        mVertexStateDescriptor.cVertexBuffers[3].arrayStride    = mTangentBuffer->getDataSize();
        mVertexStateDescriptor.cAttributes[3].format            = wgpu::VertexFormat::Float3;
        mVertexStateDescriptor.cAttributes[3].shaderLocation    = 3;
        mVertexStateDescriptor.cAttributes[3].offset            = 0;
        mVertexStateDescriptor.cVertexBuffers[3].attributes =
            &mVertexStateDescriptor.cAttributes[3];
        mVertexStateDescriptor.cVertexBuffers[4].attributeCount = 1;
        mVertexStateDescriptor.cVertexBuffers[4].arrayStride    = mBiNormalBuffer->getDataSize();
        mVertexStateDescriptor.cAttributes[4].format            = wgpu::VertexFormat::Float3;
        mVertexStateDescriptor.cAttributes[4].shaderLocation    = 4;
        mVertexStateDescriptor.cAttributes[4].offset            = 0;
        mVertexStateDescriptor.cVertexBuffers[4].attributes =
            &mVertexStateDescriptor.cAttributes[4];
        mVertexStateDescriptor.vertexBufferCount = 5;
        mVertexStateDescriptor.indexFormat       = wgpu::IndexFormat::Uint16;
    }
    else
    {
        mVertexStateDescriptor.cVertexBuffers[0].attributeCount = 1;
        mVertexStateDescriptor.cVertexBuffers[0].arrayStride    = mPositionBuffer->getDataSize();
        mVertexStateDescriptor.cAttributes[0].format            = wgpu::VertexFormat::Float3;
        mVertexStateDescriptor.cAttributes[0].shaderLocation    = 0;
        mVertexStateDescriptor.cAttributes[0].offset            = 0;
        mVertexStateDescriptor.cVertexBuffers[0].attributes =
            &mVertexStateDescriptor.cAttributes[0];
        mVertexStateDescriptor.cVertexBuffers[1].attributeCount = 1;
        mVertexStateDescriptor.cVertexBuffers[1].arrayStride    = mNormalBuffer->getDataSize();
        mVertexStateDescriptor.cAttributes[1].format            = wgpu::VertexFormat::Float3;
        mVertexStateDescriptor.cAttributes[1].shaderLocation    = 1;
        mVertexStateDescriptor.cAttributes[1].offset            = 0;
        mVertexStateDescriptor.cVertexBuffers[1].attributes =
            &mVertexStateDescriptor.cAttributes[1];
        mVertexStateDescriptor.cVertexBuffers[2].attributeCount = 1;
        mVertexStateDescriptor.cVertexBuffers[2].arrayStride    = mTexCoordBuffer->getDataSize();
        mVertexStateDescriptor.cAttributes[2].format            = wgpu::VertexFormat::Float2;
        mVertexStateDescriptor.cAttributes[2].shaderLocation    = 2;
        mVertexStateDescriptor.cAttributes[2].offset            = 0;
        mVertexStateDescriptor.cVertexBuffers[2].attributes =
            &mVertexStateDescriptor.cAttributes[2];
        mVertexStateDescriptor.vertexBufferCount = 3;
        mVertexStateDescriptor.indexFormat       = wgpu::IndexFormat::Uint16;
    }

    if (mSkyboxTexture && mReflectionTexture && mName != MODELNAME::MODELGLOBEBASE)
    {
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
    }
    else if (mNormalTexture && mName != MODELNAME::MODELGLOBEBASE)
    {
        mGroupLayoutModel = mContextDawn->MakeBindGroupLayout({
            {0, wgpu::ShaderStage::Fragment, wgpu::BindingType::UniformBuffer},
            {1, wgpu::ShaderStage::Fragment, wgpu::BindingType::Sampler},
            {2, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture, false, false,
             wgpu::TextureViewDimension::e2D, wgpu::TextureComponentType::Float},
            {3, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture, false, false,
             wgpu::TextureViewDimension::e2D, wgpu::TextureComponentType::Float},
        });
    }
    else
    {
        mGroupLayoutModel = mContextDawn->MakeBindGroupLayout({
            {0, wgpu::ShaderStage::Fragment, wgpu::BindingType::UniformBuffer},
            {1, wgpu::ShaderStage::Fragment, wgpu::BindingType::Sampler},
            {2, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture, false, false,
             wgpu::TextureViewDimension::e2D, wgpu::TextureComponentType::Float},
        });
    }

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

    mLightFactorBuffer =
        mContextDawn->createBufferFromData(&mLightFactorUniforms, sizeof(mLightFactorUniforms),
                                           wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);
    mWorldBuffer =
        mContextDawn->createBufferFromData(&mWorldUniformPer, sizeof(mWorldUniformPer),
                                           wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);

    // Generic models use reflection, normal or diffuse shaders, of which grouplayouts are
    // diiferent in texture binding. MODELGLOBEBASE use diffuse shader though it contains
    // normal and reflection textures.
    if (mSkyboxTexture && mReflectionTexture && mName != MODELNAME::MODELGLOBEBASE)
    {
        mBindGroupModel = mContextDawn->makeBindGroup(
            mGroupLayoutModel, {{0, mLightFactorBuffer, 0, sizeof(LightFactorUniforms)},
                                {1, mReflectionTexture->getSampler()},
                                {2, mSkyboxTexture->getSampler()},
                                {3, mDiffuseTexture->getTextureView()},
                                {4, mNormalTexture->getTextureView()},
                                {5, mReflectionTexture->getTextureView()},
                                {6, mSkyboxTexture->getTextureView()}});
    }
    else if (mNormalTexture && mName != MODELNAME::MODELGLOBEBASE)
    {
        mBindGroupModel = mContextDawn->makeBindGroup(
            mGroupLayoutModel, {
                                   {0, mLightFactorBuffer, 0, sizeof(LightFactorUniforms)},
                                   {1, mDiffuseTexture->getSampler()},
                                   {2, mDiffuseTexture->getTextureView()},
                                   {3, mNormalTexture->getTextureView()},
                               });
    }
    else
    {
        mBindGroupModel = mContextDawn->makeBindGroup(
            mGroupLayoutModel, {
                                   {0, mLightFactorBuffer, 0, sizeof(LightFactorUniforms)},
                                   {1, mDiffuseTexture->getSampler()},
                                   {2, mDiffuseTexture->getTextureView()},
                               });
    }

    mBindGroupPer = mContextDawn->makeBindGroup(mGroupLayoutPer,
                                                {
                                                    {0, mWorldBuffer, 0, sizeof(WorldUniformPer)},
                                                });

    mContextDawn->setBufferData(mLightFactorBuffer, 0, sizeof(LightFactorUniforms),
                                &mLightFactorUniforms);
}

void GenericModelDawn::prepareForDraw()
{
    mContextDawn->setBufferData(mWorldBuffer, 0, sizeof(WorldUniformPer), &mWorldUniformPer);
}

void GenericModelDawn::draw()
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
    // diffuseShader doesn't have to input tangent buffer or binormal buffer.
    if (mTangentBuffer && mBiNormalBuffer && mName != MODELNAME::MODELGLOBEBASE)
    {
        pass.SetVertexBuffer(3, mTangentBuffer->getBuffer());
        pass.SetVertexBuffer(4, mBiNormalBuffer->getBuffer());
    }
    pass.SetIndexBuffer(mIndicesBuffer->getBuffer(), 0);
    pass.DrawIndexed(mIndicesBuffer->getTotalComponents(), instance, 0, 0, 0);
    instance = 0;
}

void GenericModelDawn::updatePerInstanceUniforms(const WorldUniforms &worldUniforms)
{
    mWorldUniformPer.WorldUniforms[instance] = worldUniforms;

    instance++;
}
