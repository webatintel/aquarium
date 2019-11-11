//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// OutsideModelDawn.h: Defnes outside model of Dawn

#pragma once
#ifndef OUTSIDEMODELDAWN_H
#define OUTSIDEMODELDAWN_H 1

#include "ContextDawn.h"
#include "ProgramDawn.h"
#include "dawn/webgpu_cpp.h"
#include "utils/ComboRenderPipelineDescriptor.h"

#include "../Model.h"

class OutsideModelDawn : public Model
{
public:
    OutsideModelDawn(const Context *context, Aquarium *aquarium, MODELGROUP type, MODELNAME name, bool blend);
    ~OutsideModelDawn();

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

    struct LightFactorUniforms
    {
        float shininess;
        float specularFactor;
    } mLightFactorUniforms;

    WorldUniforms mWorldUniformPer[20];

  private:
    utils::ComboVertexStateDescriptor mVertexStateDescriptor;
    wgpu::RenderPipeline mPipeline;

    wgpu::BindGroupLayout mGroupLayoutModel;
    wgpu::BindGroupLayout mGroupLayoutPer;
    wgpu::PipelineLayout mPipelineLayout;

    wgpu::BindGroup mBindGroupModel;
    wgpu::BindGroup mBindGroupPer;

    wgpu::Buffer mLightFactorBuffer;
    wgpu::Buffer mViewBuffer;

    const ContextDawn *mContextDawn;
    ProgramDawn *mProgramDawn;
};

#endif