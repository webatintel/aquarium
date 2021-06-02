//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "GenericModelDawn.h"

#include <vector>

#include "../Aquarium.h"

GenericModelDawn::GenericModelDawn(Context *context,
                                   Aquarium *aquarium,
                                   MODELGROUP type,
                                   MODELNAME name,
                                   bool blend)
    : Model(type, name, blend), instance(0) {
  mContextDawn = static_cast<ContextDawn *>(context);

  mLightFactorUniforms.shininess = 50.0f;
  mLightFactorUniforms.specularFactor = 1.0f;
}

GenericModelDawn::~GenericModelDawn() {
  mPipeline = nullptr;
  mGroupLayoutModel = nullptr;
  mGroupLayoutPer = nullptr;
  mPipelineLayout = nullptr;
  mBindGroupModel = nullptr;
  mBindGroupPer = nullptr;
  mLightFactorBuffer = nullptr;
  mWorldBuffer = nullptr;
}

void GenericModelDawn::init() {
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

  // Generic models use reflection, normal or diffuse shaders, of which
  // groupLayouts are diiferent in texture binding.  MODELGLOBEBASE use diffuse
  // shader though it contains normal and reflection textures.
  std::vector<wgpu::VertexAttribute> vertexAttribute;
  if (mNormalTexture && mName != MODELNAME::MODELGLOBEBASE) {
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
  } else {
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
  }

  // Generic models use reflection, normal or diffuse shaders, of which
  // groupLayouts are diiferent in texture binding.  MODELGLOBEBASE use diffuse
  // shader though it contains normal and reflection textures.
  std::vector<wgpu::VertexBufferLayout> vertexBufferLayout;
  if (mNormalTexture && mName != MODELNAME::MODELGLOBEBASE) {
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
  } else {
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
  }

  mVertexState.module = mVsModule;
  mVertexState.entryPoint = "main";
  mVertexState.bufferCount = static_cast<uint32_t>(vertexBufferLayout.size());
  mVertexState.buffers = vertexBufferLayout.data();

  {
    std::vector<wgpu::BindGroupLayoutEntry> bindGroupLayoutEntry;
    if (mSkyboxTexture && mReflectionTexture &&
        mName != MODELNAME::MODELGLOBEBASE) {
      bindGroupLayoutEntry.resize(7);
      bindGroupLayoutEntry[0].binding = 0;
      bindGroupLayoutEntry[0].visibility = wgpu::ShaderStage::Fragment;
      bindGroupLayoutEntry[0].buffer.type = wgpu::BufferBindingType::Uniform;
      bindGroupLayoutEntry[0].buffer.hasDynamicOffset = false;
      bindGroupLayoutEntry[0].buffer.minBindingSize = 0;
      bindGroupLayoutEntry[1].binding = 1;
      bindGroupLayoutEntry[1].visibility = wgpu::ShaderStage::Fragment;
      bindGroupLayoutEntry[1].sampler.type =
          wgpu::SamplerBindingType::Filtering;
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
          wgpu::TextureViewDimension::Cube;
      bindGroupLayoutEntry[6].texture.multisampled = false;
    } else if (mNormalTexture && mName != MODELNAME::MODELGLOBEBASE) {
      bindGroupLayoutEntry.resize(4);
      bindGroupLayoutEntry[0].binding = 0;
      bindGroupLayoutEntry[0].visibility = wgpu::ShaderStage::Fragment;
      bindGroupLayoutEntry[0].buffer.type = wgpu::BufferBindingType::Uniform;
      bindGroupLayoutEntry[0].buffer.hasDynamicOffset = false;
      bindGroupLayoutEntry[0].buffer.minBindingSize = 0;
      bindGroupLayoutEntry[1].binding = 1;
      bindGroupLayoutEntry[1].visibility = wgpu::ShaderStage::Fragment;
      bindGroupLayoutEntry[1].sampler.type =
          wgpu::SamplerBindingType::Filtering;
      bindGroupLayoutEntry[2].binding = 2;
      bindGroupLayoutEntry[2].visibility = wgpu::ShaderStage::Fragment;
      bindGroupLayoutEntry[2].texture.sampleType =
          wgpu::TextureSampleType::Float;
      bindGroupLayoutEntry[2].texture.viewDimension =
          wgpu::TextureViewDimension::e2D;
      bindGroupLayoutEntry[2].texture.multisampled = false;
      bindGroupLayoutEntry[3].binding = 3;
      bindGroupLayoutEntry[3].visibility = wgpu::ShaderStage::Fragment;
      bindGroupLayoutEntry[3].texture.sampleType =
          wgpu::TextureSampleType::Float;
      bindGroupLayoutEntry[3].texture.viewDimension =
          wgpu::TextureViewDimension::e2D;
      bindGroupLayoutEntry[3].texture.multisampled = false;
    } else {
      bindGroupLayoutEntry.resize(3);
      bindGroupLayoutEntry[0].binding = 0;
      bindGroupLayoutEntry[0].visibility = wgpu::ShaderStage::Fragment;
      bindGroupLayoutEntry[0].buffer.type = wgpu::BufferBindingType::Uniform;
      bindGroupLayoutEntry[0].buffer.hasDynamicOffset = false;
      bindGroupLayoutEntry[0].buffer.minBindingSize = 0;
      bindGroupLayoutEntry[1].binding = 1;
      bindGroupLayoutEntry[1].visibility = wgpu::ShaderStage::Fragment;
      bindGroupLayoutEntry[1].sampler.type =
          wgpu::SamplerBindingType::Filtering;
      bindGroupLayoutEntry[2].binding = 2;
      bindGroupLayoutEntry[2].visibility = wgpu::ShaderStage::Fragment;
      bindGroupLayoutEntry[2].texture.sampleType =
          wgpu::TextureSampleType::Float;
      bindGroupLayoutEntry[2].texture.viewDimension =
          wgpu::TextureViewDimension::e2D;
      bindGroupLayoutEntry[2].texture.multisampled = false;
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

  mLightFactorBuffer = mContextDawn->createBufferFromData(
      &mLightFactorUniforms, sizeof(mLightFactorUniforms),
      sizeof(mLightFactorUniforms),
      wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);
  mWorldBuffer = mContextDawn->createBufferFromData(
      &mWorldUniformPer, sizeof(mWorldUniformPer), sizeof(mWorldUniformPer),
      wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);

  // Generic models use reflection, normal or diffuse shaders, of which
  // grouplayouts are diiferent in texture binding. MODELGLOBEBASE use diffuse
  // shader though it contains normal and reflection textures.
  {
    std::vector<wgpu::BindGroupEntry> bindGroupEntry;
    if (mSkyboxTexture && mReflectionTexture &&
        mName != MODELNAME::MODELGLOBEBASE) {
      bindGroupEntry.resize(7);
      bindGroupEntry[0].binding = 0;
      bindGroupEntry[0].buffer = mLightFactorBuffer;
      bindGroupEntry[0].offset = 0;
      bindGroupEntry[0].size = sizeof(LightFactorUniforms);
      bindGroupEntry[1].binding = 1;
      bindGroupEntry[1].sampler = mReflectionTexture->getSampler();
      bindGroupEntry[2].binding = 2;
      bindGroupEntry[2].sampler = mSkyboxTexture->getSampler();
      bindGroupEntry[3].binding = 3;
      bindGroupEntry[3].textureView = mDiffuseTexture->getTextureView();
      bindGroupEntry[4].binding = 4;
      bindGroupEntry[4].textureView = mNormalTexture->getTextureView();
      bindGroupEntry[5].binding = 5;
      bindGroupEntry[5].textureView = mReflectionTexture->getTextureView();
      bindGroupEntry[6].binding = 6;
      bindGroupEntry[6].textureView = mSkyboxTexture->getTextureView();
    } else if (mNormalTexture && mName != MODELNAME::MODELGLOBEBASE) {
      bindGroupEntry.resize(4);
      bindGroupEntry[0].binding = 0;
      bindGroupEntry[0].buffer = mLightFactorBuffer;
      bindGroupEntry[0].offset = 0;
      bindGroupEntry[0].size = sizeof(LightFactorUniforms);
      bindGroupEntry[1].binding = 1;
      bindGroupEntry[1].sampler = mDiffuseTexture->getSampler();
      bindGroupEntry[2].binding = 2;
      bindGroupEntry[2].textureView = mDiffuseTexture->getTextureView();
      bindGroupEntry[3].binding = 3;
      bindGroupEntry[3].textureView = mNormalTexture->getTextureView();
    } else {
      bindGroupEntry.resize(3);
      bindGroupEntry[0].binding = 0;
      bindGroupEntry[0].buffer = mLightFactorBuffer;
      bindGroupEntry[0].offset = 0;
      bindGroupEntry[0].size = sizeof(LightFactorUniforms);
      bindGroupEntry[1].binding = 1;
      bindGroupEntry[1].sampler = mDiffuseTexture->getSampler();
      bindGroupEntry[2].binding = 2;
      bindGroupEntry[2].textureView = mDiffuseTexture->getTextureView();
    }
    mBindGroupModel =
        mContextDawn->makeBindGroup(mGroupLayoutModel, bindGroupEntry);
  }

  {
    std::vector<wgpu::BindGroupEntry> bindGroupEntry;
    bindGroupEntry.resize(1);
    bindGroupEntry[0].binding = 0;
    bindGroupEntry[0].buffer = mWorldBuffer;
    bindGroupEntry[0].offset = 0;
    bindGroupEntry[0].size = sizeof(WorldUniformPer);
    mBindGroupPer =
        mContextDawn->makeBindGroup(mGroupLayoutPer, bindGroupEntry);
  }

  mContextDawn->setBufferData(mLightFactorBuffer, sizeof(LightFactorUniforms),
                              &mLightFactorUniforms,
                              sizeof(LightFactorUniforms));
}

void GenericModelDawn::prepareForDraw() {
  mContextDawn->updateBufferData(mWorldBuffer, sizeof(WorldUniformPer),
                                 &mWorldUniformPer, sizeof(WorldUniformPer));
}

void GenericModelDawn::draw() {
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
  if (mTangentBuffer && mBiNormalBuffer && mName != MODELNAME::MODELGLOBEBASE) {
    pass.SetVertexBuffer(3, mTangentBuffer->getBuffer());
    pass.SetVertexBuffer(4, mBiNormalBuffer->getBuffer());
  }
  pass.SetIndexBuffer(mIndicesBuffer->getBuffer(), wgpu::IndexFormat::Uint16, 0,
                      0);
  pass.DrawIndexed(mIndicesBuffer->getTotalComponents(), instance, 0, 0, 0);
  instance = 0;
}

void GenericModelDawn::updatePerInstanceUniforms(
    const WorldUniforms &worldUniforms) {
  mWorldUniformPer.WorldUniforms[instance] = worldUniforms;

  instance++;
}
