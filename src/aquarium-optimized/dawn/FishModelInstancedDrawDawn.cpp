//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelDawn.cpp: Implements fish model of Dawn.

#include "FishModelInstancedDrawDawn.h"

#include <vector>

#include "BufferDawn.h"

FishModelInstancedDrawDawn::FishModelInstancedDrawDawn(Context *context,
                                                       Aquarium *aquarium,
                                                       MODELGROUP type,
                                                       MODELNAME name,
                                                       bool blend)
    : FishModel(type, name, blend, aquarium), instance(0) {
  mContextDawn = static_cast<ContextDawn *>(context);

  mLightFactorUniforms.shininess = 5.0f;
  mLightFactorUniforms.specularFactor = 0.3f;

  const Fish &fishInfo =
      fishTable[name - MODELNAME::MODELSMALLFISHAINSTANCEDDRAWS];
  mFishVertexUniforms.fishLength = fishInfo.fishLength;
  mFishVertexUniforms.fishBendAmount = fishInfo.fishBendAmount;
  mFishVertexUniforms.fishWaveLength = fishInfo.fishWaveLength;

  instance =
      aquarium->fishCount[fishInfo.modelName - MODELNAME::MODELSMALLFISHA];
  mFishPers = new FishPer[instance];
}

void FishModelInstancedDrawDawn::init() {
  if (instance == 0)
    return;

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

  wgpu::BufferDescriptor bufferDescriptor;
  bufferDescriptor.usage =
      wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
  bufferDescriptor.size = sizeof(FishPer) * instance;
  bufferDescriptor.mappedAtCreation = false;
  mFishPersBuffer = mContextDawn->createBuffer(bufferDescriptor);

  std::vector<wgpu::VertexAttributeDescriptor> vertexAttributeDescriptor;
  vertexAttributeDescriptor.resize(9);
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
  vertexAttributeDescriptor[4].offset = offsetof(FishPer, worldPosition);
  vertexAttributeDescriptor[4].shaderLocation = 4;
  vertexAttributeDescriptor[5].format = wgpu::VertexFormat::Float3;
  vertexAttributeDescriptor[5].offset = 0;
  vertexAttributeDescriptor[5].shaderLocation = 5;
  vertexAttributeDescriptor[6].format = wgpu::VertexFormat::Float;
  vertexAttributeDescriptor[6].offset = offsetof(FishPer, scale);
  vertexAttributeDescriptor[6].shaderLocation = 6;
  vertexAttributeDescriptor[7].format = wgpu::VertexFormat::Float3;
  vertexAttributeDescriptor[7].offset = offsetof(FishPer, nextPosition);
  vertexAttributeDescriptor[7].shaderLocation = 7;
  vertexAttributeDescriptor[8].format = wgpu::VertexFormat::Float;
  vertexAttributeDescriptor[8].offset = offsetof(FishPer, time);
  vertexAttributeDescriptor[8].shaderLocation = 8;

  std::vector<wgpu::VertexBufferLayoutDescriptor> vertexBufferLayoutDescriptor;
  vertexBufferLayoutDescriptor.resize(6);
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
  vertexBufferLayoutDescriptor[5].arrayStride = sizeof(FishPer);
  vertexBufferLayoutDescriptor[5].stepMode = wgpu::InputStepMode::Instance;
  vertexBufferLayoutDescriptor[5].attributeCount = 4;
  vertexBufferLayoutDescriptor[5].attributes = &vertexAttributeDescriptor[5];

  mVertexStateDescriptor.vertexBufferCount =
      static_cast<uint32_t>(vertexBufferLayoutDescriptor.size());
  mVertexStateDescriptor.vertexBuffers = vertexBufferLayoutDescriptor.data();
  mVertexStateDescriptor.indexFormat = wgpu::IndexFormat::Uint16;

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

  mGroupLayoutPer = mContextDawn->MakeBindGroupLayout({
      {0, wgpu::ShaderStage::Vertex, wgpu::BindingType::UniformBuffer},
  });

  mPipelineLayout = mContextDawn->MakeBasicPipelineLayout({
      mContextDawn->groupLayoutGeneral,
      mContextDawn->groupLayoutWorld,
      mGroupLayoutModel,
      mGroupLayoutPer,
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
                              &mFishVertexUniforms, sizeof(FishVertexUniforms));
}

void FishModelInstancedDrawDawn::prepareForDraw() {
}

void FishModelInstancedDrawDawn::draw() {
  if (instance == 0)
    return;

  mContextDawn->setBufferData(mFishPersBuffer, sizeof(FishPer) * instance,
                              mFishPers, sizeof(FishPer) * instance);

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
  pass.SetVertexBuffer(5, mFishPersBuffer);
  pass.SetIndexBuffer(mIndicesBuffer->getBuffer(), 0);
  pass.DrawIndexed(mIndicesBuffer->getTotalComponents(), instance, 0, 0, 0);
}

void FishModelInstancedDrawDawn::updatePerInstanceUniforms(
    const WorldUniforms &worldUniforms) {
}

void FishModelInstancedDrawDawn::updateFishPerUniforms(float x,
                                                       float y,
                                                       float z,
                                                       float nextX,
                                                       float nextY,
                                                       float nextZ,
                                                       float scale,
                                                       float time,
                                                       int index) {
  mFishPers[index].worldPosition[0] = x;
  mFishPers[index].worldPosition[1] = y;
  mFishPers[index].worldPosition[2] = z;
  mFishPers[index].nextPosition[0] = nextX;
  mFishPers[index].nextPosition[1] = nextY;
  mFishPers[index].nextPosition[2] = nextZ;
  mFishPers[index].scale = scale;
  mFishPers[index].time = time;
}

FishModelInstancedDrawDawn::~FishModelInstancedDrawDawn() {
  mPipeline = nullptr;
  mGroupLayoutModel = nullptr;
  mGroupLayoutPer = nullptr;
  mPipelineLayout = nullptr;
  mBindGroupModel = nullptr;
  mBindGroupPer = nullptr;
  mFishVertexBuffer = nullptr;
  mLightFactorBuffer = nullptr;
  mFishPersBuffer = nullptr;
  delete mFishPers;
}
