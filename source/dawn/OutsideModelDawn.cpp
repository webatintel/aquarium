//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "OutsideModelDawn.h"

#include <vector>

OutsideModelDawn::OutsideModelDawn(Context *context,
                                   Aquarium *aquarium,
                                   MODELGROUP type,
                                   MODELNAME name,
                                   bool blend)
    : Model(type, name, blend) {
  mContextDawn = static_cast<ContextDawn *>(context);

  mLightFactorUniforms.shininess = 50.0f;
  mLightFactorUniforms.specularFactor = 0.0f;
}

OutsideModelDawn::~OutsideModelDawn() {
  mPipeline = nullptr;
  mGroupLayoutModel = nullptr;
  mGroupLayoutPer = nullptr;
  mPipelineLayout = nullptr;
  mBindGroupModel = nullptr;
  mBindGroupPer = nullptr;
  mLightFactorBuffer = nullptr;
  mViewBuffer = nullptr;
}

void OutsideModelDawn::init() {
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

  // Outside models use diffuse shaders.
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
  mViewBuffer = mContextDawn->createBufferFromData(
      &mWorldUniformPer, sizeof(WorldUniforms) * 20,
      mContextDawn->CalcConstantBufferByteSize(sizeof(WorldUniforms) * 20),
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
    bindGroupEntry.resize(1);
    bindGroupEntry[0].binding = 0;
    bindGroupEntry[0].buffer = mViewBuffer;
    bindGroupEntry[0].offset = 0;
    bindGroupEntry[0].size =
        mContextDawn->CalcConstantBufferByteSize(sizeof(WorldUniforms) * 20);
    mBindGroupPer =
        mContextDawn->makeBindGroup(mGroupLayoutPer, bindGroupEntry);
  }

  mContextDawn->setBufferData(mLightFactorBuffer, sizeof(LightFactorUniforms),
                              &mLightFactorUniforms,
                              sizeof(LightFactorUniforms));
}

void OutsideModelDawn::prepareForDraw() {
}

void OutsideModelDawn::draw() {
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
  if (mTangentBuffer && mBiNormalBuffer) {
    pass.SetVertexBuffer(3, mTangentBuffer->getBuffer());
    pass.SetVertexBuffer(4, mBiNormalBuffer->getBuffer());
  }
  pass.SetIndexBuffer(mIndicesBuffer->getBuffer(), wgpu::IndexFormat::Uint16, 0,
                      0);
  pass.DrawIndexed(mIndicesBuffer->getTotalComponents(), 1, 0, 0, 0);
}

void OutsideModelDawn::updatePerInstanceUniforms(
    const WorldUniforms &worldUniforms) {
  memcpy(&mWorldUniformPer, &worldUniforms, sizeof(WorldUniforms));

  mContextDawn->updateBufferData(
      mViewBuffer,
      mContextDawn->CalcConstantBufferByteSize(sizeof(WorldUniforms) * 20),
      &mWorldUniformPer, sizeof(WorldUniforms));
}
