//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// ContextDawn.h : Defines the accessing to graphics API of Dawn.

#ifndef CONTEXTDAWN_H
#define CONTEXTDAWN_H

#ifdef DAWN_ENABLE_VULKAN_BACKEND
// The Vulkan header is included by VulkanBackend.h, so this should be placed
// before the GLFW header.
#include "dawn_native/VulkanBackend.h"
#endif
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "dawn/webgpu_cpp.h"
#include "dawn_native/DawnNative.h"
#include "utils/WGPUHelpers.h"

#include "../Context.h"
#include "BufferManagerDawn.h"

class TextureDawn;
class BufferDawn;
class ProgramDawn;
class RingBufferDawn;
class BufferManagerDawn;
enum BACKENDTYPE : short;

class ContextDawn : public Context {
public:
  ContextDawn(BACKENDTYPE backendType);
  ~ContextDawn();
  bool initialize(
      BACKENDTYPE backend,
      const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset,
      int windowWidth,
      int windowHeight) override;
  void setWindowTitle(const std::string &text) override;
  bool ShouldQuit() override;
  void KeyBoardQuit() override;
  void DoFlush(const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)>
                   &toggleBitset) override;
  void Flush() override;
  void Terminate() override;
  void showWindow() override;
  void updateFPS(const FPSTimer &fpsTimer,
                 int *fishCount,
                 std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)>
                     *toggleBitset) override;
  void showFPS() override;
  void destoryImgUI() override;

  void preFrame() override;

  Model *createModel(Aquarium *aquarium,
                     MODELGROUP type,
                     MODELNAME name,
                     bool blend) override;
  Buffer *createBuffer(int numComponents,
                       std::vector<float> *buffer,
                       bool isIndex) override;
  Buffer *createBuffer(int numComponents,
                       std::vector<unsigned short> *buffer,
                       bool isIndex) override;

  Program *createProgram(const std::string &mVId,
                         const std::string &mFId) override;

  Texture *createTexture(const std::string &name,
                         const std::string &url) override;
  Texture *createTexture(const std::string &name,
                         const std::vector<std::string> &urls) override;
  wgpu::Texture createTexture(const wgpu::TextureDescriptor &descriptor) const;
  wgpu::Sampler createSampler(const wgpu::SamplerDescriptor &descriptor) const;
  wgpu::Buffer createBufferFromData(const void *data,
                                    uint32_t size,
                                    uint32_t maxSize,
                                    wgpu::BufferUsage usage);
  wgpu::BufferCopyView createBufferCopyView(const wgpu::Buffer &buffer,
                                            uint32_t offset,
                                            uint32_t bytesPerRow,
                                            uint32_t rowsPerImage) const;
  wgpu::CommandBuffer copyBufferToTexture(
      const wgpu::BufferCopyView &bufferCopyView,
      const wgpu::TextureCopyView &textureCopyView,
      const wgpu::Extent3D &ext3D) const;
  wgpu::CommandBuffer copyBufferToBuffer(wgpu::Buffer const &srcBuffer,
                                         uint64_t srcOffset,
                                         wgpu::Buffer const &destBuffer,
                                         uint64_t destOffset,
                                         uint64_t size) const;

  wgpu::TextureCopyView createTextureCopyView(wgpu::Texture texture,
                                              uint32_t level,
                                              wgpu::Origin3D origin);
  wgpu::ShaderModule createShaderModule(utils::SingleShaderStage stage,
                                        const std::string &str) const;
  wgpu::BindGroupLayout MakeBindGroupLayout(
      std::initializer_list<wgpu::BindGroupLayoutEntry> bindingsInitializer)
      const;
  wgpu::PipelineLayout MakeBasicPipelineLayout(
      std::vector<wgpu::BindGroupLayout> bindingsInitializer) const;
  wgpu::RenderPipeline createRenderPipeline(
      wgpu::PipelineLayout mPipelineLayout,
      ProgramDawn *mProgramDawn,
      const wgpu::VertexStateDescriptor &mVertexInputDescriptor,
      bool enableBlend) const;
  wgpu::TextureView createMultisampledRenderTargetView() const;
  wgpu::TextureView createDepthStencilView() const;
  wgpu::Buffer createBuffer(uint32_t size, wgpu::BufferUsage bit) const;
  void setBufferData(const wgpu::Buffer &buffer,
                     uint32_t bufferSize,
                     const void *data,
                     uint32_t dataSize);
  wgpu::BindGroup makeBindGroup(
      const wgpu::BindGroupLayout &layout,
      std::initializer_list<utils::BindingInitializationHelper>
          bindingsInitializer) const;

  void initGeneralResources(Aquarium *aquarium) override;
  void updateWorldlUniforms(Aquarium *aquarium) override;
  const wgpu::Device &getDevice() const { return mDevice; }
  const wgpu::RenderPassEncoder &getRenderPass() const { return mRenderPass; }

  void reallocResource(int preTotalInstance,
                       int curTotalInstance,
                       bool enableDynamicBufferOffset) override;
  void updateAllFishData() override;
  void updateBufferData(const wgpu::Buffer &buffer,
                        size_t bufferSize,
                        void *data,
                        size_t dataSize) const;
  wgpu::CreateBufferMappedResult CreateBufferMapped(wgpu::BufferUsage usage,
                                                    uint64_t size) const;
  void WaitABit();
  wgpu::CommandEncoder createCommandEncoder() const;
  size_t CalcConstantBufferByteSize(size_t byteSize) const;

  std::vector<wgpu::CommandBuffer> mCommandBuffers;
  wgpu::Queue queue;

  wgpu::BindGroupLayout groupLayoutGeneral;
  wgpu::BindGroup bindGroupGeneral;
  wgpu::BindGroupLayout groupLayoutWorld;
  wgpu::BindGroup bindGroupWorld;

  wgpu::BindGroupLayout groupLayoutFishPer;
  wgpu::Buffer fishPersBuffer;
  wgpu::BindGroup *bindGroupFishPers;

  FishPer *fishPers;

  wgpu::Device mDevice;

private:
  bool GetHardwareAdapter(
      std::unique_ptr<dawn_native::Instance> &instance,
      dawn_native::Adapter *backendAdapter,
      wgpu::BackendType backendType,
      const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset);

  void initAvailableToggleBitset(BACKENDTYPE backendType) override;
  static void framebufferResizeCallback(GLFWwindow *window,
                                        int width,
                                        int height);
  void destoryFishResource();

  // TODO(jiawei.shao@intel.com): remove wgpu::TextureUsageBit::CopyDst when the
  // bug in Dawn is fixed.
  static constexpr wgpu::TextureUsage kSwapchainBackBufferUsage =
      wgpu::TextureUsage::OutputAttachment | wgpu::TextureUsage::CopyDst;

  bool mIsSwapchainOutOfDate = false;
  GLFWwindow *mWindow;
  std::unique_ptr<dawn_native::Instance> mInstance;

  wgpu::SwapChain mSwapchain;
  wgpu::CommandEncoder mCommandEncoder;
  wgpu::RenderPassEncoder mRenderPass;
  utils::ComboRenderPassDescriptor mRenderPassDescriptor;

  wgpu::TextureView mBackbufferView;
  wgpu::TextureView mSceneRenderTargetView;
  wgpu::TextureView mSceneDepthStencilView;
  wgpu::RenderPipeline mPipeline;
  wgpu::BindGroup mBindGroup;
  wgpu::TextureFormat mPreferredSwapChainFormat;

  wgpu::Buffer mLightWorldPositionBuffer;
  wgpu::Buffer mLightBuffer;
  wgpu::Buffer mFogBuffer;

  bool mEnableDynamicBufferOffset;

  BufferManagerDawn *bufferManager;
};

#endif  // CONTEXTDAWN_H
