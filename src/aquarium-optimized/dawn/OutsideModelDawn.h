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
#include "dawn/dawncpp.h"
#include "utils/ComboRenderPipelineDescriptor.h"

#include "../Model.h"

class OutsideModelDawn : public Model
{
  public:
    OutsideModelDawn(const Context *context,
                     Aquarium *aquarium,
                     MODELGROUP type,
                     MODELNAME name,
                     bool blend);
    ~OutsideModelDawn();

    void init() override;
    void prepareForDraw() const override;
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

    WorldUniforms mWorldUniformPer;

  private:
    utils::ComboVertexInputDescriptor mVertexInputDescriptor;
    dawn::RenderPipeline mPipeline;

    dawn::BindGroupLayout mGroupLayoutModel;
    dawn::BindGroupLayout mGroupLayoutPer;
    dawn::PipelineLayout mPipelineLayout;

    dawn::BindGroup mBindGroupModel;
    dawn::BindGroup mBindGroupPer;

    dawn::Buffer mLightFactorBuffer;
    dawn::Buffer mViewBuffer;

    const ContextDawn *mContextDawn;
    ProgramDawn *mProgramDawn;
};

#endif