//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelDawn.cpp: Implements fish model of Dawn.

#include "FishModelDawn.h"

#include <iostream>
#include <vector>

#include "BufferDawn.h"

FishModelDawn::FishModelDawn(Context *context,
                             Aquarium *aquarium,
                             MODELGROUP type,
                             MODELNAME name,
                             bool blend)
    : FishModel(type, name, blend, aquarium) {
  mContextDawn = static_cast<ContextDawn *>(context);

  mEnableDynamicBufferOffset = aquarium->toggleBitset.test(
      static_cast<size_t>(TOGGLE::ENABLEDYNAMICBUFFEROFFSET));

  mLightFactorUniforms.shininess = 5.0f;
  mLightFactorUniforms.specularFactor = 0.3f;

  const Fish &fishInfo = fishTable[name - MODELNAME::MODELSMALLFISHA];
  mFishVertexUniforms.fishLength = fishInfo.fishLength;
  mFishVertexUniforms.fishBendAmount = fishInfo.fishBendAmount;
  mFishVertexUniforms.fishWaveLength = fishInfo.fishWaveLength;

  mCurInstance =
      mAquarium->fishCount[fishInfo.modelName - MODELNAME::MODELSMALLFISHA];
  mPreInstance = mCurInstance;
}

void FishModelDawn::init() {
  mProgramDawn = static_cast<ProgramDawn *>(mProgram);

  mDiffuseTexture = static_cast<TextureDawn *>(textureMap["diffuse"]);
  mNormalTexture = static_cast<TextureDawn *>(textureMap["normalMap"]);
  mReflectionTexture = static_cast<TextureDawn *>(textureMap["reflectionMap"]);
  mSkyboxTexture = static_cast<TextureDawn *>(textureMap["skybox"]);

  mPositionBuffer = static_cast<BufferDawn *>(bufferMap["position"]);
  mNormalBuffer = static_cast<BufferDawn *>(bufferMap["normal"]);
  mTexCoordBuffer = static_cast<BufferDawn *>(bufferMap["texCoord"]);
  mTangentBuffer = static_cast<BufferDawn *>(bufferMap["tangent"]);
  mBiNormalBuffer = static_cast<BufferDawn *>(bufferMap["binormal"]);
  mIndicesBuffer = static_cast<BufferDawn *>(bufferMap["indices"]);

  std::vector<wgpu::VertexAttributeDescriptor> vertexAttributeDescriptor;
  vertexAttributeDescriptor.resize(5);
  vertexAttributeDescriptor[0].format = wgpu::VertexFormat::Float3;
  vertexAttributeDescriptor[0].offset = 0;
  vertexAttributeDescriptor[0].shaderLocation = 0;
  vertexAttributeDescriptor[1].format = wgpu::VertexFormat::Float3;
  vertexAttributeDescriptor[1].offset = 0;
  vertexAttributeDescriptor[1].shaderLocation = 1;
  vertexAttributeDescriptor[2].format = wgpu::VertexFormat::Float2;
  vertexAttributeDescriptor[2].offset = 0;
  vertexAttributeDescriptor[2].shaderLocation = 2;
  vertexAttributeDescriptor[3].format = wgpu::VertexFormat::Float3;
  vertexAttributeDescriptor[3].offset = 0;
  vertexAttributeDescriptor[3].shaderLocation = 3;
  vertexAttributeDescriptor[4].format = wgpu::VertexFormat::Float3;
  vertexAttributeDescriptor[4].offset = 0;
  vertexAttributeDescriptor[4].shaderLocation = 4;

  std::vector<wgpu::VertexBufferLayoutDescriptor> vertexBufferLayoutDescriptor;
  vertexBufferLayoutDescriptor.resize(5);
  vertexBufferLayoutDescriptor[0].arrayStride = mPositionBuffer->getDataSize();
  vertexBufferLayoutDescriptor[0].stepMode = wgpu::InputStepMode::Vertex;
  vertexBufferLayoutDescriptor[0].attributeCount = 1;
  vertexBufferLayoutDescriptor[0].attributes = &vertexAttributeDescriptor[0];
  vertexBufferLayoutDescriptor[1].arrayStride = mNormalBuffer->getDataSize();
  vertexBufferLayoutDescriptor[1].stepMode = wgpu::InputStepMode::Vertex;
  vertexBufferLayoutDescriptor[1].attributeCount = 1;
  vertexBufferLayoutDescriptor[1].attributes = &vertexAttributeDescriptor[1];
  vertexBufferLayoutDescriptor[2].arrayStride = mTexCoordBuffer->getDataSize();
  vertexBufferLayoutDescriptor[2].stepMode = wgpu::InputStepMode::Vertex;
  vertexBufferLayoutDescriptor[2].attributeCount = 1;
  vertexBufferLayoutDescriptor[2].attributes = &vertexAttributeDescriptor[2];
  vertexBufferLayoutDescriptor[3].arrayStride = mTangentBuffer->getDataSize();
  vertexBufferLayoutDescriptor[3].stepMode = wgpu::InputStepMode::Vertex;
  vertexBufferLayoutDescriptor[3].attributeCount = 1;
  vertexBufferLayoutDescriptor[3].attributes = &vertexAttributeDescriptor[3];
  vertexBufferLayoutDescriptor[4].arrayStride = mBiNormalBuffer->getDataSize();
  vertexBufferLayoutDescriptor[4].stepMode = wgpu::InputStepMode::Vertex;
  vertexBufferLayoutDescriptor[4].attributeCount = 1;
  vertexBufferLayoutDescriptor[4].attributes = &vertexAttributeDescriptor[4];

  mVertexStateDescriptor.vertexBufferCount =
      static_cast<uint32_t>(vertexBufferLayoutDescriptor.size());
  mVertexStateDescriptor.vertexBuffers = vertexBufferLayoutDescriptor.data();

  if (mSkyboxTexture && mReflectionTexture) {
    mGroupLayoutModel = mContextDawn->MakeBindGroupLayout({
        {0, wgpu::ShaderStage::Vertex, wgpu::BindingType::UniformBuffer},
        {1, wgpu::ShaderStage::Fragment, wgpu::BindingType::UniformBuffer},
        {2, wgpu::ShaderStage::Fragment, wgpu::BindingType::Sampler},
        {3, wgpu::ShaderStage::Fragment, wgpu::BindingType::Sampler},
        {4, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture,
         false, 0, false, wgpu::TextureViewDimension::e2D,
         wgpu::TextureComponentType::Float},
        {5, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture,
         false, 0, false, wgpu::TextureViewDimension::e2D,
         wgpu::TextureComponentType::Float},
        {6, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture,
         false, 0, false, wgpu::TextureViewDimension::e2D,
         wgpu::TextureComponentType::Float},
        {7, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture,
         false, 0, false, wgpu::TextureViewDimension::Cube,
         wgpu::TextureComponentType::Float},
    });
  } else {
    mGroupLayoutModel = mContextDawn->MakeBindGroupLayout({
        {0, wgpu::ShaderStage::Vertex, wgpu::BindingType::UniformBuffer},
        {1, wgpu::ShaderStage::Fragment, wgpu::BindingType::UniformBuffer},
        {2, wgpu::ShaderStage::Fragment, wgpu::BindingType::Sampler},
        {3, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture,
         false, 0, false, wgpu::TextureViewDimension::e2D,
         wgpu::TextureComponentType::Float},
        {4, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture,
         false, 0, false, wgpu::TextureViewDimension::e2D,
         wgpu::TextureComponentType::Float},
    });
  }

  mPipelineLayout = mContextDawn->MakeBasicPipelineLayout({
      mContextDawn->groupLayoutGeneral,
      mContextDawn->groupLayoutWorld,
      mGroupLayoutModel,
      mContextDawn->groupLayoutFishPer,
  });

  mPipeline = mContextDawn->createRenderPipeline(
      mPipelineLayout, mProgramDawn, mVertexStateDescriptor, mBlend);

  mFishVertexBuffer = mContextDawn->createBufferFromData(
      &mFishVertexUniforms, sizeof(FishVertexUniforms),
      sizeof(FishVertexUniforms),
      wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);
  mLightFactorBuffer = mContextDawn->createBufferFromData(
      &mLightFactorUniforms, sizeof(LightFactorUniforms),
      sizeof(LightFactorUniforms),
      wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);

  // Fish models includes small, medium and big. Some of them contains
  // reflection and skybox texture, but some doesn't.
  if (mSkyboxTexture && mReflectionTexture) {
    mBindGroupModel = mContextDawn->makeBindGroup(
        mGroupLayoutModel,
        {{0, mFishVertexBuffer, 0, sizeof(FishVertexUniforms), {}, {}},
         {1, mLightFactorBuffer, 0, sizeof(LightFactorUniforms), {}, {}},
         {2, {}, 0, 0, mReflectionTexture->getSampler(), {}},
         {3, {}, 0, 0, mSkyboxTexture->getSampler(), {}},
         {4, {}, 0, 0, {}, mDiffuseTexture->getTextureView()},
         {5, {}, 0, 0, {}, mNormalTexture->getTextureView()},
         {6, {}, 0, 0, {}, mReflectionTexture->getTextureView()},
         {7, {}, 0, 0, {}, mSkyboxTexture->getTextureView()}});
  } else {
    mBindGroupModel = mContextDawn->makeBindGroup(
        mGroupLayoutModel,
        {{0, mFishVertexBuffer, 0, sizeof(FishVertexUniforms), {}, {}},
         {1, mLightFactorBuffer, 0, sizeof(LightFactorUniforms), {}, {}},
         {2, {}, 0, 0, mDiffuseTexture->getSampler(), {}},
         {3, {}, 0, 0, {}, mDiffuseTexture->getTextureView()},
         {4, {}, 0, 0, {}, mNormalTexture->getTextureView()}});
  }

  mContextDawn->setBufferData(mLightFactorBuffer, sizeof(LightFactorUniforms),
                              &mLightFactorUniforms,
                              sizeof(LightFactorUniforms));
  mContextDawn->setBufferData(mFishVertexBuffer, sizeof(FishVertexUniforms),
                              &mFishVertexUniforms,
                              sizeof(LightFactorUniforms));
}

void FishModelDawn::draw() {
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
  pass.SetIndexBufferWithFormat(mIndicesBuffer->getBuffer(),
                                wgpu::IndexFormat::Uint16, 0);

  if (mEnableDynamicBufferOffset) {
    for (int i = 0; i < mCurInstance; i++) {
      uint32_t offset = 256u * (i + mFishPerOffset);
      pass.SetBindGroup(3, mContextDawn->bindGroupFishPers[0], 1, &offset);
      pass.DrawIndexed(mIndicesBuffer->getTotalComponents(), 1, 0, 0, 0);
    }
  } else {
    for (int i = 0; i < mCurInstance; i++) {
      pass.SetBindGroup(3, mContextDawn->bindGroupFishPers[i + mFishPerOffset],
                        0, nullptr);
      pass.DrawIndexed(mIndicesBuffer->getTotalComponents(), 1, 0, 0, 0);
    }
  }
}

void FishModelDawn::updatePerInstanceUniforms(
    const WorldUniforms &worldUniforms) {
}

void FishModelDawn::updateFishPerUniforms(float x,
                                          float y,
                                          float z,
                                          float nextX,
                                          float nextY,
                                          float nextZ,
                                          float scale,
                                          float time,
                                          int index) {
  index += mFishPerOffset;
  mContextDawn->fishPers[index].worldPosition[0] = x;
  mContextDawn->fishPers[index].worldPosition[1] = y;
  mContextDawn->fishPers[index].worldPosition[2] = z;
  mContextDawn->fishPers[index].nextPosition[0] = nextX;
  mContextDawn->fishPers[index].nextPosition[1] = nextY;
  mContextDawn->fishPers[index].nextPosition[2] = nextZ;
  mContextDawn->fishPers[index].scale = scale;
  mContextDawn->fishPers[index].time = time;
}

FishModelDawn::~FishModelDawn() {
  mPipeline = nullptr;
  mGroupLayoutModel = nullptr;
  mPipelineLayout = nullptr;
  mBindGroupModel = nullptr;
  mFishVertexBuffer = nullptr;
  mLightFactorBuffer = nullptr;
}
