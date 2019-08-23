//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelDawn.h: Defnes fish model of Dawn

#pragma once
#ifndef FISHMODELDAWN_H
#define FISHMODELDAWN_H 1

#include "ContextDawn.h"
#include "ProgramDawn.h"
#include "dawn/dawncpp.h"
#include "utils/ComboRenderPipelineDescriptor.h"

#include "../FishModel.h"

class FishModelDawn : public FishModel
{
  public:
    FishModelDawn(const Context *context,
                  Aquarium *aquarium,
                  MODELGROUP type,
                  MODELNAME name,
                  bool blend);
    ~FishModelDawn();

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

    void reallocResource() override;
    void destoryFishResource() override;

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
        float padding[56];  // TODO(yizhou): the padding is to align with 256 byte offset.
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
    utils::ComboVertexInputDescriptor mVertexInputDescriptor;
    dawn::RenderPipeline mPipeline;

    dawn::BindGroupLayout mGroupLayoutModel;
    dawn::BindGroupLayout mGroupLayoutPer;
    dawn::PipelineLayout mPipelineLayout;

    dawn::BindGroup mBindGroupModel;
    dawn::BindGroup *mBindGroupPers;

    dawn::Buffer mFishVertexBuffer;
    dawn::Buffer mLightFactorBuffer;

    dawn::Buffer mFishPersBuffer;

    ProgramDawn *mProgramDawn;
    const ContextDawn *mContextDawn;

    bool mEnableDynamicBufferOffset;
    Aquarium *mAquarium;
};

#endif
