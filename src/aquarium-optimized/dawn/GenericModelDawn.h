//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// GenericModelDawn.h: Defnes generic model of Dawn

#ifndef GENERICMODELDAWN_H
#define GENERICMODELDAWN_H

#include "dawn/webgpu_cpp.h"
#include "utils/ComboRenderPipelineDescriptor.h"

#include "../Model.h"
#include "ContextDawn.h"
#include "ProgramDawn.h"

class GenericModelDawn : public Model {
public:
  GenericModelDawn(Context *context,
                   Aquarium *aquarium,
                   MODELGROUP type,
                   MODELNAME name,
                   bool blend);
  ~GenericModelDawn();

  void init() override;
  void prepareForDraw() override;
  void draw() override;

  void updatePerInstanceUniforms(const WorldUniforms &worldUniforms) override;

  TextureDawn *mDiffuseTexture;
  TextureDawn *mNormalTexture;
  TextureDawn *mReflectionTexture;
  TextureDawn *mSkyboxTexture;

  BufferDawn *mPositionBuffer;
  BufferDawn *mNormalBuffer;
  BufferDawn *mTexCoordBuffer;
  BufferDawn *mTangentBuffer;
  BufferDawn *mBiNormalBuffer;

  BufferDawn *mIndicesBuffer;

  struct LightFactorUniforms {
    float shininess;
    float specularFactor;
  } mLightFactorUniforms;

  struct WorldUniformPer {
    WorldUniforms WorldUniforms[20];
  };
  WorldUniformPer mWorldUniformPer;

private:
  utils::ComboVertexStateDescriptor mVertexStateDescriptor;
  wgpu::RenderPipeline mPipeline;

  wgpu::BindGroupLayout mGroupLayoutModel;
  wgpu::BindGroupLayout mGroupLayoutPer;
  wgpu::PipelineLayout mPipelineLayout;

  wgpu::BindGroup mBindGroupModel;
  wgpu::BindGroup mBindGroupPer;

  wgpu::Buffer mLightFactorBuffer;
  wgpu::Buffer mWorldBuffer;

  ContextDawn *mContextDawn;
  ProgramDawn *mProgramDawn;

  int instance;
};

#endif  // GENERICMODELDAWN_H
