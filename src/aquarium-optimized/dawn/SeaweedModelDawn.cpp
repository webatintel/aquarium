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

  mDiffuseTexture = static_cast<TextureDawn *>(textureMap["diffuse"]);
  mNormalTexture = static_cast<TextureDawn *>(textureMap["normalMap"]);
  mReflectionTexture = static_cast<TextureDawn *>(textureMap["reflectionMap"]);
  mSkyboxTexture = static_cast<TextureDawn *>(textureMap["skybox"]);

  mPositionBuffer = static_cast<BufferDawn *>(bufferMap["position"]);
  mNormalBuffer = static_cast<BufferDawn *>(bufferMap["normal"]);
  mTexCoordBuffer = static_cast<BufferDawn *>(bufferMap["texCoord"]);
  mIndicesBuffer = static_cast<BufferDawn *>(bufferMap["indices"]);

  std::vector<wgpu::VertexAttributeDescriptor> vertexAttributeDescriptor;
  vertexAttributeDescriptor.resize(3);
  vertexAttributeDescriptor[0].format = wgpu::VertexFormat::Float3;
  vertexAttributeDescriptor[0].offset = 0;
  vertexAttributeDescriptor[0].shaderLocation = 0;
  vertexAttributeDescriptor[1].format = wgpu::VertexFormat::Float3;
  vertexAttributeDescriptor[1].offset = 0;
  vertexAttributeDescriptor[1].shaderLocation = 1;
  vertexAttributeDescriptor[2].format = wgpu::VertexFormat::Float2;
  vertexAttributeDescriptor[2].offset = 0;
  vertexAttributeDescriptor[2].shaderLocation = 2;

  std::vector<wgpu::VertexBufferLayoutDescriptor> vertexBufferLayoutDescriptor;
  vertexBufferLayoutDescriptor.resize(3);
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

  mVertexStateDescriptor.vertexBufferCount =
      static_cast<uint32_t>(vertexBufferLayoutDescriptor.size());
  mVertexStateDescriptor.vertexBuffers = vertexBufferLayoutDescriptor.data();

  mGroupLayoutModel = mContextDawn->MakeBindGroupLayout({
      {0, wgpu::ShaderStage::Fragment, wgpu::BindingType::UniformBuffer},
      {1, wgpu::ShaderStage::Fragment, wgpu::BindingType::Sampler},
      {2, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture, false,
       0, false, wgpu::TextureViewDimension::e2D,
       wgpu::TextureComponentType::Float},
  });

  mGroupLayoutPer = mContextDawn->MakeBindGroupLayout({
      {0, wgpu::ShaderStage::Vertex, wgpu::BindingType::UniformBuffer},
      {1, wgpu::ShaderStage::Vertex, wgpu::BindingType::UniformBuffer},
  });

  mPipelineLayout = mContextDawn->MakeBasicPipelineLayout({
      mContextDawn->groupLayoutGeneral,
      mContextDawn->groupLayoutWorld,
      mGroupLayoutModel,
      mGroupLayoutPer,
  });

  mPipeline = mContextDawn->createRenderPipeline(
      mPipelineLayout, mProgramDawn, mVertexStateDescriptor, mBlend);

  mLightFactorBuffer = mContextDawn->createBufferFromData(
      &mLightFactorUniforms, sizeof(mLightFactorUniforms),
      sizeof(mLightFactorUniforms),
      wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);
  mTimeBuffer = mContextDawn->createBufferFromData(
      &mSeaweedPer, sizeof(mSeaweedPer),
      mContextDawn->CalcConstantBufferByteSize(sizeof(mSeaweedPer) * 4),
      wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);
  mViewBuffer = mContextDawn->createBufferFromData(
      &mWorldUniformPer, sizeof(WorldUniformPer),
      mContextDawn->CalcConstantBufferByteSize(sizeof(WorldUniformPer)),
      wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);

  mBindGroupModel = mContextDawn->makeBindGroup(
      mGroupLayoutModel,
      {
          {0, mLightFactorBuffer, 0, sizeof(LightFactorUniforms), {}, {}},
          {1, {}, 0, 0, mDiffuseTexture->getSampler(), {}},
          {2, {}, 0, 0, {}, mDiffuseTexture->getTextureView()},
      });

  mBindGroupPer = mContextDawn->makeBindGroup(
      mGroupLayoutPer,
      {
          {0,
           mViewBuffer,
           0,
           mContextDawn->CalcConstantBufferByteSize(sizeof(WorldUniformPer)),
           {},
           {}},
          {1,
           mTimeBuffer,
           0,
           mContextDawn->CalcConstantBufferByteSize(sizeof(SeaweedPer) * 4),
           {},
           {}},
      });

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
      mTimeBuffer,
      mContextDawn->CalcConstantBufferByteSize(sizeof(SeaweedPer) * 4),
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
  pass.SetIndexBufferWithFormat(mIndicesBuffer->getBuffer(),
                                wgpu::IndexFormat::Uint16, 0);
  pass.DrawIndexed(mIndicesBuffer->getTotalComponents(), instance, 0, 0, 0);
  instance = 0;
}

void SeaweedModelDawn::updatePerInstanceUniforms(
    const WorldUniforms &worldUniforms) {
  mWorldUniformPer.worldUniforms[instance] = worldUniforms;
  mSeaweedPer.time[instance] = mAquarium->g.mclock + instance;

  instance++;
}

void SeaweedModelDawn::updateSeaweedModelTime(float time) {
}
