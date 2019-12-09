//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelD3D12.cpp: Implements fish model of D3D12.

#include "BufferD3D12.h"
#include "FishModelInstancedDrawD3D12.h"

FishModelInstancedDrawD3D12::FishModelInstancedDrawD3D12(Context *context,
                                                         Aquarium *aquarium,
                                                         MODELGROUP type,
                                                         MODELNAME name,
                                                         bool blend)
    : FishModel(type, name, blend, aquarium), instance(0)
{
    mContextD3D12 = static_cast<ContextD3D12 *>(context);

    const Fish &fishInfo              = fishTable[name - MODELNAME::MODELSMALLFISHAINSTANCEDDRAWS];
    mFishVertexUniforms.fishLength     = fishInfo.fishLength;
    mFishVertexUniforms.fishBendAmount = fishInfo.fishBendAmount;
    mFishVertexUniforms.fishWaveLength = fishInfo.fishWaveLength;

    mLightFactorUniforms.shininess      = 5.0f;
    mLightFactorUniforms.specularFactor = 0.3f;

    instance = aquarium->fishCount[fishInfo.modelName - MODELNAME::MODELSMALLFISHA];
    mFishPers = new FishPer[instance];
}

FishModelInstancedDrawD3D12::~FishModelInstancedDrawD3D12()
{
    delete mFishPers;
}

void FishModelInstancedDrawD3D12::init()
{
    if (instance == 0)
        return;

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

    mFishPersBuffer = mContextD3D12->createDefaultBuffer(
        mFishPers, mContextD3D12->CalcConstantBufferByteSize(sizeof(FishPer) * instance),
        mFishPersUploadBuffer);
    mFishPersBufferView.BufferLocation = mFishPersBuffer->GetGPUVirtualAddress();
    mFishPersBufferView.SizeInBytes =
        mContextD3D12->CalcConstantBufferByteSize(sizeof(FishPer) * instance);
    mFishPersBufferView.StrideInBytes = sizeof(FishPer);

    mVertexBufferView[5] = mFishPersBufferView;

    mInputElementDescs = {
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
         0},
        {"TEXCOORD", 3, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 4, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 5, DXGI_FORMAT_R32G32B32_FLOAT, 5, 0,
         D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
        {"TEXCOORD", 6, DXGI_FORMAT_R32_FLOAT, 5, 3 * sizeof(float),
         D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
        {"TEXCOORD", 7, DXGI_FORMAT_R32G32B32_FLOAT, 5, 4 * sizeof(float),
         D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
        {"TEXCOORD", 8, DXGI_FORMAT_R32_FLOAT, 5, 5 * sizeof(float),
         D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
    };

    // create constant buffer, desc.
    mFishVertexBuffer = mContextD3D12->createDefaultBuffer(
        &mFishVertexUniforms, mContextD3D12->CalcConstantBufferByteSize(sizeof(FishVertexUniforms)),
        mFishVertexUploadBuffer);
    mFishVertexView.BufferLocation = mFishVertexBuffer->GetGPUVirtualAddress();
    mFishVertexView.SizeInBytes    = mContextD3D12->CalcConstantBufferByteSize(
        sizeof(mFishVertexUniforms));  // CB size is required to be 256-byte aligned.
    mContextD3D12->buildCbvDescriptor(mFishVertexView, &mFishVertexGPUHandle);
    mLightFactorBuffer = mContextD3D12->createDefaultBuffer(
        &mLightFactorUniforms,
        mContextD3D12->CalcConstantBufferByteSize(sizeof(LightFactorUniforms)),
        mLightFactorUploadBuffer);
    mLightFactorView.BufferLocation = mLightFactorBuffer->GetGPUVirtualAddress();
    mLightFactorView.SizeInBytes    = mContextD3D12->CalcConstantBufferByteSize(
        sizeof(LightFactorUniforms));  // CB size is required to be 256-byte aligned.
    mContextD3D12->buildCbvDescriptor(mLightFactorView, &mLightFactorGPUHandle);

    // Create root signature to bind resources.
    // Bind textures, samplers and immutable constant buffers in a descriptor table.
    // Bind frequently updated constant buffers by root descriptors.
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    CD3DX12_ROOT_PARAMETER1 rootParameters[4];
    CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
    rootParameters[0] = mContextD3D12->rootParameterGeneral;
    rootParameters[1] = mContextD3D12->rootParameterWorld;

    if (mSkyboxTexture && mReflectionTexture)
    {
        mDiffuseTexture->createSrvDescriptor();
        mNormalTexture->createSrvDescriptor();
        mReflectionTexture->createSrvDescriptor();

        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 2, 0, 2,
                       D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0, 2,
                       D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        rootParameters[2].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);
        rootParameters[3].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
    }
    else
    {
        mDiffuseTexture->createSrvDescriptor();
        mNormalTexture->createSrvDescriptor();

        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 2, 0, 2,
                       D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 2,
                       D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        rootParameters[2].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);
        rootParameters[3].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
    }

    rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 2u,
                               mContextD3D12->staticSamplers.data(),
                               D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    mContextD3D12->createRootSignature(rootSignatureDesc, mRootSignature);

    mContextD3D12->createGraphicsPipelineState(
        mInputElementDescs, mRootSignature, mProgramD3D12->getVSModule(),
        mProgramD3D12->getFSModule(), mPipelineState, mBlend);
}

void FishModelInstancedDrawD3D12::prepareForDraw()
{
    mContextD3D12->updateConstantBufferSync(mFishPersBuffer, mFishPersUploadBuffer, mFishPers,
                                            sizeof(FishPer) * instance);
}

void FishModelInstancedDrawD3D12::draw()
{
    if (instance == 0)
        return;

    mContextD3D12->mCommandList->SetPipelineState(mPipelineState.Get());
    mContextD3D12->mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

    mContextD3D12->mCommandList->SetGraphicsRootDescriptorTable(0, mContextD3D12->lightGPUHandle);
    mContextD3D12->mCommandList->SetGraphicsRootDescriptorTable(
        1, mContextD3D12->lightWorldPositionGPUHandle);
    mContextD3D12->mCommandList->SetGraphicsRootDescriptorTable(2, mFishVertexGPUHandle);
    mContextD3D12->mCommandList->SetGraphicsRootDescriptorTable(
        3, mDiffuseTexture->getTextureGPUHandle());

    mContextD3D12->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    mContextD3D12->mCommandList->IASetVertexBuffers(0, 6, mVertexBufferView);
    mContextD3D12->mCommandList->IASetIndexBuffer(&mIndicesBuffer->mIndexBufferView);

    mContextD3D12->mCommandList->DrawIndexedInstanced(mIndicesBuffer->getTotalComponents(),
                                                      instance, 0, 0, 0);
}

void FishModelInstancedDrawD3D12::updatePerInstanceUniforms(const WorldUniforms &worldUniforms) {}

void FishModelInstancedDrawD3D12::updateFishPerUniforms(float x,
                                                        float y,
                                                        float z,
                                                        float nextX,
                                                        float nextY,
                                                        float nextZ,
                                                        float scale,
                                                        float time,
                                                        int index)
{
    mFishPers[index].worldPosition[0] = x;
    mFishPers[index].worldPosition[1] = y;
    mFishPers[index].worldPosition[2] = z;
    mFishPers[index].nextPosition[0]  = nextX;
    mFishPers[index].nextPosition[1]  = nextY;
    mFishPers[index].nextPosition[2]  = nextZ;
    mFishPers[index].scale            = scale;
    mFishPers[index].time             = time;
}

