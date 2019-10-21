//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "ContextD3D12.h"

#include <iostream>
#include <sstream>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "BufferD3D12.h"
#include "FishModelD3D12.h"
#include "FishModelInstancedDrawD3D12.h"
#include "GenericModelD3D12.h"
#include "InnerModelD3D12.h"
#include "OutsideModelD3D12.h"
#include "ProgramD3D12.h"
#include "SeaweedModelD3D12.h"
#include "TextureD3D12.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_glfw.h"

const CD3DX12_HEAP_PROPERTIES defaultheapProperties =
    CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
const CD3DX12_HEAP_PROPERTIES uploadheapProperties =
    CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        printf("D3D12 backend failed: file \"%s\", line %d\n", __FILE__, __LINE__);
    }
}

ContextD3D12::ContextD3D12(BACKENDTYPE backendType)
    : lightWorldPositionView({}),
      mWindow(nullptr),
      mDevice(nullptr),
      mCommandQueue(nullptr),
      mSwapChain(nullptr),
      mPreferredSwapChainFormat(DXGI_FORMAT_R8G8B8A8_UNORM),
      mCompileFlags(0),
      m_frameIndex(0),
      mRtvHeap(nullptr),
      mDsvHeap(nullptr),
      mCbvsrvHeap(nullptr),
      mRtvDescriptorSize(0),
      mCbvmSrvDescriptorSize(0),
      mFenceValue(0),
      mRootSignature({}),
      mViewport(0.0f, 0.0f, 0.0f, 0.0f),
      mScissorRect(0.0f, 0.0f, 0.0f, 0.0f),
      mLightView({}),
      mFogView({}),
      mSceneRenderTargetView({}),
      mEnableMSAA(false)
{
    for (UINT n = 0; n < mFrameCount; n++)
    {
        mBufferSerias[n] = 0;
    }

    mResourceHelper = new ResourceHelper("d3d12", "", backendType);
    initAvailableToggleBitset(backendType);
}

ContextD3D12::~ContextD3D12()
{
    delete mResourceHelper;
    destoryImgUI();
}

bool ContextD3D12::initialize(
    BACKENDTYPE backend,
    const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset)
{
    mEnableMSAA = toggleBitset.test(static_cast<size_t>(TOGGLE::ENABLEMSAAx4));

    // initialise GLFW
    if (!glfwInit())
    {
        std::cout << "Failed to initialise GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    // set full screen
    // glfwWindowHint(GLFW_DECORATED, GL_FALSE);

    GLFWmonitor *pMonitor   = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(pMonitor);
    mClientWidth            = mode->width;
    mClientHeight           = mode->height;

    if (toggleBitset.test(static_cast<size_t>(TOGGLE::ENABLEFULLSCREENMODE)))
    {
        mWindow = glfwCreateWindow(mClientWidth, mClientHeight, "Aquarium", pMonitor, nullptr);
    }
    else
    {
        mWindow = glfwCreateWindow(mClientWidth, mClientHeight, "Aquarium", nullptr, nullptr);
    }

    if (mWindow == nullptr)
    {
        std::cout << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        return false;
    }

    HWND hwnd = glfwGetWin32Window(mWindow);

    // Get the resolution of screen
    glfwGetFramebufferSize(mWindow, &mClientWidth, &mClientHeight);

    UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after mDevice creation will invalidate the active mDevice.
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }

        mCompileFlags =
            D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_STRICTNESS;
    }
#endif

    ComPtr<IDXGIFactory4> mFactory;
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&mFactory)));

    ComPtr<IDXGIAdapter1> hardwareAdapter;
    ThrowIfFailed(GetHardwareAdapter(mFactory.Get(), &hardwareAdapter, toggleBitset));

    ThrowIfFailed(
        D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDevice)));
    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount           = mFrameCount;
    swapChainDesc.Width                 = mClientWidth;
    swapChainDesc.Height                = mClientHeight;
    swapChainDesc.Format                = mPreferredSwapChainFormat;
    swapChainDesc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count      = 1;

    mViewport.Width     = static_cast<float>(mClientWidth);
    mViewport.Height    = static_cast<float>(mClientHeight);
    mScissorRect.right  = static_cast<LONG>(mClientWidth);
    mScissorRect.bottom = static_cast<LONG>(mClientHeight);

    ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(mFactory->CreateSwapChainForHwnd(
        mCommandQueue.Get(),  // Swap chain needs the queue so that it can force a flush on it.
        hwnd, &swapChainDesc, nullptr, nullptr, &swapChain));

    // This sample does not support fullscreen transitions.
    ThrowIfFailed(mFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));

    ThrowIfFailed(swapChain.As(&mSwapChain));

    m_frameIndex = mSwapChain->GetCurrentBackBufferIndex();

    // Describe and create a render target view (RTV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors             = mEnableMSAA ? mFrameCount + 1 : mFrameCount;
    rtvHeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap)));

    mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle;
    // Create frame resources.
    {
        rtvHandle = mRtvHeap->GetCPUDescriptorHandleForHeapStart();

        // Create a RTV for each frame.
        for (UINT n = 0; n < mFrameCount; n++)
        {
            ThrowIfFailed(mSwapChain->GetBuffer(n, IID_PPV_ARGS(&mRenderTargets[n])));
            mDevice->CreateRenderTargetView(mRenderTargets[n].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, mRtvDescriptorSize);
        }
    }

    // Describe and create a depth stencil view (DSV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors             = 1;
    dsvHeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(mDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&mDsvHeap)));

    // Init cbvuav descriptor heap
    // Create a shader resource view (SRV) and constant
    // buffer view (CBV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC cbvSrvHeapDesc = {};
    cbvSrvHeapDesc.NumDescriptors             = cbvsrvCount;
    cbvSrvHeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvSrvHeapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ThrowIfFailed(mDevice->CreateDescriptorHeap(&cbvSrvHeapDesc, IID_PPV_ARGS(&mCbvsrvHeap)));

    mCbvmSrvDescriptorSize =
        mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    cbvsrvCPUHandle = mCbvsrvHeap->GetCPUDescriptorHandleForHeapStart();
    cbvsrvGPUHandle = mCbvsrvHeap->GetGPUDescriptorHandleForHeapStart();

    // Init 3 command allocators for 3 back buffers
    for (UINT n = 0; n < mFrameCount; n++)
    {
        ThrowIfFailed(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                      IID_PPV_ARGS(&mCommandAllocators[n])));
    }

    createCommandList(nullptr, mCommandList);

    // Check highest version of root signature.
    mRootSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (FAILED(mDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &mRootSignature,
                                            sizeof(mRootSignature))))
    {
        mRootSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    if (mEnableMSAA)
    {
        D3D12_RESOURCE_DESC textureDesc = {};
        textureDesc.MipLevels           = 1;
        textureDesc.Format              = mPreferredSwapChainFormat;
        textureDesc.Width               = mClientWidth;
        textureDesc.Height              = mClientHeight;
        textureDesc.Flags               = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        textureDesc.DepthOrArraySize    = 1;
        textureDesc.SampleDesc.Count    = 4u;
        textureDesc.SampleDesc.Quality  = 0;
        textureDesc.Dimension           = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

        ThrowIfFailed(mDevice->CreateCommittedResource(
            &defaultheapProperties, D3D12_HEAP_FLAG_NONE, &textureDesc,
            D3D12_RESOURCE_STATE_RENDER_TARGET, nullptr, IID_PPV_ARGS(&mSceneRenderTargetTexture)));

        mSceneRenderTargetView.Format             = mPreferredSwapChainFormat;
        mSceneRenderTargetView.ViewDimension      = D3D12_RTV_DIMENSION_TEXTURE2DMS;
        mSceneRenderTargetView.Texture2D.MipSlice = 1;

        mDevice->CreateRenderTargetView(mSceneRenderTargetTexture.Get(), &mSceneRenderTargetView,
                                        rtvHandle);
    }
    createDepthStencilView();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    // We use glfw for d3d12 backend as well, but imgui only expose glfw to OpenGL and Vulkan.
    // However, the glfw api is totoally independent of Graphics API, so we could just ingnore the
    // name of the function.
    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    ImGui_ImplDX12_Init(mDevice.Get(), mFrameCount, mPreferredSwapChainFormat, cbvsrvCPUHandle,
                        cbvsrvGPUHandle);
    cbvsrvCPUHandle.Offset(mCbvmSrvDescriptorSize);
    cbvsrvGPUHandle.Offset(mCbvmSrvDescriptorSize);

    return true;
}

// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
// If no such adapter can be found, *ppAdapter will be set to nullptr.
bool ContextD3D12::GetHardwareAdapter(
    IDXGIFactory2 *pFactory,
    IDXGIAdapter1 **ppAdapter,
    const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset)
{
    ComPtr<IDXGIAdapter1> adapter;
    *ppAdapter = nullptr;

    bool enableIntegratedGpu = toggleBitset.test(static_cast<size_t>(TOGGLE::INTEGRATEDGPU));
    bool enableDiscreteGpu   = toggleBitset.test(static_cast<size_t>(TOGGLE::DISCRETEGPU));
    bool useDefaultGpu       = (enableDiscreteGpu | enableIntegratedGpu) == false ? true : false;

    for (UINT adapterIndex = 0;
         DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (useDefaultGpu ||
            (enableDiscreteGpu && (desc.VendorId == 0x10DE || desc.VendorId == 0x1002)) ||
            (enableIntegratedGpu && desc.VendorId == 0x8086))
        {
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                continue;
            }

            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
                                            _uuidof(ID3D12Device), nullptr)))
            {
                std::wstring str = desc.Description;
                std::string renderer = std::string(str.begin(), str.end());
                std::cout << renderer << std::endl;
                mResourceHelper->setRenderer(renderer);

                break;
            }
        }
    }

    *ppAdapter = adapter.Detach();
    if (ppAdapter == nullptr)
    {
        std::cerr << "Failed to create adapter." << std::endl;
        return false;
    }
    return true;
}

void ContextD3D12::setWindowTitle(const std::string &text)
{
    glfwSetWindowTitle(mWindow, text.c_str());
}

bool ContextD3D12::ShouldQuit()
{
    return glfwWindowShouldClose(mWindow);
}

void ContextD3D12::KeyBoardQuit()
{
    if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(mWindow, GL_TRUE);
}

void ContextD3D12::stateTransition(ComPtr<ID3D12Resource> &resource,
                                   D3D12_RESOURCE_STATES preState,
                                   D3D12_RESOURCE_STATES transferState) const
{
    CD3DX12_RESOURCE_BARRIER barrier =
        CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), preState, transferState);
    mCommandList->ResourceBarrier(1, &barrier);
}

void ContextD3D12::initAvailableToggleBitset(BACKENDTYPE backendType)
{
    mAvailableToggleBitset.set(static_cast<size_t>(TOGGLE::ENABLEMSAAx4));
    mAvailableToggleBitset.set(static_cast<size_t>(TOGGLE::ENABLEINSTANCEDDRAWS));
    mAvailableToggleBitset.set(static_cast<size_t>(TOGGLE::DISCRETEGPU));
    mAvailableToggleBitset.set(static_cast<size_t>(TOGGLE::INTEGRATEDGPU));
    mAvailableToggleBitset.set(static_cast<size_t>(TOGGLE::ENABLEFULLSCREENMODE));
}

void ContextD3D12::DoFlush(const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset)
{
    // Resolve MSAA texture to non MSAA texture, and then present.
    if (mEnableMSAA)
    {
        stateTransition(mSceneRenderTargetTexture, D3D12_RESOURCE_STATE_RENDER_TARGET,
                        D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
        stateTransition(mRenderTargets[m_frameIndex], D3D12_RESOURCE_STATE_COMMON,
                        D3D12_RESOURCE_STATE_RESOLVE_DEST);

        mCommandList->ResolveSubresource(mRenderTargets[m_frameIndex].Get(), 0,
                                         mSceneRenderTargetTexture.Get(), 0,
                                         mPreferredSwapChainFormat);

        stateTransition(mSceneRenderTargetTexture, D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
                        D3D12_RESOURCE_STATE_RENDER_TARGET);
        stateTransition(mRenderTargets[m_frameIndex], D3D12_RESOURCE_STATE_RESOLVE_DEST,
                        D3D12_RESOURCE_STATE_COMMON);
    }
    else
    {
        stateTransition(mRenderTargets[m_frameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET,
                        D3D12_RESOURCE_STATE_COMMON);
    }

    ThrowIfFailed(mCommandList->Close());

    // Execute the command list.
    ID3D12CommandList *ppCommandLists[] = {mCommandList.Get()};
    mCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Present the frame.
    ThrowIfFailed(mSwapChain->Present(1, 0));

    WaitForPreviousFrame();

    glfwPollEvents();
}

void ContextD3D12::Flush()
{
    // Close the command list and execute it to begin the initial GPU setup.
    ThrowIfFailed(mCommandList->Close());
    ID3D12CommandList *ppCommandLists[] = {mCommandList.Get()};
    mCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Create synchronization objects and wait until assets have been uploaded to the GPU.
    {
        ThrowIfFailed(
            mDevice->CreateFence(mFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));

        // Create an event handle to use for frame synchronization.
        mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (mFenceEvent == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }

        ThrowIfFailed(mSwapChain->Present(1, 0));
        m_frameIndex = mSwapChain->GetCurrentBackBufferIndex();

        WaitForPreviousFrame();
    }
}

void ContextD3D12::Terminate()
{
    UINT lastSerias = 0;
    if (m_frameIndex == 0)
    {
        lastSerias = mBufferSerias[2];
    }
    else
    {
        lastSerias = mBufferSerias[m_frameIndex - 1];
    }

    if (mFence->GetCompletedValue() < lastSerias)
    {
        ThrowIfFailed(mFence->SetEventOnCompletion(lastSerias, mFenceEvent));
        WaitForSingleObject(mFenceEvent, INFINITE);
    }

    glfwTerminate();
}

void ContextD3D12::showWindow()
{
    glfwShowWindow(mWindow);
}

void ContextD3D12::showFPS(const FPSTimer &fpsTimer,
                           int *fishCount,
                           std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> *toggleBitset)
{
    // Start the Dear ImGui frame
    ImGui_ImplDX12_NewFrame();
    renderImgui(fpsTimer, fishCount, toggleBitset);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mCommandList.Get());
}

void ContextD3D12::destoryImgUI()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ContextD3D12::preFrame()
{
    // Reuse the memory associated with command recording.
    // We can only reset when the associated command lists have finished execution on the GPU.
    ThrowIfFailed(mCommandAllocators[m_frameIndex]->Reset());

    // A command list can be reset after it has been added to the command queue via
    // ExecuteCommandList.
    // Reusing the command list reuses memory.
    ThrowIfFailed(mCommandList->Reset(mCommandAllocators[m_frameIndex].Get(), nullptr));

    // Set descriptor heaps related to command list.
    ID3D12DescriptorHeap *mDescriptorHeaps[] = {mCbvsrvHeap.Get()};

    mCommandList->SetDescriptorHeaps(_countof(mDescriptorHeaps), mDescriptorHeaps);
    mCommandList->RSSetViewports(1, &mViewport);
    mCommandList->RSSetScissorRects(1, &mScissorRect);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle;
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle;
    if (mEnableMSAA)
    {
        rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
                                                  mFrameCount, mRtvDescriptorSize);
    }
    else
    {
        rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
                                                  m_frameIndex, mRtvDescriptorSize);

        stateTransition(mRenderTargets[m_frameIndex], D3D12_RESOURCE_STATE_COMMON,
                        D3D12_RESOURCE_STATE_RENDER_TARGET);
    }

    dsvHandle = mDsvHeap->GetCPUDescriptorHandleForHeapStart();

    mCommandList->ClearDepthStencilView(mDsvHeap->GetCPUDescriptorHandleForHeapStart(),
                                        D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0,
                                        0, nullptr);

    mCommandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);
}

Model *ContextD3D12::createModel(Aquarium *aquarium, MODELGROUP type, MODELNAME name, bool blend)
{
    Model *model;
    switch (type)
    {
        case MODELGROUP::FISH:
            model = new FishModelD3D12(this, aquarium, type, name, blend);
            break;
        case MODELGROUP::FISHINSTANCEDDRAW:
            model = new FishModelInstancedDrawD3D12(this, aquarium, type, name, blend);
            break;
        case MODELGROUP::GENERIC:
            model = new GenericModelD3D12(this, aquarium, type, name, blend);
            break;
        case MODELGROUP::INNER:
            model = new InnerModelD3D12(this, aquarium, type, name, blend);
            break;
        case MODELGROUP::SEAWEED:
            model = new SeaweedModelD3D12(this, aquarium, type, name, blend);
            break;
        case MODELGROUP::OUTSIDE:
            model = new OutsideModelD3D12(this, aquarium, type, name, blend);
            break;
        default:
            model = nullptr;
            std::cerr << "can not create model type" << std::endl;
    }

    return model;
}

Buffer *ContextD3D12::createBuffer(int numComponents, std::vector<float> *buf, bool isIndex)
{
    Buffer *buffer =
        new BufferD3D12(this, static_cast<int>(buf->size()), numComponents, *buf, isIndex);
    return buffer;
}

Buffer *ContextD3D12::createBuffer(int numComponents,
                                   std::vector<unsigned short> *buf,
                                   bool isIndex)
{
    Buffer *buffer =
        new BufferD3D12(this, static_cast<int>(buf->size()), numComponents, *buf, isIndex);
    return buffer;
}

Program *ContextD3D12::createProgram(const std::string &mVId, const std::string &mFId)
{
    ProgramD3D12 *program = new ProgramD3D12(this, mVId, mFId);
    program->loadProgram();

    return program;
}

Texture *ContextD3D12::createTexture(const std::string &name, const std::string &url)
{
    Texture *texture = new TextureD3D12(this, name, url);
    texture->loadTexture();
    return texture;
}

Texture *ContextD3D12::createTexture(const std::string &name, const std::vector<std::string> &urls)
{
    Texture *texture = new TextureD3D12(this, name, urls);
    texture->loadTexture();
    return texture;
}

void ContextD3D12::initGeneralResources(Aquarium *aquarium)
{
    // create common constant buffer, desc and view.
    mLightBuffer =
        createDefaultBuffer(&aquarium->lightUniforms,
                            CalcConstantBufferByteSize(sizeof(LightUniforms)), mLightUploadBuffer);
    mLightView.BufferLocation = mLightBuffer->GetGPUVirtualAddress();
    mLightView.SizeInBytes    = CalcConstantBufferByteSize(sizeof(LightUniforms));
    buildCbvDescriptor(mLightView, &lightGPUHandle);

    mFogBuffer = createDefaultBuffer(
        &aquarium->fogUniforms, CalcConstantBufferByteSize(sizeof(FogUniforms)), mFogUploadBuffer);
    mFogView.BufferLocation = mFogBuffer->GetGPUVirtualAddress();
    mFogView.SizeInBytes    = CalcConstantBufferByteSize(
        sizeof(FogUniforms));  // CB size is required to be 256-byte aligned.
    buildCbvDescriptor(mFogView, &fogGPUHandle);

    TextureD3D12 *skybox = static_cast<TextureD3D12 *>(aquarium->getSkybox());
    skybox->createSrvDescriptor();

    rangeGeneral[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 2, 0, 0,
                         D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    rangeGeneral[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0,
                         D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    rootParameterGeneral.InitAsDescriptorTable(2, rangeGeneral, D3D12_SHADER_VISIBILITY_PIXEL);

    mLightWorldPositionBuffer =
        createUploadBuffer(&aquarium->lightWorldPositionUniform,
                           CalcConstantBufferByteSize(sizeof(LightWorldPositionUniform)));
    lightWorldPositionView.BufferLocation = mLightWorldPositionBuffer->GetGPUVirtualAddress();
    lightWorldPositionView.SizeInBytes =
        CalcConstantBufferByteSize(sizeof(LightWorldPositionUniform));

    rootParameterWorld.InitAsConstantBufferView(0, 1, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC,
                                                D3D12_SHADER_VISIBILITY_VERTEX);

    CD3DX12_STATIC_SAMPLER_DESC sampler2D(0u,                                     // shaderRegister
                                          D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT,  // filter
                                          D3D12_TEXTURE_ADDRESS_MODE_MIRROR,      // addressU
                                          D3D12_TEXTURE_ADDRESS_MODE_MIRROR,      // addressV
                                          D3D12_TEXTURE_ADDRESS_MODE_MIRROR, 0.0f, 0u,
                                          D3D12_COMPARISON_FUNC_ALWAYS,
                                          D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK, 0.0f,
                                          D3D12_FLOAT32_MAX, D3D12_SHADER_VISIBILITY_PIXEL, 0u);

    CD3DX12_STATIC_SAMPLER_DESC samplerCube(1u,  // shaderRegister
                                            D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT,  // filter
                                            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,       // addressU
                                            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,       // addressV
                                            D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 0.0f, 0u,
                                            D3D12_COMPARISON_FUNC_NEVER,
                                            D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK, 0.0f,
                                            D3D12_FLOAT32_MAX, D3D12_SHADER_VISIBILITY_PIXEL, 0u);
    staticSamplers.emplace_back(std::move(sampler2D));
    staticSamplers.emplace_back(std::move(samplerCube));
}

void ContextD3D12::updateWorldlUniforms(Aquarium *aquarium)
{
    CD3DX12_RANGE readRange(0, 0);
    UINT8 *m_pCbvDataBegin;
    mLightWorldPositionBuffer->Map(0, &readRange, reinterpret_cast<void **>(&m_pCbvDataBegin));
    memcpy(m_pCbvDataBegin, &aquarium->lightWorldPositionUniform,
           sizeof(LightWorldPositionUniform));
}

ComPtr<ID3DBlob> ContextD3D12::createShaderModule(const std::string &type,
                                                  const std::string &shaderStr)
{
    ComPtr<ID3DBlob> shader;
    ComPtr<ID3DBlob> errors;
    HRESULT hr;

    if (type == "VS")
    {
        hr = (D3DCompile(shaderStr.c_str(), shaderStr.length(), nullptr, nullptr, nullptr, "main",
                         "vs_5_1", mCompileFlags, 0, &shader, &errors));
    }
    else  // "FS"
    {
        hr = (D3DCompile(shaderStr.c_str(), shaderStr.length(), nullptr, nullptr, nullptr, "main",
                         "ps_5_1", mCompileFlags, 0, &shader, &errors));
    }

    if (FAILED(hr))
    {
        if (nullptr != errors)
        {
            std::cerr << ((char *)errors->GetBufferPointer());
            errors->Release();
        }
    }

    return shader;
}

void ContextD3D12::createCommittedResource(const D3D12_HEAP_PROPERTIES &properties,
                                           const D3D12_RESOURCE_DESC &desc,
                                           D3D12_RESOURCE_STATES state,
                                           ComPtr<ID3D12Resource> &resource)
{
    if (FAILED(mDevice->CreateCommittedResource(&properties, D3D12_HEAP_FLAG_NONE, &desc, state,
                                                nullptr, IID_PPV_ARGS(&resource))))
    {
        std::cout << "failed to create Resource" << std::endl;
    }
}

void ContextD3D12::updateSubresources(ID3D12GraphicsCommandList *pCmdList,
                                      ID3D12Resource *pDestinationResource,
                                      ID3D12Resource *pIntermediate,
                                      UINT64 IntermediateOffset,
                                      UINT FirstSubresource,
                                      UINT NumSubresources,
                                      D3D12_SUBRESOURCE_DATA *pSrcData)
{
    UpdateSubresources(pCmdList, pDestinationResource, pIntermediate, IntermediateOffset,
                       FirstSubresource, NumSubresources, pSrcData);
}

void ContextD3D12::executeCommandLists(UINT NumCommandLists,
                                       ID3D12CommandList *const *ppCommandLists)
{
    mCommandQueue->ExecuteCommandLists(NumCommandLists, ppCommandLists);
}

void ContextD3D12::WaitForPreviousFrame()
{
    mFenceValue++;
    // Signal and increment the fence value.
    const UINT64 fence = mFenceValue;
    ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), fence));
    mBufferSerias[m_frameIndex] = mFenceValue;

    // Aquarium uses 3 back buffers for better performance.
    // Wait until the previous before previous frame is finished.
    int prepreIndex   = (m_frameIndex + 1) % 3;
    UINT prepreSerias = mBufferSerias[prepreIndex];
    if (mFence->GetCompletedValue() < prepreSerias)
    {
        ThrowIfFailed(mFence->SetEventOnCompletion(fence, mFenceEvent));
        WaitForSingleObject(mFenceEvent, INFINITE);
    }

    // Get frame index for the next frame
    m_frameIndex = mSwapChain->GetCurrentBackBufferIndex();
}

void ContextD3D12::FlushPreviousFrames()
{
    mFenceValue++;
    // Signal and increment the fence value.
    const UINT64 fence = mFenceValue;
    ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), fence));
    mBufferSerias[m_frameIndex] = mFenceValue;

    if (mFence->GetCompletedValue() < mFenceValue)
    {
        ThrowIfFailed(mFence->SetEventOnCompletion(fence, mFenceEvent));
        WaitForSingleObject(mFenceEvent, INFINITE);
    }

    // Get frame index for the next frame
    m_frameIndex = mSwapChain->GetCurrentBackBufferIndex();
}

void ContextD3D12::reallocResource(int preTotalInstance,
                                   int curTotalInstance,
                                   bool enableDynamicBufferOffset)
{
}

void ContextD3D12::updateAllFishData(
    const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset)
{
}

void ContextD3D12::createDepthStencilView()
{
    D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
    depthStencilViewDesc.Format                        = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension =
        mEnableMSAA ? D3D12_DSV_DIMENSION_TEXTURE2DMS : D3D12_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

    D3D12_CLEAR_VALUE depthOptimizedClearValue    = {};
    depthOptimizedClearValue.Format               = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthOptimizedClearValue.DepthStencil.Depth   = 1.0f;
    depthOptimizedClearValue.DepthStencil.Stencil = 0;

    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment          = 0;
    depthStencilDesc.Width              = mClientWidth;
    depthStencilDesc.Height             = mClientHeight;
    depthStencilDesc.DepthOrArraySize   = 1;
    depthStencilDesc.MipLevels          = 1;
    depthStencilDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count   = mEnableMSAA ? 4 : 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags              = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    ThrowIfFailed(mDevice->CreateCommittedResource(
        &defaultheapProperties, D3D12_HEAP_FLAG_NONE, &depthStencilDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthOptimizedClearValue, IID_PPV_ARGS(&mDepthStencil)));

    mDevice->CreateDepthStencilView(mDepthStencil.Get(), &depthStencilViewDesc,
                                    mDsvHeap->GetCPUDescriptorHandleForHeapStart());
}

void ContextD3D12::createCommandList(ID3D12PipelineState *pInitialState,
                                     ComPtr<ID3D12GraphicsCommandList> &commandList)
{
    ThrowIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                             mCommandAllocators[0].Get(), pInitialState,
                                             IID_PPV_ARGS(&commandList)));
}

ComPtr<ID3D12Resource> ContextD3D12::createDefaultBuffer(const void *initData,
                                                         UINT64 byteSize,
                                                         ComPtr<ID3D12Resource> &uploadBuffer) const
{
    ComPtr<ID3D12Resource> defaultBuffer;

    CD3DX12_RESOURCE_DESC resourceDescriptor = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

    // Create the actual default buffer resource.
    ThrowIfFailed(mDevice->CreateCommittedResource(
        &defaultheapProperties, D3D12_HEAP_FLAG_NONE, &resourceDescriptor,
        D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

    // In order to copy CPU memory data into our default buffer, we need to create
    // an intermediate upload heap.
    ThrowIfFailed(mDevice->CreateCommittedResource(
        &uploadheapProperties, D3D12_HEAP_FLAG_NONE, &resourceDescriptor,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

    // Describe the data we want to copy into the default buffer.
    D3D12_SUBRESOURCE_DATA subResourceData = {};
    subResourceData.pData                  = initData;
    subResourceData.RowPitch               = byteSize;
    subResourceData.SlicePitch             = subResourceData.RowPitch;

    // Schedule to copy the data to the default buffer resource.
    stateTransition(defaultBuffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);

    UpdateSubresources<1>(mCommandList.Get(), defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1,
                          &subResourceData);

    stateTransition(defaultBuffer, D3D12_RESOURCE_STATE_COPY_DEST,
                    D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

    return defaultBuffer;
}

ComPtr<ID3D12Resource> ContextD3D12::createUploadBuffer(const void *initData, UINT64 byteSize) const
{
    // create an uploadBuffer for dynamic data.
    ComPtr<ID3D12Resource> uploadBuffer;
    CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
    mDevice->CreateCommittedResource(&uploadheapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc,
                                     D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                     IID_PPV_ARGS(uploadBuffer.GetAddressOf()));

    // Copy the triangle data to the vertex buffer.
    UINT8 *pVertexDataBegin;
    CD3DX12_RANGE readRange(0, 0);  // We do not intend to read from this resource on the CPU.
    uploadBuffer->Map(0, &readRange, reinterpret_cast<void **>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, initData, byteSize);
    uploadBuffer->Unmap(0, nullptr);
    return uploadBuffer;
}

void ContextD3D12::createRootSignature(
    const D3D12_VERSIONED_ROOT_SIGNATURE_DESC &pRootSignatureDesc,
    ComPtr<ID3D12RootSignature> &rootSignature) const
{
    ComPtr<ID3DBlob> signature = nullptr;
    ComPtr<ID3DBlob> error     = nullptr;
    ThrowIfFailed(
        D3DX12SerializeVersionedRootSignature(&pRootSignatureDesc, mRootSignature.HighestVersion,
                                              signature.GetAddressOf(), error.GetAddressOf()));
    if (error != nullptr)
    {
        ::OutputDebugStringA((char *)error->GetBufferPointer());
    }
    ThrowIfFailed(mDevice->CreateRootSignature(0, signature->GetBufferPointer(),
                                               signature->GetBufferSize(),
                                               IID_PPV_ARGS(&rootSignature)));
}

void ContextD3D12::createTexture(const D3D12_RESOURCE_DESC &textureDesc,
                                 const std::vector<UINT8 *> &texture,
                                 ComPtr<ID3D12Resource> &m_texture,
                                 ComPtr<ID3D12Resource> &textureUploadHeap,
                                 int TextureWidth,
                                 int TextureHeight,
                                 int TexturePixelSize,
                                 int mipLevels,
                                 int arraySize)
{
    ThrowIfFailed(mDevice->CreateCommittedResource(&defaultheapProperties, D3D12_HEAP_FLAG_NONE,
                                                   &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST,
                                                   nullptr, IID_PPV_ARGS(&m_texture)));

    int num2DSubresources = mipLevels * arraySize;
    const UINT64 uploadBufferSize =
        GetRequiredIntermediateSize(m_texture.Get(), 0, num2DSubresources);

    CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
    // Create the GPU upload buffer.
    ThrowIfFailed(mDevice->CreateCommittedResource(&uploadheapProperties, D3D12_HEAP_FLAG_NONE,
                                                   &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
                                                   nullptr, IID_PPV_ARGS(&textureUploadHeap)));

    // Max mipmap levels is 10.
    D3D12_SUBRESOURCE_DATA textureData[11];

    for (int i = 0; i < num2DSubresources; i++)
    {
        textureData[i].pData      = texture[i];
        textureData[i].RowPitch   = TextureWidth * 4;
        textureData[i].SlicePitch = textureData[i].RowPitch * TextureHeight;

        // Width and height of cubemap are the same.
        if (arraySize == 6)
        {
            continue;
        }

        TextureWidth >>= 1;
        TextureHeight >>= 1;
        if (TextureHeight == 0)
        {
            TextureHeight = 1;
        }
    }

    UpdateSubresources(mCommandList.Get(), m_texture.Get(), textureUploadHeap.Get(), 0, 0,
                       num2DSubresources, &textureData[0]);

    stateTransition(m_texture, D3D12_RESOURCE_STATE_COPY_DEST,
                    D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void ContextD3D12::createGraphicsPipelineState(
    const std::vector<D3D12_INPUT_ELEMENT_DESC> &mInputElementDescs,
    const ComPtr<ID3D12RootSignature> &rootSignature,
    const ComPtr<ID3DBlob> &mVertexShader,
    const ComPtr<ID3DBlob> &mPixelShader,
    ComPtr<ID3D12PipelineState> &mPipelineState,
    bool enableBlend) const
{
    // Describe and create the graphics mPipeline state object (PSO).
    D3D12_DEPTH_STENCILOP_DESC stencilDesc = {};
    stencilDesc.StencilFailOp              = D3D12_STENCIL_OP_KEEP;
    stencilDesc.StencilDepthFailOp         = D3D12_STENCIL_OP_KEEP;
    stencilDesc.StencilPassOp              = D3D12_STENCIL_OP_KEEP;
    stencilDesc.StencilFunc                = D3D12_COMPARISON_FUNC_ALWAYS;

    D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
    renderTargetBlendDesc.BlendEnable                    = enableBlend;
    renderTargetBlendDesc.LogicOpEnable                  = false;
    renderTargetBlendDesc.SrcBlend                       = D3D12_BLEND_SRC_ALPHA;
    renderTargetBlendDesc.DestBlend                      = D3D12_BLEND_INV_SRC_ALPHA;
    renderTargetBlendDesc.BlendOp                        = D3D12_BLEND_OP_ADD;
    renderTargetBlendDesc.SrcBlendAlpha                  = D3D12_BLEND_SRC_ALPHA;
    renderTargetBlendDesc.DestBlendAlpha                 = D3D12_BLEND_INV_SRC_ALPHA;
    renderTargetBlendDesc.BlendOpAlpha                   = D3D12_BLEND_OP_ADD;
    renderTargetBlendDesc.LogicOp                        = D3D12_LOGIC_OP_NOOP;
    renderTargetBlendDesc.RenderTargetWriteMask          = D3D12_COLOR_WRITE_ENABLE_ALL;

    D3D12_BLEND_DESC blendDesc       = {};
    blendDesc.AlphaToCoverageEnable  = false;
    blendDesc.IndependentBlendEnable = false;
    blendDesc.RenderTarget[0]        = renderTargetBlendDesc;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc    = {};
    psoDesc.InputLayout                           = {mInputElementDescs.data(),
                           static_cast<unsigned int>(mInputElementDescs.size())};
    psoDesc.pRootSignature                        = rootSignature.Get();
    psoDesc.VS                                    = CD3DX12_SHADER_BYTECODE(mVertexShader.Get());
    psoDesc.PS                                    = CD3DX12_SHADER_BYTECODE(mPixelShader.Get());
    psoDesc.RasterizerState.FillMode              = D3D12_FILL_MODE_SOLID;
    psoDesc.RasterizerState.CullMode              = D3D12_CULL_MODE_BACK;
    psoDesc.RasterizerState.FrontCounterClockwise = TRUE;  // oppose to opengl
    psoDesc.RasterizerState.DepthBias             = D3D12_DEFAULT_DEPTH_BIAS;
    psoDesc.RasterizerState.DepthBiasClamp        = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    psoDesc.RasterizerState.SlopeScaledDepthBias  = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    psoDesc.RasterizerState.DepthClipEnable       = TRUE;
    psoDesc.RasterizerState.MultisampleEnable     = mEnableMSAA;
    psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
    psoDesc.RasterizerState.ForcedSampleCount     = 0;
    psoDesc.RasterizerState.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    psoDesc.BlendState                            = blendDesc;
    psoDesc.DepthStencilState.DepthEnable         = true;
    psoDesc.DepthStencilState.DepthFunc           = D3D12_COMPARISON_FUNC_LESS;
    psoDesc.DepthStencilState.DepthWriteMask      = D3D12_DEPTH_WRITE_MASK_ALL;
    psoDesc.DepthStencilState.BackFace            = stencilDesc;
    psoDesc.DepthStencilState.FrontFace           = stencilDesc;
    psoDesc.DepthStencilState.StencilEnable       = true;
    psoDesc.DepthStencilState.StencilReadMask     = 0xff;
    psoDesc.DepthStencilState.StencilWriteMask    = 0xff;
    psoDesc.DSVFormat                             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    psoDesc.SampleMask                            = UINT_MAX;
    psoDesc.PrimitiveTopologyType                 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets                      = 1u;
    psoDesc.RTVFormats[0]                         = mPreferredSwapChainFormat;
    psoDesc.SampleDesc.Count                      = mEnableMSAA ? 4u : 1u;
    psoDesc.SampleDesc.Quality                    = 0;

    ThrowIfFailed(mDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPipelineState)));
}

void ContextD3D12::buildSrvDescriptor(const ComPtr<ID3D12Resource> &resource,
                                      const D3D12_SHADER_RESOURCE_VIEW_DESC &mSrvDesc,
                                      D3D12_GPU_DESCRIPTOR_HANDLE *hGpuDescriptor)
{
    mDevice->CreateShaderResourceView(resource.Get(), &mSrvDesc, cbvsrvCPUHandle);
    cbvsrvCPUHandle.Offset(mCbvmSrvDescriptorSize);

    *hGpuDescriptor = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvsrvGPUHandle);
    cbvsrvGPUHandle.Offset(mCbvmSrvDescriptorSize);
}

void ContextD3D12::buildCbvDescriptor(const D3D12_CONSTANT_BUFFER_VIEW_DESC &cbvDesc,
                                      D3D12_GPU_DESCRIPTOR_HANDLE *hGpuDescriptor)
{
    mDevice->CreateConstantBufferView(&cbvDesc, cbvsrvCPUHandle);
    cbvsrvCPUHandle.Offset(mCbvmSrvDescriptorSize);

    *hGpuDescriptor = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvsrvGPUHandle);
    cbvsrvGPUHandle.Offset(mCbvmSrvDescriptorSize);
}

UINT ContextD3D12::CalcConstantBufferByteSize(UINT byteSize)
{
    // Constant buffers must be a multiple of the minimum hardware
    // allocation size (usually 256 bytes).  So round up to nearest
    // multiple of 256.  We do this by adding 255 and then masking off
    // the lower 2 bytes which store all bits < 256.
    // Example: Suppose byteSize = 300.
    // (300 + 255) & ~255
    // 555 & ~255
    // 0x022B & ~0x00ff
    // 0x022B & 0xff00
    // 0x0200
    // 512
    return (byteSize + 255) & ~255;
}
