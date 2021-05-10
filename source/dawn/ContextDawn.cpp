//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// DeviceDawn.cpp: Implements accessing functions to the graphics API of Dawn.

#include "ContextDawn.h"

#include <array>
#include <iostream>
#include <string>
#include <vector>

#include "SPIRV/GlslangToSpv.h"
#include "build/build_config.h"
#include "dawn/dawn_proc.h"
#include "dawn/webgpu.h"
#include "dawn/webgpu_cpp.h"
#include "dawn_native/DawnNative.h"
#include "glslang/Public/ShaderLang.h"
#include "imgui_impl_glfw.h"
#if 0
#include "spirv-tools/libspirv.hpp"
#include "spirv-tools/optimizer.hpp"
#endif

#include "../Aquarium.h"
#include "../Assert.h"
#include "../FishModel.h"
#include "BufferDawn.h"
#include "FishModelDawn.h"
#include "FishModelInstancedDrawDawn.h"
#include "GenericModelDawn.h"
#include "InnerModelDawn.h"
#include "OutsideModelDawn.h"
#include "PlatformContextDawn.h"
#include "ProgramDawn.h"
#include "SeaweedModelDawn.h"
#include "TextureDawn.h"
#include "imgui_impl_dawn.h"

#if defined(OS_WIN)
#include <Windows.h>
#endif
#if defined(OS_MAC) || (defined(OS_LINUX) && !defined(OS_CHROMEOS))
#include <unistd.h>
#endif

ContextDawn::ContextDawn(BACKENDTYPE backendType)
    : queue(nullptr),
      groupLayoutGeneral(nullptr),
      bindGroupGeneral(nullptr),
      groupLayoutWorld(nullptr),
      bindGroupWorld(nullptr),
      groupLayoutFishPer(nullptr),
      fishPersBuffer(nullptr),
      bindGroupFishPers(nullptr),
      fishPers(nullptr),
      mDevice(nullptr),
      mWindow(nullptr),
      mInstance(),
      mSwapchain(nullptr),
      mCommandEncoder(nullptr),
      mRenderPass(nullptr),
      mRenderPassDescriptor({}),
      mSceneRenderTargetView(nullptr),
      mSceneDepthStencilView(nullptr),
      mPipeline(nullptr),
      mBindGroup(nullptr),
      mPreferredSwapChainFormat(wgpu::TextureFormat::RGBA8Unorm),
      bufferManager(nullptr) {
  mResourceHelper = new ResourceHelper("dawn", "", backendType);
  glslang::InitializeProcess();
  initAvailableToggleBitset(backendType);
}

ContextDawn::~ContextDawn() {
  glslang::FinalizeProcess();
  delete mResourceHelper;
  if (mWindow != nullptr && !mDisableControlPanel) {
    destoryImgUI();
  }

  mSceneRenderTargetView = nullptr;
  mSceneDepthStencilView = nullptr;
  mBackbufferView = nullptr;
  mPipeline = nullptr;
  mBindGroup = nullptr;
  mLightWorldPositionBuffer = nullptr;
  mLightBuffer = nullptr;
  mFogBuffer = nullptr;
  mCommandEncoder = nullptr;
  mCommandBuffers.clear();
  mRenderPass = nullptr;
  mRenderPassDescriptor = {};
  groupLayoutGeneral = nullptr;
  bindGroupGeneral = nullptr;
  groupLayoutWorld = nullptr;
  bindGroupWorld = nullptr;

  groupLayoutFishPer = nullptr;
  destoryFishResource();
  delete bufferManager;

  mSwapchain = nullptr;
  queue = nullptr;
  mDevice = nullptr;

  glfwTerminate();
}

ContextDawn *ContextDawn::create(BACKENDTYPE backendType) {
  if (backendType & BACKENDTYPE::BACKENDTYPED3D12) {
#ifdef DAWN_ENABLE_BACKEND_D3D12
    return new PlatformContextDawn(backendType);
#endif
  } else if (backendType & BACKENDTYPE::BACKENDTYPEMETAL) {
#ifdef DAWN_ENABLE_BACKEND_METAL
    return new PlatformContextDawn(backendType);
#endif
  } else if (backendType & BACKENDTYPE::BACKENDTYPEVULKAN) {
#ifdef DAWN_ENABLE_BACKEND_VULKAN
    return new PlatformContextDawn(backendType);
#endif
  }
  return nullptr;
}

bool ContextDawn::initialize(
    BACKENDTYPE backend,
    const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset,
    int windowWidth,
    int windowHeight) {
  wgpu::BackendType backendType = wgpu::BackendType::Null;

  if (backend & BACKENDTYPE::BACKENDTYPED3D12)
    backendType = wgpu::BackendType::D3D12;
  else if (backend & BACKENDTYPE::BACKENDTYPEMETAL)
    backendType = wgpu::BackendType::Metal;
  else if (backend & BACKENDTYPE::BACKENDTYPEOPENGL)
    backendType = wgpu::BackendType::OpenGL;
  else if (backend & BACKENDTYPE::BACKENDTYPEVULKAN)
    backendType = wgpu::BackendType::Vulkan;
  else {
    std::cerr << "Backend type can not reached." << std::endl;
    return false;
  }

  mDisableControlPanel =
      toggleBitset.test(static_cast<TOGGLE>(TOGGLE::DISABLECONTROLPANEL));

  // initialise GLFW
  if (!glfwInit()) {
    std::cout << "Failed to initialise GLFW" << std::endl;
    return false;
  }

  // Without this GLFW will initialize a GL context on the window, which
  // prevents using the window with other APIs (by crashing in weird ways).
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  // set full screen
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

  GLFWmonitor *pMonitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode = glfwGetVideoMode(pMonitor);
  mClientWidth = mode->width;
  mClientHeight = mode->height;

  setWindowSize(windowWidth, windowHeight);

  if (toggleBitset.test(static_cast<size_t>(TOGGLE::ENABLEFULLSCREENMODE))) {
    mWindow = glfwCreateWindow(mClientWidth, mClientHeight, "Aquarium",
                               pMonitor, nullptr);
  } else {
    mWindow = glfwCreateWindow(mClientWidth, mClientHeight, "Aquarium", nullptr,
                               nullptr);
  }

  if (mWindow == nullptr) {
    std::cout << "Failed to open GLFW window." << std::endl;
    glfwTerminate();
    return false;
  }

  // Get the resolution of screen
  glfwGetFramebufferSize(mWindow, &mClientWidth, &mClientHeight);

  mInstance = std::make_unique<dawn_native::Instance>();

  // Enable debug layer in Debug mode
#if defined(_DEBUG)
  mInstance->EnableBackendValidation(true);
#endif

  mInstance->DiscoverDefaultAdapters();

  dawn_native::Adapter backendAdapter;
  if (!GetHardwareAdapter(mInstance, &backendAdapter, backendType,
                          toggleBitset)) {
    return false;
  }

  WGPUDevice backendDevice;
  dawn_native::DeviceDescriptor descriptor;
  if (toggleBitset.test(static_cast<size_t>(TOGGLE::TURNOFFVSYNC))) {
    const char *turnOffVsync = "turn_off_vsync";
    descriptor.forceEnabledToggles.push_back(turnOffVsync);
  }
  if (toggleBitset.test(static_cast<size_t>(TOGGLE::DISABLED3D12RENDERPASS))) {
    const char *useD3D12RenderPass = "use_d3d12_render_pass";
    descriptor.forceDisabledToggles.push_back(useD3D12RenderPass);
  }
  if (toggleBitset.test(static_cast<size_t>(TOGGLE::DISABLEDAWNVALIDATION))) {
    const char *skipValidation = "skip_validation";
    descriptor.forceEnabledToggles.push_back(skipValidation);
  }
  backendDevice = backendAdapter.CreateDevice(&descriptor);

  DawnProcTable backendProcs = dawn_native::GetProcs();

  dawnProcSetProcs(&backendProcs);
  mDevice = wgpu::Device::Acquire(backendDevice);

  queue = mDevice.GetDefaultQueue();
  wgpu::SwapChainDescriptor swapChainDesc;
  swapChainDesc.implementation =
      reinterpret_cast<uintptr_t>(getSwapChainImplementation(backendType));

  mSwapchain = mDevice.CreateSwapChain(nullptr, &swapChainDesc);

  mPreferredSwapChainFormat = getPreferredSwapChainTextureFormat(backendType);
  mSwapchain.Configure(mPreferredSwapChainFormat, kSwapchainBackBufferUsage,
                       mClientWidth, mClientHeight);

  dawn_native::PCIInfo info = backendAdapter.GetPCIInfo();
  std::string renderer = info.name;
  std::cout << renderer << std::endl;
  mResourceHelper->setRenderer(renderer);

  // When MSAA is enabled, we create an intermediate multisampled texture to
  // render the scene to.
  if (mMSAASampleCount > 1) {
    mSceneRenderTargetView = createMultisampledRenderTargetView();
  }

  mSceneDepthStencilView = createDepthStencilView();

  // TODO(jiawei.shao@intel.com): support recreating swapchain when window is
  // resized on all backends
  if (backend & BACKENDTYPE::BACKENDTYPEVULKAN) {
    glfwSetFramebufferSizeCallback(mWindow, framebufferResizeCallback);
    glfwSetWindowUserPointer(mWindow, this);
  }

  if (!mDisableControlPanel) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    // We use glfw to create window for dawn backend as well.
    // Because imgui doesn't have dawn backend, we rewrite the functions by dawn
    // API in imgui_impl_dawn.cpp and imgui_impl_dawn.h
    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    ImGui_ImplDawn_Init(this, mPreferredSwapChainFormat);
  }
  bufferManager = new BufferManagerDawn(
      this,
      !toggleBitset.test(static_cast<TOGGLE>(TOGGLE::BUFFERMAPPINGASYNC)));

  return true;
}

void ContextDawn::framebufferResizeCallback(GLFWwindow *window,
                                            int width,
                                            int height) {
  ContextDawn *contextDawn =
      reinterpret_cast<ContextDawn *>(glfwGetWindowUserPointer(window));
  contextDawn->mIsSwapchainOutOfDate = true;
}

bool ContextDawn::GetHardwareAdapter(
    std::unique_ptr<dawn_native::Instance> &instance,
    dawn_native::Adapter *backendAdapter,
    wgpu::BackendType backendType,
    const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset) {
  bool enableIntegratedGpu =
      toggleBitset.test(static_cast<size_t>(TOGGLE::INTEGRATEDGPU));
  bool enableDiscreteGpu =
      toggleBitset.test(static_cast<size_t>(TOGGLE::DISCRETEGPU));
  bool useDefaultGpu =
      (enableDiscreteGpu | enableIntegratedGpu) == false ? true : false;
  bool result = false;

  // Get an adapter for the backend to use, and create the Device.
  for (auto &adapter : instance->GetAdapters()) {
    wgpu::AdapterProperties properties;
    adapter.GetProperties(&properties);
    if (properties.backendType == backendType) {
      if (useDefaultGpu ||
          (enableDiscreteGpu &&
           adapter.GetDeviceType() == dawn_native::DeviceType::DiscreteGPU) ||
          (enableIntegratedGpu &&
           adapter.GetDeviceType() == dawn_native::DeviceType::IntegratedGPU)) {
        *backendAdapter = adapter;
        result = true;
        break;
      }
    }
  }

  if (!result) {
    std::cerr << "Failed to create adapter." << std::endl;
    return false;
  }

  return true;
}

void ContextDawn::initAvailableToggleBitset(BACKENDTYPE backendType) {
  mAvailableToggleBitset.set(static_cast<size_t>(TOGGLE::ENABLEINSTANCEDDRAWS));
  mAvailableToggleBitset.set(
      static_cast<size_t>(TOGGLE::ENABLEDYNAMICBUFFEROFFSET));
  mAvailableToggleBitset.set(static_cast<size_t>(TOGGLE::DISCRETEGPU));
  mAvailableToggleBitset.set(static_cast<size_t>(TOGGLE::INTEGRATEDGPU));
  mAvailableToggleBitset.set(static_cast<size_t>(TOGGLE::ENABLEFULLSCREENMODE));
  mAvailableToggleBitset.set(static_cast<size_t>(TOGGLE::BUFFERMAPPINGASYNC));
  mAvailableToggleBitset.set(static_cast<size_t>(TOGGLE::TURNOFFVSYNC));
  mAvailableToggleBitset.set(
      static_cast<size_t>(TOGGLE::DISABLED3D12RENDERPASS));
  mAvailableToggleBitset.set(
      static_cast<size_t>(TOGGLE::DISABLEDAWNVALIDATION));
  mAvailableToggleBitset.set(
      static_cast<size_t>(TOGGLE::SIMULATINGFISHCOMEANDGO));
  mAvailableToggleBitset.set(static_cast<size_t>(TOGGLE::DRAWPERMODEL));
}

Texture *ContextDawn::createTexture(const std::string &name,
                                    const std::string &url) {
  Texture *texture = new TextureDawn(this, name, url);
  texture->loadTexture();
  return texture;
}

Texture *ContextDawn::createTexture(const std::string &name,
                                    const std::vector<std::string> &urls) {
  Texture *texture = new TextureDawn(this, name, urls);
  texture->loadTexture();
  return texture;
}

wgpu::Texture ContextDawn::createTexture(
    const wgpu::TextureDescriptor &descriptor) const {
  return mDevice.CreateTexture(&descriptor);
}

wgpu::Sampler ContextDawn::createSampler(
    const wgpu::SamplerDescriptor &descriptor) const {
  return mDevice.CreateSampler(&descriptor);
}

wgpu::Buffer ContextDawn::createBufferFromData(const void *data,
                                               uint32_t size,
                                               uint32_t maxSize,
                                               wgpu::BufferUsage usage) {
  wgpu::BufferDescriptor descriptor;
  descriptor.usage = usage | wgpu::BufferUsage::CopyDst;
  descriptor.size = maxSize;
  descriptor.mappedAtCreation = false;
  wgpu::Buffer buffer = createBuffer(descriptor);

  setBufferData(buffer, maxSize, data, size);
  return buffer;
}

wgpu::BufferCopyView ContextDawn::createBufferCopyView(
    const wgpu::Buffer &buffer,
    uint32_t offset,
    uint32_t bytesPerRow,
    uint32_t rowsPerImage) const {
  wgpu::BufferCopyView bufferCopyView;
  bufferCopyView.layout.offset = offset;
  bufferCopyView.layout.bytesPerRow = bytesPerRow;
  bufferCopyView.layout.rowsPerImage = rowsPerImage;
  bufferCopyView.buffer = buffer;
  bufferCopyView.bytesPerRow = 0;  // This field is deprecated, but it must be
                                   // zero-initialized to pass Dawn validation.

  return bufferCopyView;
}

wgpu::TextureCopyView ContextDawn::createTextureCopyView(
    wgpu::Texture texture,
    uint32_t level,
    wgpu::Origin3D origin) {
  wgpu::TextureCopyView textureCopyView;
  textureCopyView.texture = texture;
  textureCopyView.mipLevel = level;
  textureCopyView.origin = origin;

  return textureCopyView;
}

wgpu::CommandBuffer ContextDawn::copyBufferToTexture(
    const wgpu::BufferCopyView &bufferCopyView,
    const wgpu::TextureCopyView &textureCopyView,
    const wgpu::Extent3D &ext3D) const {
  wgpu::CommandEncoder encoder = mDevice.CreateCommandEncoder();
  encoder.CopyBufferToTexture(&bufferCopyView, &textureCopyView, &ext3D);
  wgpu::CommandBuffer copy = encoder.Finish();
  return copy;
}

wgpu::CommandBuffer ContextDawn::copyBufferToBuffer(
    wgpu::Buffer const &srcBuffer,
    uint64_t srcOffset,
    wgpu::Buffer const &destBuffer,
    uint64_t destOffset,
    uint64_t size) const {
  wgpu::CommandEncoder encoder = mDevice.CreateCommandEncoder();
  encoder.CopyBufferToBuffer(srcBuffer, srcOffset, destBuffer, destOffset,
                             size);
  wgpu::CommandBuffer copy = encoder.Finish();

  return copy;
}

wgpu::ShaderModule ContextDawn::createShaderModule(
    wgpu::ShaderStage stage,
    const std::string &str) const {
  EShLanguage language;
  switch (stage) {
  case wgpu::ShaderStage::Vertex:
    language = EShLanguage::EShLangVertex;
    break;
  case wgpu::ShaderStage::Fragment:
    language = EShLanguage::EShLangFragment;
    break;
  default:
    ASSERT(false);
  }

  glslang::TShader shader(language);
  {
    const char *s = str.c_str();
    const int len = static_cast<int>(str.length());
    const TBuiltInResource resources = {
        // Copied from //third_party/glslang/StandAlone/ResourceLimits.cpp

        /* .MaxLights = */ 32,
        /* .MaxClipPlanes = */ 6,
        /* .MaxTextureUnits = */ 32,
        /* .MaxTextureCoords = */ 32,
        /* .MaxVertexAttribs = */ 64,
        /* .MaxVertexUniformComponents = */ 4096,
        /* .MaxVaryingFloats = */ 64,
        /* .MaxVertexTextureImageUnits = */ 32,
        /* .MaxCombinedTextureImageUnits = */ 80,
        /* .MaxTextureImageUnits = */ 32,
        /* .MaxFragmentUniformComponents = */ 4096,
        /* .MaxDrawBuffers = */ 32,
        /* .MaxVertexUniformVectors = */ 128,
        /* .MaxVaryingVectors = */ 8,
        /* .MaxFragmentUniformVectors = */ 16,
        /* .MaxVertexOutputVectors = */ 16,
        /* .MaxFragmentInputVectors = */ 15,
        /* .MinProgramTexelOffset = */ -8,
        /* .MaxProgramTexelOffset = */ 7,
        /* .MaxClipDistances = */ 8,
        /* .MaxComputeWorkGroupCountX = */ 65535,
        /* .MaxComputeWorkGroupCountY = */ 65535,
        /* .MaxComputeWorkGroupCountZ = */ 65535,
        /* .MaxComputeWorkGroupSizeX = */ 1024,
        /* .MaxComputeWorkGroupSizeY = */ 1024,
        /* .MaxComputeWorkGroupSizeZ = */ 64,
        /* .MaxComputeUniformComponents = */ 1024,
        /* .MaxComputeTextureImageUnits = */ 16,
        /* .MaxComputeImageUniforms = */ 8,
        /* .MaxComputeAtomicCounters = */ 8,
        /* .MaxComputeAtomicCounterBuffers = */ 1,
        /* .MaxVaryingComponents = */ 60,
        /* .MaxVertexOutputComponents = */ 64,
        /* .MaxGeometryInputComponents = */ 64,
        /* .MaxGeometryOutputComponents = */ 128,
        /* .MaxFragmentInputComponents = */ 128,
        /* .MaxImageUnits = */ 8,
        /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
        /* .MaxCombinedShaderOutputResources = */ 8,
        /* .MaxImageSamples = */ 0,
        /* .MaxVertexImageUniforms = */ 0,
        /* .MaxTessControlImageUniforms = */ 0,
        /* .MaxTessEvaluationImageUniforms = */ 0,
        /* .MaxGeometryImageUniforms = */ 0,
        /* .MaxFragmentImageUniforms = */ 8,
        /* .MaxCombinedImageUniforms = */ 8,
        /* .MaxGeometryTextureImageUnits = */ 16,
        /* .MaxGeometryOutputVertices = */ 256,
        /* .MaxGeometryTotalOutputComponents = */ 1024,
        /* .MaxGeometryUniformComponents = */ 1024,
        /* .MaxGeometryVaryingComponents = */ 64,
        /* .MaxTessControlInputComponents = */ 128,
        /* .MaxTessControlOutputComponents = */ 128,
        /* .MaxTessControlTextureImageUnits = */ 16,
        /* .MaxTessControlUniformComponents = */ 1024,
        /* .MaxTessControlTotalOutputComponents = */ 4096,
        /* .MaxTessEvaluationInputComponents = */ 128,
        /* .MaxTessEvaluationOutputComponents = */ 128,
        /* .MaxTessEvaluationTextureImageUnits = */ 16,
        /* .MaxTessEvaluationUniformComponents = */ 1024,
        /* .MaxTessPatchComponents = */ 120,
        /* .MaxPatchVertices = */ 32,
        /* .MaxTessGenLevel = */ 64,
        /* .MaxViewports = */ 16,
        /* .MaxVertexAtomicCounters = */ 0,
        /* .MaxTessControlAtomicCounters = */ 0,
        /* .MaxTessEvaluationAtomicCounters = */ 0,
        /* .MaxGeometryAtomicCounters = */ 0,
        /* .MaxFragmentAtomicCounters = */ 8,
        /* .MaxCombinedAtomicCounters = */ 8,
        /* .MaxAtomicCounterBindings = */ 1,
        /* .MaxVertexAtomicCounterBuffers = */ 0,
        /* .MaxTessControlAtomicCounterBuffers = */ 0,
        /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
        /* .MaxGeometryAtomicCounterBuffers = */ 0,
        /* .MaxFragmentAtomicCounterBuffers = */ 1,
        /* .MaxCombinedAtomicCounterBuffers = */ 1,
        /* .MaxAtomicCounterBufferSize = */ 16384,
        /* .MaxTransformFeedbackBuffers = */ 4,
        /* .MaxTransformFeedbackInterleavedComponents = */ 64,
        /* .MaxCullDistances = */ 8,
        /* .MaxCombinedClipAndCullDistances = */ 8,
        /* .MaxSamples = */ 4,
        /* .maxMeshOutputVerticesNV = */ 256,
        /* .maxMeshOutputPrimitivesNV = */ 512,
        /* .maxMeshWorkGroupSizeX_NV = */ 32,
        /* .maxMeshWorkGroupSizeY_NV = */ 1,
        /* .maxMeshWorkGroupSizeZ_NV = */ 1,
        /* .maxTaskWorkGroupSizeX_NV = */ 32,
        /* .maxTaskWorkGroupSizeY_NV = */ 1,
        /* .maxTaskWorkGroupSizeZ_NV = */ 1,
        /* .maxMeshViewCountNV = */ 4,
        /* .maxDualSourceDrawBuffersEXT = */ 1,
        /* .limits = */
        {
            /* .nonInductiveForLoops = */ 1,
            /* .whileLoops = */ 1,
            /* .doWhileLoops = */ 1,
            /* .generalUniformIndexing = */ 1,
            /* .generalAttributeMatrixVectorIndexing = */ 1,
            /* .generalVaryingIndexing = */ 1,
            /* .generalSamplerIndexing = */ 1,
            /* .generalVariableIndexing = */ 1,
            /* .generalConstantMatrixVectorIndexing = */ 1,
        },
    };
    shader.setStringsWithLengths(&s, &len, 1);
    shader.setEntryPoint("main");
    shader.setEnvInput(glslang::EShSource::EShSourceGlsl, language,
                       glslang::EShClient::EShClientVulkan, 100);
    shader.setEnvClient(glslang::EShClient::EShClientVulkan,
                        glslang::EShTargetClientVersion::EShTargetVulkan_1_0);
    shader.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv,
                        glslang::EShTargetLanguageVersion::EShTargetSpv_1_0);
    if (!shader.parse(&resources, 450, EProfile::ECoreProfile, false, false,
                      EShMessages::EShMsgDefault)) {
      std::cerr << shader.getInfoLog();
      return {};
    }
  }

  glslang::TProgram program;
  program.addShader(&shader);
  if (!program.link(EShMessages::EShMsgDefault)) {
    std::cerr << program.getInfoLog();
    return {};
  }

  std::vector<uint32_t> code;
  {
    glslang::SpvOptions options;
    glslang::GlslangToSpv(*program.getIntermediate(language), code, &options);
  }
#if 0
  {
    spvtools::Optimizer optimizer(spv_target_env::SPV_ENV_VULKAN_1_0);
    spvtools::OptimizerOptions options;
    optimizer.RegisterPerformancePasses();
    options.set_run_validator(false);
    optimizer.Run(code.data(), code.size(), &code, options);
  }
#endif

  wgpu::ShaderModuleSPIRVDescriptor spirvDescriptor;
  spirvDescriptor.codeSize = static_cast<uint32_t>(code.size());
  spirvDescriptor.code = code.data();

  wgpu::ShaderModuleDescriptor descriptor;
  descriptor.nextInChain = &spirvDescriptor;

  return mDevice.CreateShaderModule(&descriptor);
}

wgpu::BindGroupLayout ContextDawn::MakeBindGroupLayout(
    std::initializer_list<wgpu::BindGroupLayoutEntry> bindingsInitializer)
    const {
  std::vector<wgpu::BindGroupLayoutEntry> bindings = bindingsInitializer;

  wgpu::BindGroupLayoutDescriptor descriptor;
  descriptor.entryCount = static_cast<uint32_t>(bindings.size());
  descriptor.entries = bindings.data();

  return mDevice.CreateBindGroupLayout(&descriptor);
}

wgpu::PipelineLayout ContextDawn::MakeBasicPipelineLayout(
    std::vector<wgpu::BindGroupLayout> bindingsInitializer) const {
  wgpu::PipelineLayoutDescriptor descriptor;

  descriptor.bindGroupLayoutCount =
      static_cast<uint32_t>(bindingsInitializer.size());
  descriptor.bindGroupLayouts = bindingsInitializer.data();

  return mDevice.CreatePipelineLayout(&descriptor);
}

wgpu::RenderPipeline ContextDawn::createRenderPipeline(
    wgpu::PipelineLayout mPipelineLayout,
    ProgramDawn *mProgramDawn,
    const wgpu::VertexStateDescriptor &mVertexStateDescriptor,
    bool enableBlend) const {
  const wgpu::ShaderModule &mVsModule = mProgramDawn->getVSModule();
  const wgpu::ShaderModule &mFsModule = mProgramDawn->getFSModule();

  wgpu::ProgrammableStageDescriptor vertexStageDescriptor;
  vertexStageDescriptor.module = mVsModule;
  vertexStageDescriptor.entryPoint = "main";

  wgpu::ProgrammableStageDescriptor fragmentStageDescriptor;
  fragmentStageDescriptor.module = mFsModule;
  fragmentStageDescriptor.entryPoint = "main";

  wgpu::StencilStateFaceDescriptor stencilStateFaceDescriptor;
  stencilStateFaceDescriptor.compare = wgpu::CompareFunction::Always;
  stencilStateFaceDescriptor.failOp = wgpu::StencilOperation::Keep;
  stencilStateFaceDescriptor.depthFailOp = wgpu::StencilOperation::Keep;
  stencilStateFaceDescriptor.passOp = wgpu::StencilOperation::Keep;

  wgpu::DepthStencilStateDescriptor depthStencilStateDescriptor;
  depthStencilStateDescriptor.format = wgpu::TextureFormat::Depth24PlusStencil8;
  depthStencilStateDescriptor.depthWriteEnabled = true;
  depthStencilStateDescriptor.depthCompare = wgpu::CompareFunction::Less;
  depthStencilStateDescriptor.stencilFront = stencilStateFaceDescriptor;
  depthStencilStateDescriptor.stencilBack = stencilStateFaceDescriptor;
  depthStencilStateDescriptor.stencilReadMask = 0xff;
  depthStencilStateDescriptor.stencilWriteMask = 0xff;

  wgpu::BlendDescriptor blendDescriptor;
  blendDescriptor.operation = wgpu::BlendOperation::Add;
  if (enableBlend) {
    blendDescriptor.srcFactor = wgpu::BlendFactor::SrcAlpha;
    blendDescriptor.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
  } else {
    blendDescriptor.srcFactor = wgpu::BlendFactor::One;
    blendDescriptor.dstFactor = wgpu::BlendFactor::Zero;
  }

  wgpu::ColorStateDescriptor ColorStateDescriptor;
  ColorStateDescriptor.format = mPreferredSwapChainFormat;
  ColorStateDescriptor.colorBlend = blendDescriptor;
  ColorStateDescriptor.alphaBlend = blendDescriptor;
  ColorStateDescriptor.writeMask = wgpu::ColorWriteMask::All;

  wgpu::RasterizationStateDescriptor rasterizationState;
  rasterizationState.nextInChain = nullptr;
  rasterizationState.frontFace = wgpu::FrontFace::CCW;
  rasterizationState.cullMode = wgpu::CullMode::Back;
  rasterizationState.depthBias = 0;
  rasterizationState.depthBiasSlopeScale = 0.0;
  rasterizationState.depthBiasClamp = 0.0;

  // test
  wgpu::RenderPipelineDescriptor descriptor;
  descriptor.layout = mPipelineLayout;
  descriptor.vertexStage = vertexStageDescriptor;
  descriptor.fragmentStage = &fragmentStageDescriptor;
  descriptor.vertexState = &mVertexStateDescriptor;
  descriptor.depthStencilState = &depthStencilStateDescriptor;
  descriptor.colorStateCount = 1;
  descriptor.colorStates = &ColorStateDescriptor;
  descriptor.primitiveTopology = wgpu::PrimitiveTopology::TriangleList;
  descriptor.sampleCount = mMSAASampleCount;
  descriptor.rasterizationState = &rasterizationState;

  wgpu::RenderPipeline mPipeline = mDevice.CreateRenderPipeline(&descriptor);

  return mPipeline;
}

wgpu::TextureView ContextDawn::createMultisampledRenderTargetView() const {
  wgpu::TextureDescriptor descriptor;
  descriptor.dimension = wgpu::TextureDimension::e2D;
  descriptor.size.width = mClientWidth;
  descriptor.size.height = mClientHeight;
  descriptor.size.depth = 1;
  descriptor.sampleCount = mMSAASampleCount;
  descriptor.format = mPreferredSwapChainFormat;
  descriptor.mipLevelCount = 1;
  descriptor.usage = wgpu::TextureUsage::OutputAttachment;

  return mDevice.CreateTexture(&descriptor).CreateView();
}

wgpu::TextureView ContextDawn::createDepthStencilView() const {
  wgpu::TextureDescriptor descriptor;
  descriptor.dimension = wgpu::TextureDimension::e2D;
  descriptor.size.width = mClientWidth;
  descriptor.size.height = mClientHeight;
  descriptor.size.depth = 1;
  descriptor.sampleCount = mMSAASampleCount;
  descriptor.format = wgpu::TextureFormat::Depth24PlusStencil8;
  descriptor.mipLevelCount = 1;
  descriptor.usage = wgpu::TextureUsage::OutputAttachment;
  auto depthStencilTexture = mDevice.CreateTexture(&descriptor);
  return depthStencilTexture.CreateView();
}

wgpu::Buffer ContextDawn::createBuffer(
    const wgpu::BufferDescriptor &descriptor) const {
  return mDevice.CreateBuffer(&descriptor);
}

void ContextDawn::setBufferData(const wgpu::Buffer &buffer,
                                uint32_t bufferSize,
                                const void *data,
                                uint32_t dataSize) {
  wgpu::BufferDescriptor descriptor;
  descriptor.usage = wgpu::BufferUsage::MapWrite | wgpu::BufferUsage::CopySrc;
  descriptor.size = bufferSize;
  descriptor.mappedAtCreation = true;
  wgpu::Buffer staging = createBuffer(descriptor);
  memcpy(staging.GetMappedRange(), data, dataSize);
  staging.Unmap();

  wgpu::CommandBuffer command =
      copyBufferToBuffer(staging, 0, buffer, 0, bufferSize);
  mCommandBuffers.emplace_back(command);
}

wgpu::BindGroup ContextDawn::makeBindGroup(
    const wgpu::BindGroupLayout &layout,
    std::initializer_list<wgpu::BindGroupEntry> bindingsInitializer) const {
  std::vector<wgpu::BindGroupEntry> bindings = bindingsInitializer;

  wgpu::BindGroupDescriptor descriptor;
  descriptor.layout = layout;
  descriptor.entryCount = static_cast<uint32_t>(bindings.size());
  descriptor.entries = bindings.data();

  return mDevice.CreateBindGroup(&descriptor);
}

void ContextDawn::initGeneralResources(Aquarium *aquarium) {
  // initilize general uniform buffers
  groupLayoutGeneral = MakeBindGroupLayout({
      {0, wgpu::ShaderStage::Fragment, wgpu::BindingType::UniformBuffer},
      {1, wgpu::ShaderStage::Fragment, wgpu::BindingType::UniformBuffer},
  });

  mLightBuffer = createBufferFromData(
      &aquarium->lightUniforms, sizeof(aquarium->lightUniforms),
      sizeof(aquarium->lightUniforms),
      wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);
  mFogBuffer = createBufferFromData(
      &aquarium->fogUniforms, sizeof(aquarium->fogUniforms),
      sizeof(aquarium->fogUniforms),
      wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);

  bindGroupGeneral = makeBindGroup(
      groupLayoutGeneral,
      {{0, mLightBuffer, 0, sizeof(aquarium->lightUniforms), {}, {}},
       {1, mFogBuffer, 0, sizeof(aquarium->fogUniforms), {}, {}}});

  setBufferData(mLightBuffer, sizeof(LightUniforms), &aquarium->lightUniforms,
                sizeof(LightUniforms));
  setBufferData(mFogBuffer, sizeof(FogUniforms), &aquarium->fogUniforms,
                sizeof(FogUniforms));

  // initilize world uniform buffers
  groupLayoutWorld = MakeBindGroupLayout({
      {0, wgpu::ShaderStage::Vertex, wgpu::BindingType::UniformBuffer},
  });

  mLightWorldPositionBuffer = createBufferFromData(
      &aquarium->lightWorldPositionUniform,
      sizeof(aquarium->lightWorldPositionUniform),
      CalcConstantBufferByteSize(sizeof(aquarium->lightWorldPositionUniform)),
      wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);

  bindGroupWorld = makeBindGroup(
      groupLayoutWorld, {
                            {0,
                             mLightWorldPositionBuffer,
                             0,
                             CalcConstantBufferByteSize(
                                 sizeof(aquarium->lightWorldPositionUniform)),
                             {},
                             {}},
                        });

  bool enableDynamicBufferOffset = aquarium->toggleBitset.test(
      static_cast<size_t>(TOGGLE::ENABLEDYNAMICBUFFEROFFSET));
  if (enableDynamicBufferOffset) {
    groupLayoutFishPer = MakeBindGroupLayout({
        {0, wgpu::ShaderStage::Vertex, wgpu::BindingType::UniformBuffer, true},
    });
  } else {
    groupLayoutFishPer = MakeBindGroupLayout({
        {0, wgpu::ShaderStage::Vertex, wgpu::BindingType::UniformBuffer},
    });
  }

  reallocResource(aquarium->getPreFishCount(), aquarium->getCurFishCount(),
                  enableDynamicBufferOffset);
}

void ContextDawn::updateWorldlUniforms(Aquarium *aquarium) {
  updateBufferData(
      mLightWorldPositionBuffer,
      CalcConstantBufferByteSize(sizeof(LightWorldPositionUniform)),
      &aquarium->lightWorldPositionUniform, sizeof(LightWorldPositionUniform));
}

Buffer *ContextDawn::createBuffer(int numComponents,
                                  std::vector<float> *buf,
                                  bool isIndex) {
  Buffer *buffer = new BufferDawn(this, static_cast<int>(buf->size()),
                                  numComponents, buf, isIndex);
  return buffer;
}

Buffer *ContextDawn::createBuffer(int numComponents,
                                  std::vector<unsigned short> *buf,
                                  bool isIndex) {
  Buffer *buffer = new BufferDawn(this, static_cast<int>(buf->size()),
                                  numComponents, buf, isIndex);
  return buffer;
}

Program *ContextDawn::createProgram(const std::string &mVId,
                                    const std::string &mFId) {
  ProgramDawn *program = new ProgramDawn(this, mVId, mFId);

  return program;
}

void ContextDawn::setWindowTitle(const std::string &text) {
  glfwSetWindowTitle(mWindow, text.c_str());
}

bool ContextDawn::ShouldQuit() {
  return glfwWindowShouldClose(mWindow);
}

void ContextDawn::KeyBoardQuit() {
  if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
}

// Submit commands of the frame
void ContextDawn::DoFlush(
    const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset) {
  mRenderPass.EndPass();

  bufferManager->flush();

  wgpu::CommandBuffer cmd = mCommandEncoder.Finish();
  mCommandBuffers.emplace_back(cmd);

  Flush();

  mSwapchain.Present();

  glfwPollEvents();
}

void ContextDawn::Flush() {
  queue.Submit(mCommandBuffers.size(), mCommandBuffers.data());
  mCommandBuffers.clear();
}

void ContextDawn::Terminate() {
}

void ContextDawn::showWindow() {
  glfwShowWindow(mWindow);
}

void ContextDawn::updateFPS(
    const FPSTimer &fpsTimer,
    int *fishCount,
    std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> *toggleBitset) {
  if (mDisableControlPanel) {
    return;
  }

  // Start the Dear ImGui frame
  ImGui_ImplDawn_NewFrame(
      mMSAASampleCount,
      toggleBitset->test(static_cast<TOGGLE>(TOGGLE::ENABLEALPHABLENDING)));
  renderImgui(fpsTimer, fishCount, toggleBitset);
  ImGui_ImplDawn_RenderDrawData(ImGui::GetDrawData());
}

void ContextDawn::showFPS() {
  if (mDisableControlPanel) {
    return;
  }

  ImGui_ImplDawn_Draw(ImGui::GetDrawData());
}

void ContextDawn::destoryImgUI() {
  ImGui_ImplDawn_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void ContextDawn::preFrame() {
  if (mIsSwapchainOutOfDate) {
    glfwGetFramebufferSize(mWindow, &mClientWidth, &mClientHeight);
    if (mMSAASampleCount > 1) {
      mSceneRenderTargetView = createMultisampledRenderTargetView();
    }
    mSceneDepthStencilView = createDepthStencilView();
    mSwapchain.Configure(mPreferredSwapChainFormat, kSwapchainBackBufferUsage,
                         mClientWidth, mClientHeight);

    mIsSwapchainOutOfDate = false;
  }

  mCommandEncoder = mDevice.CreateCommandEncoder();
  mBackbufferView = mSwapchain.GetCurrentTextureView();

  wgpu::RenderPassColorAttachmentDescriptor colorAttachmentDescriptor;
  if (mMSAASampleCount > 1) {
    // If MSAA is enabled, we render to a multisampled texture and then resolve
    // to the backbuffer
    colorAttachmentDescriptor.attachment = mSceneRenderTargetView;
    colorAttachmentDescriptor.resolveTarget = mBackbufferView;
    colorAttachmentDescriptor.loadOp = wgpu::LoadOp::Clear;
    colorAttachmentDescriptor.storeOp = wgpu::StoreOp::Clear;
    colorAttachmentDescriptor.clearColor = {0.f, 0.8f, 1.f, 0.f};
  } else {
    // When MSAA is off, we render directly to the backbuffer
    colorAttachmentDescriptor.attachment = mBackbufferView;
    colorAttachmentDescriptor.loadOp = wgpu::LoadOp::Clear;
    colorAttachmentDescriptor.storeOp = wgpu::StoreOp::Store;
    colorAttachmentDescriptor.clearColor = {0.f, 0.8f, 1.f, 0.f};
  }

  wgpu::RenderPassDepthStencilAttachmentDescriptor
      depthStencilAttachmentDescriptor;
  depthStencilAttachmentDescriptor.attachment = mSceneDepthStencilView;
  depthStencilAttachmentDescriptor.depthLoadOp = wgpu::LoadOp::Clear;
  depthStencilAttachmentDescriptor.depthStoreOp = wgpu::StoreOp::Store;
  depthStencilAttachmentDescriptor.clearDepth = 1.f;
  depthStencilAttachmentDescriptor.stencilLoadOp = wgpu::LoadOp::Clear;
  depthStencilAttachmentDescriptor.stencilStoreOp = wgpu::StoreOp::Store;
  depthStencilAttachmentDescriptor.clearStencil = 0;

  mRenderPassDescriptor.colorAttachmentCount = 1;
  mRenderPassDescriptor.colorAttachments = &colorAttachmentDescriptor;
  mRenderPassDescriptor.depthStencilAttachment =
      &depthStencilAttachmentDescriptor;

  mRenderPass = mCommandEncoder.BeginRenderPass(&mRenderPassDescriptor);
}

Model *ContextDawn::createModel(Aquarium *aquarium,
                                MODELGROUP type,
                                MODELNAME name,
                                bool blend) {
  Model *model;
  switch (type) {
  case MODELGROUP::FISH:
    model = new FishModelDawn(this, aquarium, type, name, blend);
    break;
  case MODELGROUP::FISHINSTANCEDDRAW:
    model = new FishModelInstancedDrawDawn(this, aquarium, type, name, blend);
    break;
  case MODELGROUP::GENERIC:
    model = new GenericModelDawn(this, aquarium, type, name, blend);
    break;
  case MODELGROUP::INNER:
    model = new InnerModelDawn(this, aquarium, type, name, blend);
    break;
  case MODELGROUP::SEAWEED:
    model = new SeaweedModelDawn(this, aquarium, type, name, blend);
    break;
  case MODELGROUP::OUTSIDE:
    model = new OutsideModelDawn(this, aquarium, type, name, blend);
    break;
  default:
    model = nullptr;
    std::cout << "can not create model type" << std::endl;
  }

  return model;
}

void ContextDawn::reallocResource(int preTotalInstance,
                                  int curTotalInstance,
                                  bool enableDynamicBufferOffset) {
  mPreTotalInstance = preTotalInstance;
  mCurTotalInstance = curTotalInstance;
  mEnableDynamicBufferOffset = enableDynamicBufferOffset;

  if (curTotalInstance == 0)
    return;

  // If current fish number > pre fish number, allocate a new bigger buffer.
  // If current fish number <= prefish number, do not allocate a new one.
  // TODO(yizhou) : optimize the buffer allocation strategy.
  if (preTotalInstance >= curTotalInstance) {
    return;
  }

  destoryFishResource();

  fishPers = new FishPer[curTotalInstance];

  if (enableDynamicBufferOffset) {
    bindGroupFishPers = new wgpu::BindGroup[1];
  } else {
    bindGroupFishPers = new wgpu::BindGroup[curTotalInstance];
  }

  wgpu::BufferDescriptor descriptor;
  descriptor.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
  descriptor.size =
      CalcConstantBufferByteSize(sizeof(FishPer) * curTotalInstance);
  descriptor.mappedAtCreation = false;
  fishPersBuffer = createBuffer(descriptor);

  if (enableDynamicBufferOffset) {
    bindGroupFishPers[0] = makeBindGroup(
        groupLayoutFishPer, {{0,
                              fishPersBuffer,
                              0,
                              CalcConstantBufferByteSize(sizeof(FishPer)),
                              {},
                              {}}});
  } else {
    for (int i = 0; i < curTotalInstance; i++) {
      bindGroupFishPers[i] = makeBindGroup(
          groupLayoutFishPer, {{0,
                                fishPersBuffer,
                                CalcConstantBufferByteSize(sizeof(FishPer) * i),
                                CalcConstantBufferByteSize(sizeof(FishPer)),
                                {},
                                {}}});
    }
  }
}

void ContextDawn::WaitABit() {
  mDevice.Tick();

#if defined(OS_WIN)
  Sleep(0);
#elif defined(OS_MAC) || (defined(OS_LINUX) && !defined(OS_CHROMEOS))
  usleep(100);
#else
  ASSERT(false);
#endif
}

wgpu::CommandEncoder ContextDawn::createCommandEncoder() const {
  return mDevice.CreateCommandEncoder();
}

void ContextDawn::updateAllFishData() {
  size_t size = CalcConstantBufferByteSize(sizeof(FishPer) * mCurTotalInstance);
  updateBufferData(fishPersBuffer, size, fishPers,
                   sizeof(FishPer) * mCurTotalInstance);
}

void ContextDawn::updateBufferData(const wgpu::Buffer &buffer,
                                   size_t bufferSize,
                                   void *data,
                                   size_t dataSize) const {
  size_t offset = 0;
  RingBufferDawn *ringBuffer = bufferManager->allocate(bufferSize, &offset);

  if (ringBuffer == nullptr) {
    std::cout << "Memory upper limit." << std::endl;
    return;
  }

  ringBuffer->push(bufferManager->mEncoder, buffer, offset, 0, data, dataSize);
}

void ContextDawn::destoryFishResource() {
  fishPersBuffer = nullptr;

  if (fishPers != nullptr) {
    delete[] fishPers;
    fishPers = nullptr;
  }
  if (mEnableDynamicBufferOffset) {
    if (bindGroupFishPers != nullptr) {
      if (bindGroupFishPers[0].Get() != nullptr) {
        bindGroupFishPers[0] = nullptr;
      }
    }
  } else {
    if (bindGroupFishPers != nullptr) {
      for (int i = 0; i < mPreTotalInstance; i++) {
        if (bindGroupFishPers[i].Get() != nullptr) {
          bindGroupFishPers[i] = nullptr;
        }
      }
    }
  }

  bindGroupFishPers = nullptr;

  bufferManager->destroyBufferPool();
}

size_t ContextDawn::CalcConstantBufferByteSize(size_t byteSize) const {
  return (byteSize + 255) & ~255;
}
