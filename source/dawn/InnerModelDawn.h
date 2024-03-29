//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// InnerModelDawn.h: Defines inner model of Dawn.

#ifndef INNERMODELDAWN_H
#define INNERMODELDAWN_H

#include "dawn/webgpu_cpp.h"

#include "../Model.h"
#include "ContextDawn.h"
#include "ProgramDawn.h"

class InnerModelDawn : public Model {
public:
  InnerModelDawn(Context *context,
                 Aquarium *aquarium,
                 MODELGROUP type,
                 MODELNAME name,
                 bool blend);
  ~InnerModelDawn();

  void init() override;
  void prepareForDraw() override;
  void draw() override;
  void updatePerInstanceUniforms(const WorldUniforms &WorldUniforms) override;

  struct InnerUniforms {
    float eta;
    float tankColorFudge;
    float refractionFudge;
    float padding;
  } mInnerUniforms;

  WorldUniforms mWorldUniformPer;

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
  wgpu::VertexState mVertexState;
  wgpu::RenderPipeline mPipeline;

  wgpu::BindGroupLayout mGroupLayoutModel;
  wgpu::BindGroupLayout mGroupLayoutPer;
  wgpu::PipelineLayout mPipelineLayout;

  wgpu::BindGroup mBindGroupModel;
  wgpu::BindGroup mBindGroupPer;

  wgpu::Buffer mInnerBuffer;
  wgpu::Buffer mViewBuffer;

  ContextDawn *mContextDawn;
  ProgramDawn *mProgramDawn;
};

#endif  // INNERMODELDAWN_H
