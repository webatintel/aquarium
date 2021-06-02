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
  const wgpu::ShaderModule &mVsModule = mProgramDawn->getVSModule();

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

  std::vector<wgpu::VertexAttribute> vertexAttribute;
  vertexAttribute.resize(9);
  vertexAttribute[0].format = wgpu::VertexFormat::Float32x3;
  vertexAttribute[0].offset = 0;
  vertexAttribute[0].shaderLocation = 0;
  vertexAttribute[1].format = wgpu::VertexFormat::Float32x3;
  vertexAttribute[1].offset = 0;
  vertexAttribute[1].shaderLocation = 1;
  vertexAttribute[2].format = wgpu::VertexFormat::Float32x2;
  vertexAttribute[2].offset = 0;
  vertexAttribute[2].shaderLocation = 2;
  vertexAttribute[3].format = wgpu::VertexFormat::Float32x3;
  vertexAttribute[3].offset = 0;
  vertexAttribute[3].shaderLocation = 3;
  vertexAttribute[4].format = wgpu::VertexFormat::Float32x3;
  vertexAttribute[4].offset = offsetof(FishPer, worldPosition);
  vertexAttribute[4].shaderLocation = 4;
  vertexAttribute[5].format = wgpu::VertexFormat::Float32x3;
  vertexAttribute[5].offset = 0;
  vertexAttribute[5].shaderLocation = 5;
  vertexAttribute[6].format = wgpu::VertexFormat::Float32;
  vertexAttribute[6].offset = offsetof(FishPer, scale);
  vertexAttribute[6].shaderLocation = 6;
  vertexAttribute[7].format = wgpu::VertexFormat::Float32x3;
  vertexAttribute[7].offset = offsetof(FishPer, nextPosition);
  vertexAttribute[7].shaderLocation = 7;
  vertexAttribute[8].format = wgpu::VertexFormat::Float32;
  vertexAttribute[8].offset = offsetof(FishPer, time);
  vertexAttribute[8].shaderLocation = 8;

  std::vector<wgpu::VertexBufferLayout> vertexBufferLayout;
  vertexBufferLayout.resize(6);
  vertexBufferLayout[0].arrayStride = mPositionBuffer->getDataSize();
  vertexBufferLayout[0].stepMode = wgpu::InputStepMode::Vertex;
  vertexBufferLayout[0].attributeCount = 1;
  vertexBufferLayout[0].attributes = &vertexAttribute[0];
  vertexBufferLayout[1].arrayStride = mNormalBuffer->getDataSize();
  vertexBufferLayout[1].stepMode = wgpu::InputStepMode::Vertex;
  vertexBufferLayout[1].attributeCount = 1;
  vertexBufferLayout[1].attributes = &vertexAttribute[1];
  vertexBufferLayout[2].arrayStride = mTexCoordBuffer->getDataSize();
  vertexBufferLayout[2].stepMode = wgpu::InputStepMode::Vertex;
  vertexBufferLayout[2].attributeCount = 1;
  vertexBufferLayout[2].attributes = &vertexAttribute[2];
  vertexBufferLayout[3].arrayStride = mTangentBuffer->getDataSize();
  vertexBufferLayout[3].stepMode = wgpu::InputStepMode::Vertex;
  vertexBufferLayout[3].attributeCount = 1;
  vertexBufferLayout[3].attributes = &vertexAttribute[3];
  vertexBufferLayout[4].arrayStride = mBiNormalBuffer->getDataSize();
  vertexBufferLayout[4].stepMode = wgpu::InputStepMode::Vertex;
  vertexBufferLayout[4].attributeCount = 1;
  vertexBufferLayout[4].attributes = &vertexAttribute[4];
  vertexBufferLayout[5].arrayStride = sizeof(FishPer);
  vertexBufferLayout[5].stepMode = wgpu::InputStepMode::Instance;
  vertexBufferLayout[5].attributeCount = 4;
  vertexBufferLayout[5].attributes = &vertexAttribute[5];

  mVertexState.module = mVsModule;
  mVertexState.entryPoint = "main";
  mVertexState.bufferCount = static_cast<uint32_t>(vertexBufferLayout.size());
  mVertexState.buffers = vertexBufferLayout.data();

  {
    std::vector<wgpu::BindGroupLayoutEntry> bindGroupLayoutEntry;
    if (mSkyboxTexture && mReflectionTexture) {
      bindGroupLayoutEntry.resize(8);
      bindGroupLayoutEntry[0].binding = 0;
      bindGroupLayoutEntry[0].visibility = wgpu::ShaderStage::Vertex;
      bindGroupLayoutEntry[0].buffer.type = wgpu::BufferBindingType::Uniform;
      bindGroupLayoutEntry[0].buffer.hasDynamicOffset = false;
      bindGroupLayoutEntry[0].buffer.minBindingSize = 0;
      bindGroupLayoutEntry[1].binding = 1;
      bindGroupLayoutEntry[1].visibility = wgpu::ShaderStage::Fragment;
      bindGroupLayoutEntry[1].buffer.type = wgpu::BufferBindingType::Uniform;
      bindGroupLayoutEntry[1].buffer.hasDynamicOffset = false;
      bindGroupLayoutEntry[1].buffer.minBindingSize = 0;
      bindGroupLayoutEntry[2].binding = 2;
      bindGroupLayoutEntry[2].visibility = wgpu::ShaderStage::Fragment;
      bindGroupLayoutEntry[2].sampler.type =
          wgpu::SamplerBindingType::Filtering;
      bindGroupLayoutEntry[3].binding = 3;
      bindGroupLayoutEntry[3].visibility = wgpu::ShaderStage::Fragment;
      bindGroupLayoutEntry[3].sampler.type =
          wgpu::SamplerBindingType::Filtering;
      bindGroupLayoutEntry[4].binding = 4;
      bindGroupLayoutEntry[4].visibility = wgpu::ShaderStage::Fragment;
      bindGroupLayoutEntry[4].texture.sampleType =
          wgpu::TextureSampleType::Float;
      bindGroupLayoutEntry[4].texture.viewDimension =
          wgpu::TextureViewDimension::e2D;
      bindGroupLayoutEntry[4].texture.multisampled = false;
      bindGroupLayoutEntry[5].binding = 5;
      bindGroupLayoutEntry[5].visibility = wgpu::ShaderStage::Fragment;
      bindGroupLayoutEntry[5].texture.sampleType =
          wgpu::TextureSampleType::Float;
      bindGroupLayoutEntry[5].texture.viewDimension =
          wgpu::TextureViewDimension::e2D;
      bindGroupLayoutEntry[5].texture.multisampled = false;
      bindGroupLayoutEntry[6].binding = 6;
      bindGroupLayoutEntry[6].visibility = wgpu::ShaderStage::Fragment;
      bindGroupLayoutEntry[6].texture.sampleType =
          wgpu::TextureSampleType::Float;
      bindGroupLayoutEntry[6].texture.viewDimension =
          wgpu::TextureViewDimension::e2D;
      bindGroupLayoutEntry[6].texture.multisampled = false;
      bindGroupLayoutEntry[7].binding = 7;
      bindGroupLayoutEntry[7].visibility = wgpu::ShaderStage::Fragment;
      bindGroupLayoutEntry[7].texture.sampleType =
          wgpu::TextureSampleType::Float;
      bindGroupLayoutEntry[7].texture.viewDimension =
          wgpu::TextureViewDimension::Cube;
      bindGroupLayoutEntry[7].texture.multisampled = false;
    } else {
      bindGroupLayoutEntry.resize(5);
      bindGroupLayoutEntry[0].binding = 0;
      bindGroupLayoutEntry[0].visibility = wgpu::ShaderStage::Vertex;
      bindGroupLayoutEntry[0].buffer.type = wgpu::BufferBindingType::Uniform;
      bindGroupLayoutEntry[0].buffer.hasDynamicOffset = false;
      bindGroupLayoutEntry[0].buffer.minBindingSize = 0;
      bindGroupLayoutEntry[1].binding = 1;
      bindGroupLayoutEntry[1].visibility = wgpu::ShaderStage::Fragment;
      bindGroupLayoutEntry[1].buffer.type = wgpu::BufferBindingType::Uniform;
      bindGroupLayoutEntry[1].buffer.hasDynamicOffset = false;
      bindGroupLayoutEntry[1].buffer.minBindingSize = 0;
      bindGroupLayoutEntry[2].binding = 2;
      bindGroupLayoutEntry[2].visibility = wgpu::ShaderStage::Fragment;
      bindGroupLayoutEntry[2].sampler.type =
          wgpu::SamplerBindingType::Filtering;
      bindGroupLayoutEntry[3].binding = 3;
      bindGroupLayoutEntry[3].visibility = wgpu::ShaderStage::Fragment;
      bindGroupLayoutEntry[3].texture.sampleType =
          wgpu::TextureSampleType::Float;
      bindGroupLayoutEntry[3].texture.viewDimension =
          wgpu::TextureViewDimension::e2D;
      bindGroupLayoutEntry[3].texture.multisampled = false;
      bindGroupLayoutEntry[4].binding = 4;
      bindGroupLayoutEntry[4].visibility = wgpu::ShaderStage::Fragment;
      bindGroupLayoutEntry[4].texture.sampleType =
          wgpu::TextureSampleType::Float;
      bindGroupLayoutEntry[4].texture.viewDimension =
          wgpu::TextureViewDimension::e2D;
      bindGroupLayoutEntry[4].texture.multisampled = false;
    }
    mGroupLayoutModel = mContextDawn->MakeBindGroupLayout(bindGroupLayoutEntry);
  }

  {
    std::vector<wgpu::BindGroupLayoutEntry> bindGroupLayoutEntry;
    bindGroupLayoutEntry.resize(1);
    bindGroupLayoutEntry[0].binding = 0;
    bindGroupLayoutEntry[0].visibility = wgpu::ShaderStage::Vertex;
    bindGroupLayoutEntry[0].buffer.type = wgpu::BufferBindingType::Uniform;
    bindGroupLayoutEntry[0].buffer.hasDynamicOffset = false;
    bindGroupLayoutEntry[0].buffer.minBindingSize = 0;
    mGroupLayoutPer = mContextDawn->MakeBindGroupLayout(bindGroupLayoutEntry);
  }

  mPipelineLayout = mContextDawn->MakeBasicPipelineLayout({
      mContextDawn->groupLayoutGeneral,
      mContextDawn->groupLayoutWorld,
      mGroupLayoutModel,
      mGroupLayoutPer,
  });

  mPipeline = mContextDawn->createRenderPipeline(mPipelineLayout, mProgramDawn,
                                                 mVertexState, mBlend);

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
  std::vector<wgpu::BindGroupEntry> bindGroupEntry;
  if (mSkyboxTexture && mReflectionTexture) {
    bindGroupEntry.resize(8);
    bindGroupEntry[0].binding = 0;
    bindGroupEntry[0].buffer = mFishVertexBuffer;
    bindGroupEntry[0].offset = 0;
    bindGroupEntry[0].size = sizeof(FishVertexUniforms);
    bindGroupEntry[1].binding = 1;
    bindGroupEntry[1].buffer = mLightFactorBuffer;
    bindGroupEntry[1].offset = 0;
    bindGroupEntry[1].size = sizeof(LightFactorUniforms);
    bindGroupEntry[2].binding = 2;
    bindGroupEntry[2].sampler = mReflectionTexture->getSampler();
    bindGroupEntry[3].binding = 3;
    bindGroupEntry[3].sampler = mSkyboxTexture->getSampler();
    bindGroupEntry[4].binding = 4;
    bindGroupEntry[4].textureView = mDiffuseTexture->getTextureView();
    bindGroupEntry[5].binding = 5;
    bindGroupEntry[5].textureView = mNormalTexture->getTextureView();
    bindGroupEntry[6].binding = 6;
    bindGroupEntry[6].textureView = mReflectionTexture->getTextureView();
    bindGroupEntry[7].binding = 7;
    bindGroupEntry[7].textureView = mSkyboxTexture->getTextureView();
  } else {
    bindGroupEntry.resize(5);
    bindGroupEntry[0].binding = 0;
    bindGroupEntry[0].buffer = mFishVertexBuffer;
    bindGroupEntry[0].offset = 0;
    bindGroupEntry[0].size = sizeof(FishVertexUniforms);
    bindGroupEntry[1].binding = 1;
    bindGroupEntry[1].buffer = mLightFactorBuffer;
    bindGroupEntry[1].offset = 0;
    bindGroupEntry[1].size = sizeof(LightFactorUniforms);
    bindGroupEntry[2].binding = 2;
    bindGroupEntry[2].sampler = mDiffuseTexture->getSampler();
    bindGroupEntry[3].binding = 3;
    bindGroupEntry[3].textureView = mDiffuseTexture->getTextureView();
    bindGroupEntry[4].binding = 4;
    bindGroupEntry[4].textureView = mNormalTexture->getTextureView();
  }
  mBindGroupModel =
      mContextDawn->makeBindGroup(mGroupLayoutModel, bindGroupEntry);

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
  pass.SetIndexBuffer(mIndicesBuffer->getBuffer(), wgpu::IndexFormat::Uint16, 0,
                      0);
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
