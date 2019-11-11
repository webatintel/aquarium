// dear imgui: Renderer for Dawn
// This needs to be used along with a Platform Binding (e.g. GLFW)

#include "ProgramDawn.h"

#include "imgui_impl_dawn.h"
#include "imgui.h"
#include "utils/ComboRenderPipelineDescriptor.h"

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
bool mEnableMSAA      = false;
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
    mConstantBuffer.SetSubData(0, sizeof(VERTEX_CONSTANT_BUFFER), &vertex_constant_buffer.mvp);

    // TODO(yizhou): setting viewport isn't supported in dawn yet.
    // Setup viewport
    // pass.SetViewport(0.0f, 0.0f, draw_data->DisplaySize.x, draw_data->DisplaySize.y, 0.0f, 1.0f);

    pass.SetPipeline(mPipeline);
    pass.SetBindGroup(0, mBindGroup, 0, nullptr);
    pass.SetVertexBuffer(0, mVertexBuffer);
    pass.SetIndexBuffer(mIndexBuffer, 0);
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
    uint32_t vtx_dst = 0;
    uint32_t idx_dst = 0;
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
        mContextDawn->setBufferData(mVertexBuffer, 0, vtx_dst, mVertexData);
        mContextDawn->setBufferData(mIndexBuffer, 0, idx_dst, mIndexData);
    }

    const wgpu::RenderPassEncoder &pass = mContextDawn->getRenderPass();

    // Setup desired Dawn state
    ImGui_ImplDawn_SetupRenderState(draw_data, pass);

    // Render pass
    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    int global_vtx_offset = 0;
    int global_idx_offset = 0;
    ImVec2 clip_off       = draw_data->DisplayPos;
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
                pass.SetScissorRect(pcmd->ClipRect.x - clip_off.x, pcmd->ClipRect.y - clip_off.y,
                                    pcmd->ClipRect.z - clip_off.x, pcmd->ClipRect.w - clip_off.y);
                pass.DrawIndexed(pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset,
                                 pcmd->VtxOffset + global_vtx_offset, 0);
            }
        }
        global_idx_offset += cmd_list->IdxBuffer.Size;
        global_vtx_offset += cmd_list->VtxBuffer.Size;
    }
}

static void ImGui_ImplDawn_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO &io = ImGui::GetIO();
    unsigned char *pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // Upload texture to graphics system
    {
        wgpu::TextureDescriptor descriptor;
        descriptor.dimension       = wgpu::TextureDimension::e2D;
        descriptor.size.width      = width;
        descriptor.size.height     = height;
        descriptor.size.depth      = 1;
        descriptor.arrayLayerCount = 1;
        descriptor.sampleCount     = 1;
        descriptor.format          = mFormat;
        descriptor.mipLevelCount   = 1;
        descriptor.usage           = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::Sampled;
        mTexture                   = mContextDawn->createTexture(descriptor);

        mStagingBuffer = mContextDawn->createBufferFromData(pixels, width * height * 4,
                                                            wgpu::BufferUsage::CopySrc);
        wgpu::BufferCopyView bufferCopyView =
            mContextDawn->createBufferCopyView(mStagingBuffer, 0, width * 4, height);
        wgpu::TextureCopyView textureCopyView =
            mContextDawn->createTextureCopyView(mTexture, 0, 0, {0, 0, 0});
        wgpu::Extent3D copySize = {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1};
        wgpu::CommandBuffer copyCommand =
            mContextDawn->copyBufferToTexture(bufferCopyView, textureCopyView, copySize);
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

        wgpu::SamplerDescriptor samplerDesc;
        samplerDesc.addressModeU    = wgpu::AddressMode::Repeat;
        samplerDesc.addressModeV    = wgpu::AddressMode::Repeat;
        samplerDesc.addressModeW    = wgpu::AddressMode::Repeat;
        samplerDesc.minFilter       = wgpu::FilterMode::Linear;
        samplerDesc.magFilter       = wgpu::FilterMode::Linear;
        samplerDesc.lodMinClamp     = 0.0f;
        samplerDesc.lodMaxClamp     = 0.0f;
        samplerDesc.compare         = wgpu::CompareFunction::Always;
        samplerDesc.mipmapFilter    = wgpu::FilterMode::Linear;

        mSampler = mContextDawn->createSampler(samplerDesc);
    }

    io.Fonts->TexID = (ImTextureID)mTextureView.Get();
}

bool ImGui_ImplDawn_CreateDeviceObjects()
{
    if (!mContextDawn->mDevice)
        return false;

    utils::ComboVertexStateDescriptor mVertexStateDescriptor;
    mVertexStateDescriptor.cVertexBuffers[0].attributeCount = 3;
    mVertexStateDescriptor.cVertexBuffers[0].arrayStride    = sizeof(ImDrawVert);
    mVertexStateDescriptor.cAttributes[0].format            = wgpu::VertexFormat::Float2;
    mVertexStateDescriptor.cAttributes[0].shaderLocation    = 0;
    mVertexStateDescriptor.cAttributes[0].offset            = 0;
    mVertexStateDescriptor.cAttributes[1].format            = wgpu::VertexFormat::Float2;
    mVertexStateDescriptor.cAttributes[1].shaderLocation    = 1;
    mVertexStateDescriptor.cAttributes[1].offset            = IM_OFFSETOF(ImDrawVert, uv);
    mVertexStateDescriptor.cAttributes[2].format            = wgpu::VertexFormat::UChar4Norm;
    mVertexStateDescriptor.cAttributes[2].shaderLocation    = 2;
    mVertexStateDescriptor.cAttributes[2].offset            = IM_OFFSETOF(ImDrawVert, col);

    mVertexStateDescriptor.cVertexBuffers[0].attributes = &mVertexStateDescriptor.cAttributes[0];
    mVertexStateDescriptor.vertexBufferCount            = 1;
    mVertexStateDescriptor.indexFormat                  = wgpu::IndexFormat::Uint16;

    // Create bind group layout
    wgpu::BindGroupLayout layout = mContextDawn->MakeBindGroupLayout(
        {{0, wgpu::ShaderStage::Vertex, wgpu::BindingType::UniformBuffer},
         {1, wgpu::ShaderStage::Fragment, wgpu::BindingType::Sampler},
         {2, wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture}});

    wgpu::PipelineLayout mPipelineLayout = mContextDawn->MakeBasicPipelineLayout({layout});

    ResourceHelper *resourceHelper = mContextDawn->getResourceHelper();
    std::string programPath        = resourceHelper->getProgramPath();
    mProgramDawn = new ProgramDawn(mContextDawn, programPath + "imguiVertexShader",
                                   programPath + "imguiFragmentShader");
    mProgramDawn->loadProgram();

    const wgpu::ShaderModule &mVsModule = mProgramDawn->getVSModule();
    const wgpu::ShaderModule &mFsModule = mProgramDawn->getFSModule();

    wgpu::BlendDescriptor blendDescriptor;
    blendDescriptor.operation = wgpu::BlendOperation::Add;
    blendDescriptor.srcFactor = wgpu::BlendFactor::SrcAlpha;
    blendDescriptor.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
    wgpu::BlendDescriptor alphaDescriptor;
    alphaDescriptor.operation = wgpu::BlendOperation::Add;
    alphaDescriptor.srcFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
    alphaDescriptor.dstFactor = wgpu::BlendFactor::Zero;

    wgpu::ColorStateDescriptor ColorStateDescriptor;
    ColorStateDescriptor.colorBlend = blendDescriptor;
    ColorStateDescriptor.alphaBlend = alphaDescriptor;
    ColorStateDescriptor.writeMask  = wgpu::ColorWriteMask::All;

    wgpu::RasterizationStateDescriptor rasterizationState;
    rasterizationState.nextInChain         = nullptr;
    rasterizationState.frontFace           = wgpu::FrontFace::CCW;
    rasterizationState.cullMode            = wgpu::CullMode::None;
    rasterizationState.depthBias           = 0;
    rasterizationState.depthBiasSlopeScale = 0.0;
    rasterizationState.depthBiasClamp      = 0.0;

    // create graphics mPipeline
    utils::ComboRenderPipelineDescriptor mPipelineDescriptor(mContextDawn->mDevice);
    mPipelineDescriptor.layout                    = mPipelineLayout;
    mPipelineDescriptor.vertexStage.module        = mVsModule;
    mPipelineDescriptor.cFragmentStage.module     = mFsModule;
    mPipelineDescriptor.cVertexState              = mVertexStateDescriptor;
    mPipelineDescriptor.depthStencilState         = &mPipelineDescriptor.cDepthStencilState;
    mPipelineDescriptor.cDepthStencilState.format = wgpu::TextureFormat::Depth24PlusStencil8;
    mPipelineDescriptor.cColorStates[0]           = ColorStateDescriptor;
    mPipelineDescriptor.cColorStates[0].format    = mFormat;
    mPipelineDescriptor.cDepthStencilState.depthWriteEnabled = false;
    mPipelineDescriptor.cDepthStencilState.depthCompare      = wgpu::CompareFunction::Always;
    mPipelineDescriptor.primitiveTopology  = wgpu::PrimitiveTopology::TriangleList;
    mPipelineDescriptor.sampleCount        = mEnableMSAA ? 4 : 1;
    mPipelineDescriptor.rasterizationState = &rasterizationState;

    mPipeline = mContextDawn->mDevice.CreateRenderPipeline(&mPipelineDescriptor);

    ImGui_ImplDawn_CreateFontsTexture();

    // Create uniform buffer
    wgpu::BufferDescriptor descriptor;
    descriptor.size  = sizeof(VERTEX_CONSTANT_BUFFER);
    descriptor.usage =
        wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform | wgpu::BufferUsage::Uniform;

    mConstantBuffer = mContextDawn->mDevice.CreateBuffer(&descriptor);

    mBindGroup = mContextDawn->makeBindGroup(
        layout, {{0, mConstantBuffer, 0, sizeof(VERTEX_CONSTANT_BUFFER)},
                 {1, mSampler},
                 {2, mTextureView}});

    return true;
}

bool ImGui_ImplDawn_Init(ContextDawn *context, wgpu::TextureFormat rtv_format, bool enableMSAA)
{
    // Setup back-end capabilities flags
    ImGuiIO &io            = ImGui::GetIO();
    io.BackendRendererName = "imgui_impl_Dawn";
    io.BackendFlags |=
        ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field,
                                                 // allowing for large meshes.

    mFormat     = rtv_format;
    mContextDawn = context;

    mIndexBuffer      = NULL;
    mVertexBuffer     = NULL;
    mIndexBufferSize  = 3000;
    mVertexBufferSize = 3000;
    mEnableMSAA       = enableMSAA;

    return true;
}

void ImGui_ImplDawn_Shutdown()
{
    delete mProgramDawn;

    mPipeline  = nullptr;
    mBindGroup = nullptr;
    mVsModule  = nullptr;
    mFsModule  = nullptr;

    mIndexBuffer  = nullptr;
    mVertexBuffer = nullptr;
    mStagingBuffer   = nullptr;
    mTexture      = nullptr;
    mSampler      = nullptr;
    mConstantBuffer = nullptr;
    mTextureView     = nullptr;
}

void ImGui_ImplDawn_NewFrame()
{
    if (!mPipeline.Get())
        ImGui_ImplDawn_CreateDeviceObjects();
}
