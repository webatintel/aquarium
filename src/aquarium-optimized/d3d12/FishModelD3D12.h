//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelD3D12.h: Defnes fish model of D3D12

#ifndef FISHMODELD3D12_H
#define FISHMODELD3D12_H

#include <string>

#include "BufferD3D12.h"
#include "ContextD3D12.h"
#include "ProgramD3D12.h"
#include "TextureD3D12.h"

#include "../FishModel.h"

class FishModelD3D12 : public FishModel
{
  public:
    FishModelD3D12(Context *context,
                   Aquarium *aquarium,
                   MODELGROUP type,
                   MODELNAME name,
                   bool blend);
    ~FishModelD3D12();

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

    TextureD3D12 *mDiffuseTexture;
    TextureD3D12 *mNormalTexture;
    TextureD3D12 *mReflectionTexture;
    TextureD3D12 *mSkyboxTexture;

    BufferD3D12 *mPositionBuffer;
    BufferD3D12 *mNormalBuffer;
    BufferD3D12 *mTexCoordBuffer;
    BufferD3D12 *mTangentBuffer;
    BufferD3D12 *mBiNormalBuffer;

    BufferD3D12 *mIndicesBuffer;

  private:
    D3D12_CONSTANT_BUFFER_VIEW_DESC mLightFactorView;
    D3D12_GPU_DESCRIPTOR_HANDLE mLightFactorGPUHandle;
    ComPtr<ID3D12Resource> mLightFactorBuffer;
    ComPtr<ID3D12Resource> mLightFactorUploadBuffer;

    D3D12_CONSTANT_BUFFER_VIEW_DESC mFishVertexView;
    D3D12_GPU_DESCRIPTOR_HANDLE mFishVertexGPUHandle;
    ComPtr<ID3D12Resource> mFishVertexBuffer;
    ComPtr<ID3D12Resource> mFishVertexUploadBuffer;

    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputElementDescs;

    D3D12_VERTEX_BUFFER_VIEW mVertexBufferView[5];

    ComPtr<ID3D12RootSignature> mRootSignature;

    ComPtr<ID3D12PipelineState> mPipelineState;

    ProgramD3D12 *mProgramD3D12;
    ContextD3D12 *mContextD3D12;
};

#endif  // FISHMODELD3D12_H
