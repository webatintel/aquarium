//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// SeaweedModelDawn.h: Defines seaweed model of Dawn.

#ifndef SEAWEEDMODELDAWN_H
#define SEAWEEDMODELDAWN_H

#include "ContextDawn.h"
#include "ProgramDawn.h"
#include "dawn/webgpu_cpp.h"
#include "utils/ComboRenderPipelineDescriptor.h"

#include "../SeaweedModel.h"

class SeaweedModelDawn : public SeaweedModel
{
  public:
    SeaweedModelDawn(Context *context,
                     Aquarium *aquarium,
                     MODELGROUP type,
                     MODELNAME name,
                     bool blend);
    ~SeaweedModelDawn();

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
    utils::ComboVertexStateDescriptor mVertexStateDescriptor;
    wgpu::RenderPipeline mPipeline;

    wgpu::BindGroupLayout mGroupLayoutModel;
    wgpu::BindGroupLayout mGroupLayoutPer;
    wgpu::PipelineLayout mPipelineLayout;

    wgpu::BindGroup mBindGroupModel;
    wgpu::BindGroup mBindGroupPer;

    wgpu::Buffer mLightFactorBuffer;
    wgpu::Buffer mTimeBuffer;
    wgpu::Buffer mViewBuffer;

    ContextDawn *mContextDawn;
    ProgramDawn *mProgramDawn;
    Aquarium * mAquarium;

    int instance;
};

#endif  // SEAWEEDMODELDAWN_H
