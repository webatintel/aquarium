//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// InnerModelDawn.h: Defines inner model of Dawn.

#pragma once
#ifndef INNERMODELDAWN_H
#define INNERMODELDAWN_H 1

#include "ContextDawn.h"
#include "ProgramDawn.h"
#include "dawn/webgpu_cpp.h"
#include "utils/ComboRenderPipelineDescriptor.h"

#include "../Model.h"

class InnerModelDawn : public Model
{
  public:
    InnerModelDawn(const Context *context, Aquarium *aquarium, MODELGROUP type, MODELNAME name, bool blend);
    ~InnerModelDawn();

    void init() override;
    void prepareForDraw() override;
    void draw() override;
    void updatePerInstanceUniforms(const WorldUniforms &WorldUniforms) override;

    struct InnerUniforms
    {
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
    utils::ComboVertexStateDescriptor mVertexStateDescriptor;
    wgpu::RenderPipeline mPipeline;

    wgpu::BindGroupLayout mGroupLayoutModel;
    wgpu::BindGroupLayout mGroupLayoutPer;
    wgpu::PipelineLayout mPipelineLayout;

    wgpu::BindGroup mBindGroupModel;
    wgpu::BindGroup mBindGroupPer;

    wgpu::Buffer mInnerBuffer;
    wgpu::Buffer mViewBuffer;

    const ContextDawn *mContextDawn;
    ProgramDawn *mProgramDawn;
};

#endif // !INNERMODELDAWN_H
