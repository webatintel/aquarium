//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// GenericModelDawn.h: Defnes generic model of Dawn

#pragma once
#ifndef GENERICMODELDAWN_H
#define GENERICMODELDAWN_H 1

#include "ContextDawn.h"
#include "ProgramDawn.h"
#include "dawn/dawncpp.h"
#include "utils/ComboRenderPipelineDescriptor.h"

#include "../Model.h"

class GenericModelDawn : public Model
{
  public:
    GenericModelDawn(const Context *context,
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

    struct LightFactorUniforms
    {
        float shininess;
        float specularFactor;
    } mLightFactorUniforms;

    struct WorldUniformPer
    {
        WorldUniforms WorldUniforms[20];
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
    dawn::Buffer mWorldBuffer;

    const ContextDawn *mContextDawn;
    ProgramDawn *mProgramDawn;

    int instance;
};

#endif