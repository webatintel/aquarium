//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Context.h: Defines the accessing to graphics API of a graphics backend.

#pragma once
#ifndef Context_H
#define Context_H 1

#include <bitset>
#include <string>
#include <vector>

#include "Aquarium.h"
#include "ResourceHelper.h"

#include "common/FPSTimer.h"

class Aquarium;
class Program;
class Buffer;
class Texture;
class Model;

enum BACKENDTYPE : short;
enum MODELGROUP : short;
enum MODELNAME : short;
enum TOGGLE : short;

struct Global;
static char fishCountInputBuffer[64];

class Context
{
  public:
    Context() : show_option_window(false) {}
    virtual ~Context() {}
    virtual bool initialize(BACKENDTYPE backend,
                            const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset,
                            int windowWidth,
                            int windowHeight)                                                 = 0;
    virtual Texture *createTexture(const std::string &name, const std::string &url)           = 0;
    virtual Texture *createTexture(const std::string &name,
                                   const std::vector<std::string> &urls)                      = 0;
    virtual Buffer *createBuffer(int numComponents, std::vector<float> *buffer, bool isIndex) = 0;
    virtual Buffer *createBuffer(int numComponents,
                                 std::vector<unsigned short> *buffer,
                                 bool isIndex)                                                = 0;
    virtual Program *createProgram(const std::string &mVId, const std::string &mFId)          = 0;
    virtual void setWindowTitle(const std::string &text)                                      = 0;
    virtual bool ShouldQuit()                                                                 = 0;
    virtual void KeyBoardQuit()                                                               = 0;
    virtual void DoFlush(
        const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset) = 0;
    virtual void Terminate()                                                                  = 0;
    virtual void Flush() {}
    virtual void preFrame()   = 0;
    virtual void showWindow() = 0;
    virtual void showFPS(const FPSTimer &fpsTimer,
                         int *fishCount,
                         std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> *toggleBitset) = 0;
    virtual void destoryImgUI() = 0;
    virtual void reallocResource(int preTotalInstance,
                                 int curTotalInstance,
                                 bool enableDynamicBufferOffset)   = 0;
    virtual void updateAllFishData(
        const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset) = 0;

    int getClientWidth() const { return mClientWidth; }
    int getclientHeight() const { return mClientHeight; }
    std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> getAvailableToggleBitset()
    {
        return mAvailableToggleBitset;
    }

    virtual Model *createModel(Aquarium *aquarium, MODELGROUP type, MODELNAME name, bool blend) = 0;

    virtual void initGeneralResources(Aquarium *aquarium) {}
    virtual void updateWorldlUniforms(Aquarium *aquarium) {}

    ResourceHelper *getResourceHelper() { return mResourceHelper; }

  protected:
    void renderImgui(const FPSTimer &fpsTimer,
                     int *fishCount,
                     std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> *toggleBitset);
    void setWindowSize(int windowWidth, int windowHeight);

    int mClientWidth;
    int mClientHeight;
    int mPreTotalInstance;
    int mCurTotalInstance;

    ResourceHelper *mResourceHelper;

    std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> mAvailableToggleBitset;
    virtual void initAvailableToggleBitset(BACKENDTYPE backendType) = 0;

  private:
    bool show_option_window;
};

#endif
