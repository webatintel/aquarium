//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelDawn.h: Defnes fish model of Dawn

#ifndef FISHMODELINSTANCEDDRAWDAWN_H
#define FISHMODELINSTANCEDDRAWDAWN_H

#include "dawn/webgpu_cpp.h"
#include "utils/ComboRenderPipelineDescriptor.h"

#include "../FishModel.h"
#include "ContextDawn.h"
#include "ProgramDawn.h"

class FishModelInstancedDrawDawn : public FishModel
{
  public:
    FishModelInstancedDrawDawn(Context *context,
                               Aquarium *aquarium,
                               MODELGROUP type,
                               MODELNAME name,
                               bool blend);
    ~FishModelInstancedDrawDawn();

    void init() override;
    void prepareForDraw() override;
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

    struct FishVertexUniforms
    {
        float fishLength;
        float fishWaveLength;
        float fishBendAmount;
    } mFishVertexUniforms;

    struct LightFactorUniforms
    {
        float shininess;
        float specularFactor;
    } mLightFactorUniforms;

    struct FishPer
    {
        float worldPosition[3];
        float scale;
        float nextPosition[3];
        float time;
    };
    FishPer *mFishPers;

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

    wgpu::Buffer mFishVertexBuffer;
    wgpu::Buffer mLightFactorBuffer;

    wgpu::Buffer mFishPersBuffer;

    int instance;

    ProgramDawn *mProgramDawn;
    ContextDawn *mContextDawn;
};

#endif  // FISHMODELINSTANCEDDRAWDAWN_H
