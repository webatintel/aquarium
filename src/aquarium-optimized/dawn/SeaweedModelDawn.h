//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// SeaweedModelDawn.h: Defines seaweed model of Dawn.

#pragma once
#ifndef SEAWEEDMODELDAWN_H
#define SEAWEEDMODELDAWN_H 1

#include "ContextDawn.h"
#include "ProgramDawn.h"
#include "dawn/dawncpp.h"
#include "utils/ComboRenderPipelineDescriptor.h"

#include "../SeaweedModel.h"

class SeaweedModelDawn : public SeaweedModel
{
  public:
    SeaweedModelDawn(const Context *context, Aquarium *aquarium, MODELGROUP type, MODELNAME name, bool blend);
    ~SeaweedModelDawn();

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

    BufferDawn *mIndicesBuffer;
    void updateSeaweedModelTime(float time) override;

    struct LightFactorUniforms
    {
        float shininess;
        float specularFactor;
    } mLightFactorUniforms;

    struct SeaweedPer
    {
        float time[20];
    } mSeaweedPer;

    struct WorldUniformPer
    {
        WorldUniforms worldUniforms[20];
    };
    WorldUniformPer mWorldUniformPer;

  private:
    utils::ComboVertexInputDescriptor mVertexInputDescriptor;
    dawn::RenderPipeline mPipeline;

    dawn::BindGroupLayout mGroupLayoutModel;
    dawn::BindGroupLayout mGroupLayoutPer;
    dawn::PipelineLayout mPipelineLayout;

    dawn::BindGroup mBindGroupModel;
    dawn::BindGroup mBindGroupPer;

    dawn::Buffer mLightFactorBuffer;
    dawn::Buffer mTimeBuffer;
    dawn::Buffer mViewBuffer;

    const ContextDawn *mContextDawn;
    ProgramDawn *mProgramDawn;
    Aquarium * mAquarium;

    int instance;
};

#endif // !SEAWEEDMODEL_H