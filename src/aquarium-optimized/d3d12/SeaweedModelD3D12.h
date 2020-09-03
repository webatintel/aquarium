//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// SeaweedModelD3D12.h: Defines seaweed model of D3D12.

#ifndef SEAWEEDMODELD3D12_H
#define SEAWEEDMODELD3D12_H

#include "../SeaweedModel.h"
#include "BufferD3D12.h"
#include "ContextD3D12.h"
#include "ProgramD3D12.h"
#include "TextureD3D12.h"

class SeaweedModelD3D12 : public SeaweedModel
{
public:
  SeaweedModelD3D12(Context *context,
                    Aquarium *aquarium,
                    MODELGROUP type,
                    MODELNAME name,
                    bool blend);

  void init() override;
  void prepareForDraw() override;
  void draw() override;

  void updatePerInstanceUniforms(const WorldUniforms &worldUniforms) override;

  TextureD3D12 *mDiffuseTexture;
  TextureD3D12 *mNormalTexture;
  TextureD3D12 *mReflectionTexture;
  TextureD3D12 *mSkyboxTexture;

  BufferD3D12 *mPositionBuffer;
  BufferD3D12 *mNormalBuffer;
  BufferD3D12 *mTexCoordBuffer;

  BufferD3D12 *mIndicesBuffer;
  void updateSeaweedModelTime(float time) override;

  struct LightFactorUniforms
  {
    float shininess;
    float specularFactor;
  } mLightFactorUniforms;

  struct Seaweed
  {
    float time;
    float padding[3];
  };
  struct SeaweedPer
  {
    Seaweed seaweed[20];
  } mSeaweedPer;

  struct WorldUniformPer
  {
    WorldUniforms worldUniforms[20];
  };
  WorldUniformPer mWorldUniformPer;

private:
  D3D12_CONSTANT_BUFFER_VIEW_DESC mWorldBufferView;
  ComPtr<ID3D12Resource> mWorldBuffer;
  ComPtr<ID3D12Resource> mWorldUploadBuffer;
  D3D12_CONSTANT_BUFFER_VIEW_DESC mSeaweedBufferView;
  ComPtr<ID3D12Resource> mSeaweedBuffer;
  ComPtr<ID3D12Resource> mSeaweedUploadBuffer;

  D3D12_CONSTANT_BUFFER_VIEW_DESC mLightFactorView;
  D3D12_GPU_DESCRIPTOR_HANDLE mLightFactorGPUHandle;
  ComPtr<ID3D12Resource> mLightFactorBuffer;
  ComPtr<ID3D12Resource> mLightFactorUploadBuffer;

  std::vector<D3D12_INPUT_ELEMENT_DESC> mInputElementDescs;
  D3D12_VERTEX_BUFFER_VIEW mVertexBufferView[3];

  ComPtr<ID3D12RootSignature> mRootSignature;
  ComPtr<ID3D12PipelineState> mPipelineState;

  ContextD3D12 *mContextD3D12;
  ProgramD3D12 *mProgramD3D12;
  Aquarium *mAquarium;

  int instance;
};

#endif  // SEAWEEDMODELD3D12_H
