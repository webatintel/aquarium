//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelD3D12InstancedDraw.h: Defnes fish model of D3D12

#ifndef FISHMODELD3D12INSTANCEDDRAW_H
#define FISHMODELD3D12INSTANCEDDRAW_H

#include <string>

#include "../FishModel.h"
#include "BufferD3D12.h"
#include "ContextD3D12.h"
#include "ProgramD3D12.h"
#include "TextureD3D12.h"

class FishModelInstancedDrawD3D12 : public FishModel {
public:
  FishModelInstancedDrawD3D12(Context *context,
                              Aquarium *aquarium,
                              MODELGROUP type,
                              MODELNAME name,
                              bool blend);
  ~FishModelInstancedDrawD3D12();

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

  struct FishVertexUniforms {
    float fishLength;
    float fishWaveLength;
    float fishBendAmount;
  } mFishVertexUniforms;

  struct LightFactorUniforms {
    float shininess;
    float specularFactor;
  } mLightFactorUniforms;

  struct FishPer {
    float worldPosition[3];
    float scale;
    float nextPosition[3];
    float time;
  };
  FishPer *mFishPers;

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
  D3D12_VERTEX_BUFFER_VIEW mFishPersBufferView;
  ComPtr<ID3D12Resource> mFishPersBuffer;
  ComPtr<ID3D12Resource> mFishPersUploadBuffer;

  D3D12_CONSTANT_BUFFER_VIEW_DESC mLightFactorView;
  D3D12_GPU_DESCRIPTOR_HANDLE mLightFactorGPUHandle;
  ComPtr<ID3D12Resource> mLightFactorBuffer;
  ComPtr<ID3D12Resource> mLightFactorUploadBuffer;

  D3D12_CONSTANT_BUFFER_VIEW_DESC mFishVertexView;
  D3D12_GPU_DESCRIPTOR_HANDLE mFishVertexGPUHandle;
  ComPtr<ID3D12Resource> mFishVertexBuffer;
  ComPtr<ID3D12Resource> mFishVertexUploadBuffer;

  std::vector<D3D12_INPUT_ELEMENT_DESC> mInputElementDescs;
  D3D12_VERTEX_BUFFER_VIEW mVertexBufferView[6];

  ComPtr<ID3D12RootSignature> mRootSignature;
  ComPtr<ID3D12PipelineState> mPipelineState;

  int instance;

  ProgramD3D12 *mProgramD3D12;
  ContextD3D12 *mContextD3D12;
};

#endif  // FISHMODELINSTANCEDDRAWD3D12_H
