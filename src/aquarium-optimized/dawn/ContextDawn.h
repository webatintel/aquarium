//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// ContextDawn.h : Defines the accessing to graphics API of Dawn.

#pragma once
#ifndef CONTEXTDAWN_H
#define CONTEXTDAWN_H

#include <dawn/dawncpp.h>
#include <dawn_native/DawnNative.h>

#include "GLFW/glfw3.h"
#include "utils/DawnHelpers.h"

#include "../Context.h"

class TextureDawn;
class BufferDawn;
class ProgramDawn;
enum BACKENDTYPE: short;

class ContextDawn : public Context
{
  public:
    ContextDawn(BACKENDTYPE backendType);
    ~ContextDawn();
    bool initialize(
        BACKENDTYPE backend,
        const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset) override;
    void setWindowTitle(const std::string &text) override;
    bool ShouldQuit() override;
    void KeyBoardQuit() override;
    void DoFlush(const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset) override;
    void Flush() override;
    void Terminate() override;
    void showWindow() override;
    void showFPS(const FPSTimer &fpsTimer, int *fishCount) override;
    void destoryImgUI() override;

    void preFrame() override;

    Model *createModel(Aquarium* aquarium, MODELGROUP type, MODELNAME name, bool blend) override;
    Buffer *createBuffer(int numComponents, std::vector<float> *buffer, bool isIndex) override;
    Buffer *createBuffer(int numComponents,
                         std::vector<unsigned short> *buffer,
                         bool isIndex) override;

    Program *createProgram(const std::string &mVId, const std::string &mFId) override;

    Texture *createTexture(const std::string &name, const std::string &url) override;
    Texture *createTexture(const std::string &name, const std::vector<std::string> &urls) override;
    dawn::Texture createTexture(const dawn::TextureDescriptor &descriptor) const;
    dawn::Sampler createSampler(const dawn::SamplerDescriptor &descriptor) const;
    dawn::Buffer createBufferFromData(const void *pixels, int size, dawn::BufferUsage usage) const;
    dawn::BufferCopyView createBufferCopyView(const dawn::Buffer &buffer,
        uint32_t offset,
        uint32_t rowPitch,
        uint32_t imageHeight) const;
    dawn::CommandBuffer copyBufferToTexture(const dawn::BufferCopyView &bufferCopyView,
                                            const dawn::TextureCopyView &textureCopyView,
                                            const dawn::Extent3D &ext3D) const;
    dawn::CommandBuffer copyBufferToBuffer(dawn::Buffer const &srcBuffer,
                                           uint64_t srcOffset,
                                           dawn::Buffer const &destBuffer,
                                           uint64_t destOffset,
                                           uint64_t size);

    dawn::TextureCopyView createTextureCopyView(dawn::Texture texture,
                                                uint32_t level,
                                                uint32_t slice,
                                                dawn::Origin3D origin);
    dawn::ShaderModule createShaderModule(utils::SingleShaderStage stage,
                                          const std::string &str) const;
    dawn::BindGroupLayout  MakeBindGroupLayout(
        std::initializer_list<dawn::BindGroupLayoutBinding> bindingsInitializer) const;
    dawn::PipelineLayout MakeBasicPipelineLayout(
        std::vector<dawn::BindGroupLayout> bindingsInitializer) const;
    dawn::RenderPipeline createRenderPipeline(
        dawn::PipelineLayout mPipelineLayout,
        ProgramDawn *mProgramDawn,
        const dawn::VertexInputDescriptor &mVertexInputDescriptor,
        bool enableBlend) const;
    dawn::TextureView createMultisampledRenderTargetView() const;
    dawn::TextureView createDepthStencilView() const;
    dawn::Buffer createBuffer(uint32_t size, dawn::BufferUsage bit) const;
    void setBufferData(const dawn::Buffer &buffer, uint32_t start, uint32_t size, const void* pixels) const;
    dawn::BindGroup makeBindGroup(
        const dawn::BindGroupLayout &layout,
        std::initializer_list<utils::BindingInitializationHelper> bindingsInitializer) const;

    void initGeneralResources(Aquarium* aquarium) override;
    void updateWorldlUniforms(Aquarium* aquarium) override;
    const dawn::Device &getDevice() const { return mDevice; }
    const dawn::RenderPassEncoder &getRenderPass() const { return mRenderPass; }

    void reallocResource(int preTotalInstance,
                         int curTotalInstance,
                         bool enableDynamicBufferOffset) override;
    void updateAllFishData(
        const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset) override;

    std::vector<dawn::CommandBuffer> mCommandBuffers;
    dawn::Queue queue;

    dawn::BindGroupLayout groupLayoutGeneral;
    dawn::BindGroup bindGroupGeneral;
    dawn::BindGroupLayout groupLayoutWorld;
    dawn::BindGroup bindGroupWorld;

    dawn::BindGroupLayout groupLayoutFishPer;
    dawn::Buffer fishPersBuffer;
    dawn::BindGroup *bindGroupFishPers;

    dawn::Buffer stagingBuffer;

    struct FishPer
    {
        float worldPosition[3];
        float scale;
        float nextPosition[3];
        float time;
        float padding[56];  // TODO(yizhou): the padding is to align with 256 byte offset.
    };

    FishPer *fishPers;

    dawn::Device mDevice;

  private:
    bool GetHardwareAdapter(
        std::unique_ptr<dawn_native::Instance> &instance,
        dawn_native::Adapter *backendAdapter,
        dawn_native::BackendType backendType,
        const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset);
    void initAvailableToggleBitset(BACKENDTYPE backendType) override;
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
    void destoryFishResource();

    static void MapWriteCallback(DawnBufferMapAsyncStatus status, void *, uint64_t, void *userdata);
    void WaitABit();

    // TODO(jiawei.shao@intel.com): remove dawn::TextureUsageBit::CopyDst when the bug in Dawn is
    // fixed.
    static constexpr dawn::TextureUsage kSwapchainBackBufferUsage =
        dawn::TextureUsage::OutputAttachment | dawn::TextureUsage::CopyDst;

    bool mIsSwapchainOutOfDate = false;
    GLFWwindow *mWindow;
    std::unique_ptr<dawn_native::Instance> mInstance;

    dawn::SwapChain mSwapchain;
    dawn::CommandEncoder mCommandEncoder;
    dawn::RenderPassEncoder mRenderPass;
    utils::ComboRenderPassDescriptor mRenderPassDescriptor;

    dawn::Texture mBackbuffer;
    dawn::TextureView mSceneRenderTargetView;
    dawn::TextureView mSceneDepthStencilView;
    dawn::RenderPipeline mPipeline;
    dawn::BindGroup mBindGroup;
    dawn::TextureFormat mPreferredSwapChainFormat;

    dawn::Buffer mLightWorldPositionBuffer;
    dawn::Buffer mLightBuffer;
    dawn::Buffer mFogBuffer;

    bool mEnableMSAA;
    int mPreTotalInstance;
    int mCurTotalInstance;
    bool mEnableDynamicBufferOffset;

    void *mappedData;
};

#endif
