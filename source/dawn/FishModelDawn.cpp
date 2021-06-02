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

  std::vector<wgpu::VertexAttribute> vertexAttribute;
  vertexAttribute.resize(5);
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
  vertexAttribute[4].offset = 0;
  vertexAttribute[4].shaderLocation = 4;

  std::vector<wgpu::VertexBufferLayout> vertexBufferLayout;
  vertexBufferLayout.resize(5);
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

  mVertexState.module = mVsModule;
  mVertexState.entryPoint = "main";
  mVertexState.bufferCount = static_cast<uint32_t>(vertexBufferLayout.size());
  mVertexState.buffers = vertexBufferLayout.data();

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
    bindGroupLayoutEntry[2].sampler.type = wgpu::SamplerBindingType::Filtering;
    bindGroupLayoutEntry[3].binding = 3;
    bindGroupLayoutEntry[3].visibility = wgpu::ShaderStage::Fragment;
    bindGroupLayoutEntry[3].sampler.type = wgpu::SamplerBindingType::Filtering;
    bindGroupLayoutEntry[4].binding = 4;
    bindGroupLayoutEntry[4].visibility = wgpu::ShaderStage::Fragment;
    bindGroupLayoutEntry[4].texture.sampleType = wgpu::TextureSampleType::Float;
    bindGroupLayoutEntry[4].texture.viewDimension =
        wgpu::TextureViewDimension::e2D;
    bindGroupLayoutEntry[4].texture.multisampled = false;
    bindGroupLayoutEntry[5].binding = 5;
    bindGroupLayoutEntry[5].visibility = wgpu::ShaderStage::Fragment;
    bindGroupLayoutEntry[5].texture.sampleType = wgpu::TextureSampleType::Float;
    bindGroupLayoutEntry[5].texture.viewDimension =
        wgpu::TextureViewDimension::e2D;
    bindGroupLayoutEntry[5].texture.multisampled = false;
    bindGroupLayoutEntry[6].binding = 6;
    bindGroupLayoutEntry[6].visibility = wgpu::ShaderStage::Fragment;
    bindGroupLayoutEntry[6].texture.sampleType = wgpu::TextureSampleType::Float;
    bindGroupLayoutEntry[6].texture.viewDimension =
        wgpu::TextureViewDimension::e2D;
    bindGroupLayoutEntry[6].texture.multisampled = false;
    bindGroupLayoutEntry[7].binding = 7;
    bindGroupLayoutEntry[7].visibility = wgpu::ShaderStage::Fragment;
    bindGroupLayoutEntry[7].texture.sampleType = wgpu::TextureSampleType::Float;
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
    bindGroupLayoutEntry[2].sampler.type = wgpu::SamplerBindingType::Filtering;
    bindGroupLayoutEntry[3].binding = 3;
    bindGroupLayoutEntry[3].visibility = wgpu::ShaderStage::Fragment;
    bindGroupLayoutEntry[3].texture.sampleType = wgpu::TextureSampleType::Float;
    bindGroupLayoutEntry[3].texture.viewDimension =
        wgpu::TextureViewDimension::e2D;
    bindGroupLayoutEntry[3].texture.multisampled = false;
    bindGroupLayoutEntry[4].binding = 4;
    bindGroupLayoutEntry[4].visibility = wgpu::ShaderStage::Fragment;
    bindGroupLayoutEntry[4].texture.sampleType = wgpu::TextureSampleType::Float;
    bindGroupLayoutEntry[4].texture.viewDimension =
        wgpu::TextureViewDimension::e2D;
    bindGroupLayoutEntry[4].texture.multisampled = false;
  }
  mGroupLayoutModel = mContextDawn->MakeBindGroupLayout(bindGroupLayoutEntry);

  mPipelineLayout = mContextDawn->MakeBasicPipelineLayout({
      mContextDawn->groupLayoutGeneral,
      mContextDawn->groupLayoutWorld,
      mGroupLayoutModel,
      mContextDawn->groupLayoutFishPer,
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
  pass.SetIndexBuffer(mIndicesBuffer->getBuffer(), wgpu::IndexFormat::Uint16, 0,
                      0);

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
