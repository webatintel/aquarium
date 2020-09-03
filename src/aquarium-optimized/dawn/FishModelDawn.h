//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelDawn.h: Defnes fish model of Dawn

#ifndef FISHMODELDAWN_H
#define FISHMODELDAWN_H

#include "dawn/webgpu_cpp.h"
#include "utils/ComboRenderPipelineDescriptor.h"

#include "../FishModel.h"
#include "ContextDawn.h"
#include "ProgramDawn.h"

struct FishPer;

class FishModelDawn : public FishModel {
public:
  FishModelDawn(Context *context,
                Aquarium *aquarium,
                MODELGROUP type,
                MODELNAME name,
                bool blend);
  ~FishModelDawn();

  void init() override;
  void draw() override;

  void updatePerInstanceUniforms(const WorldUniforms &worldUniforms) override;
  void updateFishPerUniforms(float x,
                             float y,
                             float z,
                             float nextX,
                             float nextY,
                             float nextZ,
                             float scale,
                             float time,
                             int index) override;

  struct FishVertexUniforms {
    float fishLength;
    float fishWaveLength;
    float fishBendAmount;
  } mFishVertexUniforms;

  struct LightFactorUniforms {
    float shininess;
    float specularFactor;
  } mLightFactorUniforms;

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

private:
  utils::ComboVertexStateDescriptor mVertexStateDescriptor;
  wgpu::RenderPipeline mPipeline;

  wgpu::BindGroupLayout mGroupLayoutModel;
  wgpu::PipelineLayout mPipelineLayout;

  wgpu::BindGroup mBindGroupModel;

  wgpu::Buffer mFishVertexBuffer;
  wgpu::Buffer mLightFactorBuffer;

  ProgramDawn *mProgramDawn;
  ContextDawn *mContextDawn;

  bool mEnableDynamicBufferOffset;
};

#endif  // FISHMODELDAWN_H
