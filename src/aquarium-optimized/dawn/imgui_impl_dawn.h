// dear imgui: Renderer for Dawn
// This needs to be used along with a Platform Binding (e.g. GLFW)

#pragma once

#include <dawn/dawncpp.h>
#include <dawn_native/DawnNative.h>

#include "ContextDawn.h"
#include "imgui.h"
#include "utils/DawnHelpers.h"

IMGUI_IMPL_API bool ImGui_ImplDawn_Init(ContextDawn *context,
                                        dawn::TextureFormat rtv_format,
                                        bool enableMSAA);
IMGUI_IMPL_API void ImGui_ImplDawn_Shutdown();
IMGUI_IMPL_API void ImGui_ImplDawn_NewFrame();
IMGUI_IMPL_API void ImGui_ImplDawn_RenderDrawData(ImDrawData *draw_data);

// Use if you want to reset your rendering mDevice without losing ImGui state.
IMGUI_IMPL_API bool ImGui_ImplDawn_CreateDeviceObjects();
