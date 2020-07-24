//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "Context.h"

#include "Aquarium.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_internal.h"

#include <sstream>

void Context::renderImgui(const FPSTimer &fpsTimer,
                          int *fishCount,
                          std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> *toggleBitset)
{
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (show_option_window)
    {
        ImGui::SetNextWindowPos(ImVec2(500, 30), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_Appearing);

        ImGuiWindowFlags window_flags = 0;
        ImGui::Begin("Option Window", &show_option_window, window_flags);

        ImGui::Text("Number of Fish");
        static int selected  = -1;
        int fishSelection[6] = {1, 10000, 20000, 30000, 50000, 100000};
        char buf[6][32];
        for (int n = 0; n < 6; n++)
        {
            sprintf(buf[n], "%d", fishSelection[n]);
            if (ImGui::Selectable(buf[n], selected == n))
            {
                selected   = n;
                *fishCount = fishSelection[selected];
                memset(fishCountInputBuffer, 0, 64);
            }
        }

        char *pNext;
        ImGui::InputText("Specify fish count", fishCountInputBuffer, 64,
                         ImGuiInputTextFlags_CharsDecimal);
        int inputFishCount = strtol(fishCountInputBuffer, &pNext, 10);

        if (inputFishCount != 0)
        {
            *fishCount = inputFishCount;
        }

        ImGui::End();
    }

    {
        ImGui::Begin("Aquarium");

        std::string rendererInfo = mResourceHelper->getRendererInfo();
        ImGui::Text(rendererInfo.c_str());

        std::ostringstream resolutionStream;
        resolutionStream << "Resolution " << mClientWidth << "x" << mClientHeight;
        std::string resolution = resolutionStream.str();
        ImGui::Text(resolution.c_str());

        ImGui::PlotLines("[0,100 FPS]", fpsTimer.getHistoryFps(), NUM_HISTORY_DATA, 0, NULL, 0.0f,
                         100.0f, ImVec2(0, 40));

        ImGui::PlotHistogram("[0,100 ms/frame]", fpsTimer.getHistoryFrameTime(), NUM_HISTORY_DATA,
                             0, NULL, 0.0f, 100.0f, ImVec2(0, 40));

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / fpsTimer.getAverageFPS(), fpsTimer.getAverageFPS());

        if (mMSAASampleCount > 1)
        {
            ImGui::Text("MSAA: ON, Sample Count: %d", mMSAASampleCount);
        }
        else
        {
            ImGui::Text("MSAA: OFF");
        }

        if (toggleBitset->test(static_cast<size_t>(TOGGLE::ENABLEALPHABLENDING)))
        {
            ImGui::Text("ALPHABLENDING: ON");
        }
        else
        {
            ImGui::Text("ALPHABLENDING: OFF");
        }

        if (toggleBitset->test(static_cast<size_t>(TOGGLE::ENABLEDYNAMICBUFFEROFFSET)))
        {
            ImGui::Text("DBO: ON");
        }
        else
        {
            ImGui::Text("DBO: OFF");
        }

        if (toggleBitset->test(static_cast<size_t>(TOGGLE::ENABLEINSTANCEDDRAWS)))
        {
            ImGui::Text("INSTANCEDDRAWS: ON");
        }
        else
        {
            ImGui::Text("INSTANCEDDRAWS: OFF");
        }

        if (mResourceHelper->getBackendType() == BACKENDTYPE::BACKENDTYPEDAWND3D12 ||
            mResourceHelper->getBackendType() == BACKENDTYPE::BACKENDTYPED3D12)
        {
            if (toggleBitset->test(static_cast<size_t>(TOGGLE::DISABLED3D12RENDERPASS)))
            {
                ImGui::Text("RENDERPASS: OFF");
            }
            else
            {
                ImGui::Text("RENDERPASS: ON");
            }
        }

        if (mResourceHelper->getBackendType() == BACKENDTYPE::BACKENDTYPEDAWND3D12 ||
            mResourceHelper->getBackendType() == BACKENDTYPE::BACKENDTYPEDAWNVULKAN ||
            mResourceHelper->getBackendType() == BACKENDTYPE::BACKENDTYPEDAWNMETAL)
        {
            if (toggleBitset->test(static_cast<size_t>(TOGGLE::DISABLEDAWNVALIDATION)))
            {
                ImGui::Text("VALIDATION: OFF");
            }
            else
            {
                ImGui::Text("VALIDATION: ON");
            }

            if (toggleBitset->test(static_cast<size_t>(TOGGLE::BUFFERMAPPINGASYNC)))
            {
                ImGui::Text("BUFFERMAPPINGASNC: ON");
            }
            else
            {
                ImGui::Text("BUFFERMAPPINGASNC: OFF");
            }
        }

        ImGui::Checkbox("Option Window", &show_option_window);

        ImGui::End();
    }

    ImGui::Render();
}

void Context::setWindowSize(int windowWidth, int windowHeight)
{
    if (windowWidth != 0)
    {
        mClientWidth = windowWidth;
    }
    if (windowHeight != 0)
    {
        mClientHeight = windowHeight;
    }
}
