//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// SeaweenModelDawn: Implements seaweed model of Dawn.

#include "SeaweedModelDawn.h"

#include <vector>

SeaweedModelDawn::SeaweedModelDawn(Context *context,
                                   Aquarium *aquarium,
                                   MODELGROUP type,
                                   MODELNAME name,
                                   bool blend)
    : SeaweedModel(type, name, blend), instance(0) {
  mContextDawn = static_cast<ContextDawn *>(context);
  mAquarium = aquarium;

  mLightFactorUniforms.shininess = 50.0f;
  mLightFactorUniforms.specularFactor = 1.0f;
}

SeaweedModelDawn::~SeaweedModelDawn() {
  mPipeline = nullptr;
  mGroupLayoutModel = nullptr;
  mGroupLayoutPer = nullptr;
  mPipelineLayout = nullptr;
  mBindGroupModel = nullptr;
  mBindGroupPer = nullptr;
  mLightFactorBuffer = nullptr;
  mViewBuffer = nullptr;
  mTimeBuffer = nullptr;
}

void SeaweedModelDawn::init() {
  mProgramDawn = static_cast<ProgramDawn *>(mProgram);
  const wgpu::ShaderModule &mVsModule = mProgramDawn->getVSModule();

  mDiffuseTexture = static_cast<TextureDawn *>(textureMap["diffuse"]);
  mNormalTexture = static_cast<TextureDawn *>(textureMap["normalMap"]);
  mReflectionTexture = static_cast<TextureDawn *>(textureMap["reflectionMap"]);
  mSkyboxTexture = static_cast<TextureDawn *>(textureMap["skybox"]);

  mPositionBuffer = static_cast<BufferDawn *>(bufferMap["position"]);
  mNormalBuffer = static_cast<BufferDawn *>(bufferMap["normal"]);
  mTexCoordBuffer = static_cast<BufferDawn *>(bufferMap["texCoord"]);
  mIndicesBuffer = static_cast<BufferDawn *>(bufferMap["indices"]);

  std::vector<wgpu::VertexAttribute> vertexAttribute;
  vertexAttribute.resize(3);
  vertexAttribute[0].format = wgpu::VertexFormat::Float32x3;
  vertexAttribute[0].offset = 0;
  vertexAttribute[0].shaderLocation = 0;
  vertexAttribute[1].format = wgpu::VertexFormat::Float32x3;
  vertexAttribute[1].offset = 0;
  vertexAttribute[1].shaderLocation = 1;
  vertexAttribute[2].format = wgpu::VertexFormat::Float32x2;
  vertexAttribute[2].offset = 0;
  vertexAttribute[2].shaderLocation = 2;

  std::vector<wgpu::VertexBufferLayout> vertexBufferLayout;
  vertexBufferLayout.resize(3);
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

  mVertexState.module = mVsModule;
  mVertexState.entryPoint = "main";
  mVertexState.bufferCount = static_cast<uint32_t>(vertexBufferLayout.size());
  mVertexState.buffers = vertexBufferLayout.data();

  {
    std::vector<wgpu::BindGroupLayoutEntry> bindGroupLayoutEntry;
    bindGroupLayoutEntry.resize(3);
    bindGroupLayoutEntry[0].binding = 0;
    bindGroupLayoutEntry[0].visibility = wgpu::ShaderStage::Fragment;
    bindGroupLayoutEntry[0].buffer.type = wgpu::BufferBindingType::Uniform;
    bindGroupLayoutEntry[0].buffer.hasDynamicOffset = false;
    bindGroupLayoutEntry[0].buffer.minBindingSize = 0;
    bindGroupLayoutEntry[1].binding = 1;
    bindGroupLayoutEntry[1].visibility = wgpu::ShaderStage::Fragment;
    bindGroupLayoutEntry[1].sampler.type = wgpu::SamplerBindingType::Filtering;
    bindGroupLayoutEntry[2].binding = 2;
    bindGroupLayoutEntry[2].visibility = wgpu::ShaderStage::Fragment;
    bindGroupLayoutEntry[2].texture.sampleType = wgpu::TextureSampleType::Float;
    bindGroupLayoutEntry[2].texture.viewDimension =
        wgpu::TextureViewDimension::e2D;
    bindGroupLayoutEntry[2].texture.multisampled = false;
    mGroupLayoutModel = mContextDawn->MakeBindGroupLayout(bindGroupLayoutEntry);
  }

  {
    std::vector<wgpu::BindGroupLayoutEntry> bindGroupLayoutEntry;
    bindGroupLayoutEntry.resize(2);
    bindGroupLayoutEntry[0].binding = 0;
    bindGroupLayoutEntry[0].visibility = wgpu::ShaderStage::Vertex;
    bindGroupLayoutEntry[0].buffer.type = wgpu::BufferBindingType::Uniform;
    bindGroupLayoutEntry[0].buffer.hasDynamicOffset = false;
    bindGroupLayoutEntry[0].buffer.minBindingSize = 0;
    bindGroupLayoutEntry[1].binding = 1;
    bindGroupLayoutEntry[1].visibility = wgpu::ShaderStage::Vertex;
    bindGroupLayoutEntry[1].buffer.type = wgpu::BufferBindingType::Uniform;
    bindGroupLayoutEntry[1].buffer.hasDynamicOffset = false;
    bindGroupLayoutEntry[1].buffer.minBindingSize = 0;
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

  mLightFactorBuffer = mContextDawn->createBufferFromData(
      &mLightFactorUniforms, sizeof(mLightFactorUniforms),
      sizeof(mLightFactorUniforms),
      wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);
  mTimeBuffer = mContextDawn->createBufferFromData(
      &mSeaweedPer, sizeof(mSeaweedPer),
      mContextDawn->CalcConstantBufferByteSize(sizeof(mSeaweedPer)),
      wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);
  mViewBuffer = mContextDawn->createBufferFromData(
      &mWorldUniformPer, sizeof(WorldUniformPer),
      mContextDawn->CalcConstantBufferByteSize(sizeof(WorldUniformPer)),
      wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);

  {
    std::vector<wgpu::BindGroupEntry> bindGroupEntry;
    bindGroupEntry.resize(3);
    bindGroupEntry[0].binding = 0;
    bindGroupEntry[0].buffer = mLightFactorBuffer;
    bindGroupEntry[0].offset = 0;
    bindGroupEntry[0].size = sizeof(LightFactorUniforms);
    bindGroupEntry[1].binding = 1;
    bindGroupEntry[1].sampler = mDiffuseTexture->getSampler();
    bindGroupEntry[2].binding = 2;
    bindGroupEntry[2].textureView = mDiffuseTexture->getTextureView();
    mBindGroupModel =
        mContextDawn->makeBindGroup(mGroupLayoutModel, bindGroupEntry);
  }

  {
    std::vector<wgpu::BindGroupEntry> bindGroupEntry;
    bindGroupEntry.resize(2);
    bindGroupEntry[0].binding = 0;
    bindGroupEntry[0].buffer = mViewBuffer;
    bindGroupEntry[0].offset = 0;
    bindGroupEntry[0].size =
        mContextDawn->CalcConstantBufferByteSize(sizeof(WorldUniformPer));
    bindGroupEntry[1].binding = 1;
    bindGroupEntry[1].buffer = mTimeBuffer;
    bindGroupEntry[1].offset = 0;
    bindGroupEntry[1].size =
        mContextDawn->CalcConstantBufferByteSize(sizeof(SeaweedPer));
    mBindGroupPer =
        mContextDawn->makeBindGroup(mGroupLayoutPer, bindGroupEntry);
  }

  mContextDawn->setBufferData(mLightFactorBuffer, sizeof(mLightFactorUniforms),
                              &mLightFactorUniforms,
                              sizeof(mLightFactorUniforms));
}

void SeaweedModelDawn::prepareForDraw() {
  mContextDawn->updateBufferData(
      mViewBuffer,
      mContextDawn->CalcConstantBufferByteSize(sizeof(WorldUniformPer)),
      &mWorldUniformPer, sizeof(WorldUniformPer));
  mContextDawn->updateBufferData(
      mTimeBuffer, mContextDawn->CalcConstantBufferByteSize(sizeof(SeaweedPer)),
      &mSeaweedPer, sizeof(SeaweedPer));
}

void SeaweedModelDawn::draw() {
  wgpu::RenderPassEncoder pass = mContextDawn->getRenderPass();
  pass.SetPipeline(mPipeline);
  pass.SetBindGroup(0, mContextDawn->bindGroupGeneral, 0, nullptr);
  pass.SetBindGroup(1, mContextDawn->bindGroupWorld, 0, nullptr);
  pass.SetBindGroup(2, mBindGroupModel, 0, nullptr);
  pass.SetBindGroup(3, mBindGroupPer, 0, nullptr);
  pass.SetVertexBuffer(0, mPositionBuffer->getBuffer());
  pass.SetVertexBuffer(1, mNormalBuffer->getBuffer());
  pass.SetVertexBuffer(2, mTexCoordBuffer->getBuffer());
  pass.SetIndexBuffer(mIndicesBuffer->getBuffer(), wgpu::IndexFormat::Uint16, 0,
                      0);
  pass.DrawIndexed(mIndicesBuffer->getTotalComponents(), instance, 0, 0, 0);
  instance = 0;
}

void SeaweedModelDawn::updatePerInstanceUniforms(
    const WorldUniforms &worldUniforms) {
  mWorldUniformPer.worldUniforms[instance] = worldUniforms;
  mSeaweedPer.seaweed[instance].time = mAquarium->g.mclock + instance;

  instance++;
}

void SeaweedModelDawn::updateSeaweedModelTime(float time) {
}
