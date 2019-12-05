//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// InnerModelD3D12.cpp: Implements inner model of D3D12.

#include "InnerModelD3D12.h"

InnerModelD3D12::InnerModelD3D12(Context *context,
                                 Aquarium *aquarium,
                                 MODELGROUP type,
                                 MODELNAME name,
                                 bool blend)
    : Model(type, name, blend)
{
    mContextD3D12 = static_cast<ContextD3D12 *>(context);

    mInnerUniforms.eta             = 1.0f;
    mInnerUniforms.tankColorFudge  = 0.796f;
    mInnerUniforms.refractionFudge = 3.0f;
}

void InnerModelD3D12::init()
{
    mProgramD3D12 = static_cast<ProgramD3D12 *>(mProgram);

    mDiffuseTexture    = static_cast<TextureD3D12 *>(textureMap["diffuse"]);
    mNormalTexture     = static_cast<TextureD3D12 *>(textureMap["normalMap"]);
    mReflectionTexture = static_cast<TextureD3D12 *>(textureMap["reflectionMap"]);
    mSkyboxTexture     = static_cast<TextureD3D12 *>(textureMap["skybox"]);

    mPositionBuffer = static_cast<BufferD3D12 *>(bufferMap["position"]);
    mNormalBuffer   = static_cast<BufferD3D12 *>(bufferMap["normal"]);
    mTexCoordBuffer = static_cast<BufferD3D12 *>(bufferMap["texCoord"]);
    mTangentBuffer  = static_cast<BufferD3D12 *>(bufferMap["tangent"]);
    mBiNormalBuffer = static_cast<BufferD3D12 *>(bufferMap["binormal"]);
    mIndicesBuffer  = static_cast<BufferD3D12 *>(bufferMap["indices"]);

    mVertexBufferView[0] = mPositionBuffer->mVertexBufferView;
    mVertexBufferView[1] = mNormalBuffer->mVertexBufferView;
    mVertexBufferView[2] = mTexCoordBuffer->mVertexBufferView;
    mVertexBufferView[3] = mTangentBuffer->mVertexBufferView;
    mVertexBufferView[4] = mBiNormalBuffer->mVertexBufferView;

    mInputElementDescs = {{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                          {"TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0,
                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                          {"TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 2, 0,
                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                          {"TEXCOORD", 3, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0,
                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                          {"TEXCOORD", 4, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0,
                           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}};

    // create constant buffer, desc.
    mInnerBuffer = mContextD3D12->createDefaultBuffer(
        &mInnerUniforms, mContextD3D12->CalcConstantBufferByteSize(sizeof(InnerUniforms)),
        mInnerUploadBuffer);
    mInnerView.BufferLocation = mInnerBuffer->GetGPUVirtualAddress();
    mInnerView.SizeInBytes    = mContextD3D12->CalcConstantBufferByteSize(
        sizeof(InnerUniforms));  // CB size is required to be 256-byte aligned.
    mContextD3D12->buildCbvDescriptor(mInnerView, &mInnerGPUHandle);
    mWorldBuffer = mContextD3D12->createUploadBuffer(
        &mWorldUniformPer, mContextD3D12->CalcConstantBufferByteSize(sizeof(WorldUniforms)));
    mWorldBufferView.BufferLocation = mWorldBuffer->GetGPUVirtualAddress();
    mWorldBufferView.SizeInBytes = mContextD3D12->CalcConstantBufferByteSize(sizeof(WorldUniforms));

    // Create root signature to bind resources.
    // Bind textures, samplers and immutable constant buffers in a descriptor table.
    // Bind frequently updated constant buffers by root descriptors.
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    CD3DX12_ROOT_PARAMETER1 rootParameters[5];
    CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
    rootParameters[0] = mContextD3D12->rootParameterGeneral;
    rootParameters[1] = mContextD3D12->rootParameterWorld;

    mDiffuseTexture->createSrvDescriptor();
    mNormalTexture->createSrvDescriptor();
    mReflectionTexture->createSrvDescriptor();

    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 2,
                   D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0, 2,
                   D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    rootParameters[2].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[3].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);

    rootParameters[4].InitAsConstantBufferView(0, 3, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC,
                                               D3D12_SHADER_VISIBILITY_VERTEX);

    rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 2u,
                               mContextD3D12->staticSamplers.data(),
                               D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    mContextD3D12->createRootSignature(rootSignatureDesc, mRootSignature);

    mContextD3D12->createGraphicsPipelineState(
        mInputElementDescs, mRootSignature, mProgramD3D12->getVSModule(),
        mProgramD3D12->getFSModule(), mPipelineState, mBlend);
}

void InnerModelD3D12::prepareForDraw() {}

void InnerModelD3D12::draw()
{
    auto &commandList = mContextD3D12->mCommandList;

    commandList->SetPipelineState(mPipelineState.Get());
    commandList->SetGraphicsRootSignature(mRootSignature.Get());

    commandList->SetGraphicsRootDescriptorTable(0, mContextD3D12->lightGPUHandle);
    commandList->SetGraphicsRootDescriptorTable(1, mContextD3D12->lightWorldPositionGPUHandle);
    commandList->SetGraphicsRootDescriptorTable(2, mInnerGPUHandle);
    commandList->SetGraphicsRootDescriptorTable(3, mDiffuseTexture->getTextureGPUHandle());
    commandList->SetGraphicsRootConstantBufferView(4, mWorldBufferView.BufferLocation);

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, 5, mVertexBufferView);

    commandList->IASetIndexBuffer(&mIndicesBuffer->mIndexBufferView);

    commandList->DrawIndexedInstanced(mIndicesBuffer->getTotalComponents(), 1, 0, 0, 0);
}

void InnerModelD3D12::updatePerInstanceUniforms(const WorldUniforms &worldUniforms)
{
    memcpy(&mWorldUniformPer, &worldUniforms, sizeof(WorldUniforms));

    CD3DX12_RANGE readRange(0, 0);
    UINT8 *m_pCbvDataBegin;
    mWorldBuffer->Map(0, &readRange, reinterpret_cast<void **>(&m_pCbvDataBegin));
    memcpy(m_pCbvDataBegin, &mWorldUniformPer, sizeof(WorldUniforms));
}
