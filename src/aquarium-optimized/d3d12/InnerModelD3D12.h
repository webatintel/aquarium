//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// InnerModelD3D12.h: Defines inner model of D3D12.

#ifndef INNERMODELD3D12_H
#define INNERMODELD3D12_H

#include "BufferD3D12.h"
#include "ContextD3D12.h"
#include "ProgramD3D12.h"
#include "TextureD3D12.h"

#include "../Model.h"

class InnerModelD3D12 : public Model
{
  public:
    InnerModelD3D12(Context *context,
                    Aquarium *aquarium,
                    MODELGROUP type,
                    MODELNAME name,
                    bool blend);

    void init() override;
    void prepareForDraw() override;
    void draw() override;
    void updatePerInstanceUniforms(const WorldUniforms &worldUniforms) override;

    struct InnerUniforms
    {
        float eta;
        float tankColorFudge;
        float refractionFudge;
        float padding;
    } mInnerUniforms;

    WorldUniforms mWorldUniformPer;

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
    D3D12_CONSTANT_BUFFER_VIEW_DESC mWorldBufferView;
    ComPtr<ID3D12Resource> mWorldBuffer;
    ComPtr<ID3D12Resource> mWorldUploadBuffer;

    D3D12_CONSTANT_BUFFER_VIEW_DESC mInnerView;
    D3D12_GPU_DESCRIPTOR_HANDLE mInnerGPUHandle;
    ComPtr<ID3D12Resource> mInnerBuffer;
    ComPtr<ID3D12Resource> mInnerUploadBuffer;

    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputElementDescs;
    D3D12_VERTEX_BUFFER_VIEW mVertexBufferView[5];

    ComPtr<ID3D12RootSignature> mRootSignature;
    ComPtr<ID3D12PipelineState> mPipelineState;

    ContextD3D12 *mContextD3D12;
    ProgramD3D12 *mProgramD3D12;
};

#endif  // INNERMODELD3D12_H
