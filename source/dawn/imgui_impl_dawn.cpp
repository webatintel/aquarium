// dear imgui: Renderer for Dawn
// This needs to be used along with a Platform Binding (e.g. GLFW)

#include <vector>

#include "ProgramDawn.h"

#include "imgui.h"
#include "imgui_impl_dawn.h"

// Dawn data
wgpu::RenderPipeline mPipeline(nullptr);
wgpu::BindGroup mBindGroup(nullptr);
wgpu::TextureFormat mFormat(wgpu::TextureFormat::RGBA8Unorm);
wgpu::ShaderModule mVsModule(nullptr);
wgpu::ShaderModule mFsModule(nullptr);

wgpu::Buffer mIndexBuffer(nullptr);
wgpu::Buffer mVertexBuffer(nullptr);
wgpu::Buffer mConstantBuffer(nullptr);
wgpu::Buffer mStagingBuffer(nullptr);
wgpu::Texture mTexture(nullptr);
wgpu::TextureView mTextureView;
wgpu::Sampler mSampler(nullptr);

ProgramDawn *mProgramDawn(nullptr);
ContextDawn *mContextDawn(nullptr);

int mIndexBufferSize  = 0;
int mVertexBufferSize = 0;

ImDrawVert mVertexData[40000];
ImDrawIdx mIndexData[10000];

struct VERTEX_CONSTANT_BUFFER
{
    float mvp[4][4];
};

static void ImGui_ImplDawn_SetupRenderState(ImDrawData *draw_data,
                                            const wgpu::RenderPassEncoder &pass)
{
    // Setup orthographic projection matrix into our constant buffer
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to
    // draw_data->DisplayPos+data_data->DisplaySize (bottom right).
    VERTEX_CONSTANT_BUFFER vertex_constant_buffer;
    {
        float L         = draw_data->DisplayPos.x;
        float R         = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
        float T         = draw_data->DisplayPos.y;
        float B         = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
        float mvp[4][4] = {
            {2.0f / (R - L), 0.0f, 0.0f, 0.0f},
            {0.0f, 2.0f / (T - B), 0.0f, 0.0f},
            {0.0f, 0.0f, 0.5f, 0.0f},
            {(R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f},
        };
        memcpy(&vertex_constant_buffer.mvp, mvp, sizeof(mvp));
    }
    mContextDawn->setBufferData(mConstantBuffer, sizeof(VERTEX_CONSTANT_BUFFER),
                                &vertex_constant_buffer.mvp, sizeof(VERTEX_CONSTANT_BUFFER));

    // TODO(yizhou): setting viewport isn't supported in dawn yet.
    // Setup viewport
    // pass.SetViewport(0.0f, 0.0f, draw_data->DisplaySize.x * draw_data->FramebufferScale.x,
    //                  draw_data->DisplaySize.y * draw_data->FramebufferScale.y, 0.0f, 1.0f);

    pass.SetPipeline(mPipeline);
    pass.SetBindGroup(0, mBindGroup, 0, nullptr);
    pass.SetVertexBuffer(0, mVertexBuffer);
    pass.SetIndexBuffer(mIndexBuffer, wgpu::IndexFormat::Uint16, 0, 0);
}

// Render function
// (this used to be set in io.RenderDrawListsFn and called by ImGui::Render(), but you can now call
// this directly from your main loop)
void ImGui_ImplDawn_RenderDrawData(ImDrawData *draw_data)
{
    // Avoid rendering when minimized
    if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
        return;

    // Create and grow vertex/index buffers if needed
    if (mVertexBuffer.Get() == nullptr || mVertexBufferSize < draw_data->TotalVtxCount)
    {
        mVertexBufferSize = draw_data->TotalVtxCount + 5000;
        mVertexBufferSize = mVertexBufferSize % 4 == 0
                                ? mVertexBufferSize
                                : mVertexBufferSize + 4 - mVertexBufferSize % 4;

        wgpu::BufferDescriptor descriptor;
        descriptor.size  = mVertexBufferSize * sizeof(ImDrawVert);
        descriptor.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;

        mVertexBuffer = mContextDawn->mDevice.CreateBuffer(&descriptor);
    }

    if (mIndexBuffer.Get() == nullptr || mIndexBufferSize < draw_data->TotalIdxCount)
    {
        mIndexBufferSize = draw_data->TotalIdxCount + 10000;
        mIndexBufferSize = mIndexBufferSize % 4 == 0 ? mIndexBufferSize
                                                     : mIndexBufferSize + 4 - mIndexBufferSize % 4;

        wgpu::BufferDescriptor descriptor;
        descriptor.size  = mIndexBufferSize * sizeof(ImDrawIdx);
        descriptor.usage = wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst;

        mIndexBuffer = mContextDawn->mDevice.CreateBuffer(&descriptor);
    }

    // Upload vertex/index data into a single contiguous GPU buffer
    uint32_t vtx_dst    = 0;
    uint32_t idx_dst    = 0;
    ImDrawVert *pVertex = mVertexData;
    ImDrawIdx *pIndex   = mIndexData;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList *cmd_list = draw_data->CmdLists[n];
        memcpy(pVertex, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(pIndex, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));

        pVertex += cmd_list->VtxBuffer.Size;
        pIndex += cmd_list->IdxBuffer.Size;
        vtx_dst += cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
        idx_dst += cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
    }
    vtx_dst = vtx_dst % 4 == 0 ? vtx_dst : vtx_dst + 4 - vtx_dst % 4;
    idx_dst = idx_dst % 4 == 0 ? idx_dst : idx_dst + 4 - idx_dst % 4;

    if (vtx_dst != 0 && idx_dst != 0)
    {
        mContextDawn->setBufferData(mVertexBuffer, vtx_dst, mVertexData,
                                    (pVertex - mVertexData) * sizeof(ImDrawVert));
        mContextDawn->setBufferData(mIndexBuffer, idx_dst, mIndexData,
                                    (pIndex - mIndexData) * sizeof(ImDrawIdx));
    }
}

void ImGui_ImplDawn_Draw(ImDrawData *draw_data)
{
    const wgpu::RenderPassEncoder &pass = mContextDawn->getRenderPass();

    // Setup desired Dawn state
    ImGui_ImplDawn_SetupRenderState(draw_data, pass);

    // Render pass
    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    int global_vtx_offset = 0;
    int global_idx_offset = 0;
    ImVec2 clip_off       = draw_data->DisplayPos;
    ImVec2 clip_scale     = draw_data->FramebufferScale;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList *cmd_list = draw_data->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != NULL)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to
                // request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplDawn_SetupRenderState(draw_data, pass);
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                // Apply Scissor, Bind texture, Draw
                ImVec4 clip_rect;
                clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
                clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
                clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
                clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;
                pass.SetScissorRect(static_cast<uint32_t>(clip_rect.x),
                                    static_cast<uint32_t>(clip_rect.y),
                                    static_cast<uint32_t>(clip_rect.z),
                                    static_cast<uint32_t>(clip_rect.w));
                pass.DrawIndexed(pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset,
                                 pcmd->VtxOffset + global_vtx_offset, 0);
            }
        }
        global_idx_offset += cmd_list->IdxBuffer.Size;
        global_vtx_offset += cmd_list->VtxBuffer.Size;
    }
}

static void ImGui_ImplDawn_CreateFontsTexture(bool enableAlphaBlending)
{
    // Build texture atlas
    ImGuiIO &io = ImGui::GetIO();
    unsigned char *pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, enableAlphaBlending);

    // Upload texture to graphics system
    {
        wgpu::TextureDescriptor descriptor;
        descriptor.dimension     = wgpu::TextureDimension::e2D;
        descriptor.size.width    = width;
        descriptor.size.height   = height;
        descriptor.size.depthOrArrayLayers    = 1;
        descriptor.sampleCount   = 1;
        descriptor.format        = mFormat;
        descriptor.mipLevelCount = 1;
        descriptor.usage         = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::Sampled;
        mTexture                 = mContextDawn->createTexture(descriptor);

        wgpu::BufferDescriptor bufferDescriptor;
        bufferDescriptor.usage = wgpu::BufferUsage::CopySrc | wgpu::BufferUsage::MapWrite;
        bufferDescriptor.size = width * height * 4;
        bufferDescriptor.mappedAtCreation = true;
        wgpu::Buffer staging = mContextDawn->createBuffer(bufferDescriptor);
        memcpy(staging.GetMappedRange(), pixels, width * height * 4);
        staging.Unmap();

        wgpu::ImageCopyBuffer imageCopyBuffer =
            mContextDawn->createImageCopyBuffer(staging, 0, width * 4, height);
        wgpu::ImageCopyTexture imageCopyTexture =
            mContextDawn->createImageCopyTexture(mTexture, 0, {0, 0, 0});
        wgpu::Extent3D copySize = {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1};
        wgpu::CommandBuffer copyCommand =
            mContextDawn->copyBufferToTexture(imageCopyBuffer, imageCopyTexture, copySize);
        mContextDawn->queue.Submit(1, &copyCommand);

        // Create texture view
        wgpu::TextureViewDescriptor viewDescriptor;
        viewDescriptor.nextInChain     = nullptr;
        viewDescriptor.dimension       = wgpu::TextureViewDimension::e2D;
        viewDescriptor.format          = mFormat;
        viewDescriptor.baseMipLevel    = 0;
        viewDescriptor.mipLevelCount   = 1;
        viewDescriptor.baseArrayLayer  = 0;
        viewDescriptor.arrayLayerCount = 1;

        mTextureView = mTexture.CreateView(&viewDescriptor);

        wgpu::SamplerDescriptor samplerDesc = {};
        samplerDesc.addressModeU = wgpu::AddressMode::Repeat;
        samplerDesc.addressModeV = wgpu::AddressMode::Repeat;
        samplerDesc.addressModeW = wgpu::AddressMode::Repeat;
        samplerDesc.minFilter    = wgpu::FilterMode::Linear;
        samplerDesc.magFilter    = wgpu::FilterMode::Linear;
        samplerDesc.mipmapFilter = wgpu::FilterMode::Linear;

        mSampler = mContextDawn->createSampler(samplerDesc);
    }

    io.Fonts->TexID = (ImTextureID)mTextureView.Get();
}

bool ImGui_ImplDawn_CreateDeviceObjects(int MSAASampleCount, bool enableAlphaBlending)
{
    if (!mContextDawn->mDevice)
        return false;

    std::vector<wgpu::VertexAttribute> vertexAttribute;
    vertexAttribute.resize(3);
    vertexAttribute[0].format         = wgpu::VertexFormat::Float32x2;
    vertexAttribute[0].offset         = 0;
    vertexAttribute[0].shaderLocation = 0;
    vertexAttribute[1].format         = wgpu::VertexFormat::Float32x2;
    vertexAttribute[1].offset         = IM_OFFSETOF(ImDrawVert, uv);
    vertexAttribute[1].shaderLocation = 1;
    vertexAttribute[2].format         = wgpu::VertexFormat::Unorm8x4;
    vertexAttribute[2].offset         = IM_OFFSETOF(ImDrawVert, col);
    vertexAttribute[2].shaderLocation = 2;

    std::vector<wgpu::VertexBufferLayout> vertexBufferLayout;
    vertexBufferLayout.resize(1);
    vertexBufferLayout[0].arrayStride    = sizeof(ImDrawVert);
    vertexBufferLayout[0].stepMode       = wgpu::InputStepMode::Vertex;
    vertexBufferLayout[0].attributeCount = 3;
    vertexBufferLayout[0].attributes     = &vertexAttribute[0];

    // Create bind group layout
    std::vector<wgpu::BindGroupLayoutEntry> bindGroupLayoutEntry;
    bindGroupLayoutEntry.resize(3);
    bindGroupLayoutEntry[0].binding                 = 0;
    bindGroupLayoutEntry[0].visibility              = wgpu::ShaderStage::Vertex;
    bindGroupLayoutEntry[0].buffer.type             = wgpu::BufferBindingType::Uniform;
    bindGroupLayoutEntry[0].buffer.hasDynamicOffset = false;
    bindGroupLayoutEntry[0].buffer.minBindingSize   = 0;
    bindGroupLayoutEntry[1].binding                 = 1;
    bindGroupLayoutEntry[1].visibility              = wgpu::ShaderStage::Fragment;
    bindGroupLayoutEntry[1].sampler.type            = wgpu::SamplerBindingType::Filtering;
    bindGroupLayoutEntry[2].binding                 = 2;
    bindGroupLayoutEntry[2].visibility              = wgpu::ShaderStage::Fragment;
    bindGroupLayoutEntry[2].texture.sampleType      = wgpu::TextureSampleType::Float;
    bindGroupLayoutEntry[2].texture.viewDimension   = wgpu::TextureViewDimension::e2D;
    bindGroupLayoutEntry[2].texture.multisampled    = false;
    wgpu::BindGroupLayout layout = mContextDawn->MakeBindGroupLayout(bindGroupLayoutEntry);

    wgpu::PipelineLayout mPipelineLayout = mContextDawn->MakeBasicPipelineLayout({layout});

    ResourceHelper *resourceHelper = mContextDawn->getResourceHelper();
    std::string programPath        = resourceHelper->getProgramPath();
    mProgramDawn = new ProgramDawn(mContextDawn, programPath + "imguiVertexShader",
                                   programPath + "imguiFragmentShader");
    mProgramDawn->compileProgram(false, "");

    const wgpu::ShaderModule &mVsModule = mProgramDawn->getVSModule();
    const wgpu::ShaderModule &mFsModule = mProgramDawn->getFSModule();

    wgpu::VertexState vertexState;
    vertexState.module      = mVsModule;
    vertexState.entryPoint  = "main";
    vertexState.bufferCount = static_cast<uint32_t>(vertexBufferLayout.size());
    vertexState.buffers     = vertexBufferLayout.data();

    wgpu::PrimitiveState primitiveState;
    primitiveState.topology         = wgpu::PrimitiveTopology::TriangleList;
    primitiveState.stripIndexFormat = wgpu::IndexFormat::Undefined;
    primitiveState.frontFace        = wgpu::FrontFace::CCW;
    primitiveState.cullMode         = wgpu::CullMode::None;

    wgpu::StencilFaceState stencilFaceState;
    stencilFaceState.compare     = wgpu::CompareFunction::Always;
    stencilFaceState.failOp      = wgpu::StencilOperation::Keep;
    stencilFaceState.depthFailOp = wgpu::StencilOperation::Keep;
    stencilFaceState.passOp      = wgpu::StencilOperation::Keep;

    wgpu::DepthStencilState depthStencilState;
    depthStencilState.format              = wgpu::TextureFormat::Depth24PlusStencil8;
    depthStencilState.depthWriteEnabled   = false;
    depthStencilState.depthCompare        = wgpu::CompareFunction::Always;
    depthStencilState.stencilFront        = stencilFaceState;
    depthStencilState.stencilBack         = stencilFaceState;
    depthStencilState.stencilReadMask     = 0xffffffff;
    depthStencilState.stencilWriteMask    = 0xffffffff;
    depthStencilState.depthBias           = 0;
    depthStencilState.depthBiasSlopeScale = 0.0;
    depthStencilState.depthBiasClamp      = 0.0;

    wgpu::MultisampleState multisampleState;
    multisampleState.count                  = MSAASampleCount;
    multisampleState.mask                   = 0xffffffff;
    multisampleState.alphaToCoverageEnabled = false;

    wgpu::BlendComponent colorBlendComponent;
    wgpu::BlendComponent alphaBlendComponent;
    if (enableAlphaBlending)
    {
        colorBlendComponent.operation = wgpu::BlendOperation::Add;
        colorBlendComponent.srcFactor = wgpu::BlendFactor::SrcAlpha;
        colorBlendComponent.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;

        alphaBlendComponent.operation = wgpu::BlendOperation::Add;
        alphaBlendComponent.srcFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
        alphaBlendComponent.dstFactor = wgpu::BlendFactor::Zero;
    } else
    {
        colorBlendComponent.operation = wgpu::BlendOperation::Add;
        colorBlendComponent.srcFactor = wgpu::BlendFactor::One;
        colorBlendComponent.dstFactor = wgpu::BlendFactor::Zero;

        alphaBlendComponent.operation = wgpu::BlendOperation::Add;
        alphaBlendComponent.srcFactor = wgpu::BlendFactor::One;
        alphaBlendComponent.dstFactor = wgpu::BlendFactor::Zero;
    }

    wgpu::BlendState blendState;
    blendState.color = colorBlendComponent;
    blendState.alpha = alphaBlendComponent;

    wgpu::ColorTargetState colorTargetState;
    colorTargetState.format    = mFormat;
    colorTargetState.blend     = &blendState;
    colorTargetState.writeMask = wgpu::ColorWriteMask::All;

    wgpu::FragmentState fragmentState;
    fragmentState.module      = mFsModule;
    fragmentState.entryPoint  = "main";
    fragmentState.targetCount = 1;
    fragmentState.targets     = &colorTargetState;

    // create graphics mPipeline
    wgpu::RenderPipelineDescriptor2 mPipelineDescriptor;
    mPipelineDescriptor.layout       = mPipelineLayout;
    mPipelineDescriptor.vertex       = vertexState;
    mPipelineDescriptor.primitive    = primitiveState;
    mPipelineDescriptor.depthStencil = &depthStencilState;
    mPipelineDescriptor.multisample  = multisampleState;
    mPipelineDescriptor.fragment     = &fragmentState;

    mPipeline = mContextDawn->mDevice.CreateRenderPipeline(&mPipelineDescriptor);

    ImGui_ImplDawn_CreateFontsTexture(enableAlphaBlending);

    // Create uniform buffer
    wgpu::BufferDescriptor descriptor;
    descriptor.size = sizeof(VERTEX_CONSTANT_BUFFER);
    descriptor.usage =
        wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform | wgpu::BufferUsage::Uniform;

    mConstantBuffer = mContextDawn->mDevice.CreateBuffer(&descriptor);

    std::vector<wgpu::BindGroupEntry> bindGroupEntry;
    bindGroupEntry.resize(3);
    bindGroupEntry[0].binding      = 0;
    bindGroupEntry[0].buffer      = mConstantBuffer;
    bindGroupEntry[0].offset      = 0;
    bindGroupEntry[0].size        = sizeof(VERTEX_CONSTANT_BUFFER);
    bindGroupEntry[1].binding     = 1;
    bindGroupEntry[1].sampler     = mSampler;
    bindGroupEntry[2].binding     = 2;
    bindGroupEntry[2].textureView = mTextureView;
    mBindGroup = mContextDawn->makeBindGroup(layout, bindGroupEntry);

    return true;
}

bool ImGui_ImplDawn_Init(ContextDawn *context, wgpu::TextureFormat rtv_format)
{
    // Setup back-end capabilities flags
    ImGuiIO &io            = ImGui::GetIO();
    io.BackendRendererName = "imgui_impl_Dawn";
    io.BackendFlags |=
        ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field,
                                                 // allowing for large meshes.

    mFormat      = rtv_format;
    mContextDawn = context;

    mIndexBuffer      = NULL;
    mVertexBuffer     = NULL;
    mIndexBufferSize  = 3000;
    mVertexBufferSize = 3000;

    return true;
}

void ImGui_ImplDawn_Shutdown()
{
    delete mProgramDawn;

    mPipeline  = nullptr;
    mBindGroup = nullptr;
    mVsModule  = nullptr;
    mFsModule  = nullptr;

    mIndexBuffer    = nullptr;
    mVertexBuffer   = nullptr;
    mStagingBuffer  = nullptr;
    mTexture        = nullptr;
    mSampler        = nullptr;
    mConstantBuffer = nullptr;
    mTextureView    = nullptr;
}

void ImGui_ImplDawn_NewFrame(int MSAASampleCount, bool enableAlphaBlending)
{
    if (!mPipeline.Get())
        ImGui_ImplDawn_CreateDeviceObjects(MSAASampleCount, enableAlphaBlending);
}
