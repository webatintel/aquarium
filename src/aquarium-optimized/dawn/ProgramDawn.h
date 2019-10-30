//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// ProgramDawn.h: Defines Program wrapper of Dawn.
// Load shaders from folder shaders/dawn.

#pragma once
#ifndef PROGRAMDAWN_H
#define PROGRAMDAWN_H 1

#include "../Program.h"

#include <string>
#include <unordered_map>

#include "BufferDawn.h"
#include "TextureDawn.h"

#include "../Aquarium.h"

class ContextDawn;

class ProgramDawn : public Program
{
public:
    ProgramDawn() {}
    ProgramDawn(ContextDawn *context, const std::string &mVId, const std::string &mFId);
    ~ProgramDawn() override;

    void loadProgram();
    wgpu::ShaderModule getVSModule() { return mVsModule; }
    wgpu::ShaderModule getFSModule() { return mFsModule; }

  private:
    wgpu::ShaderModule mVsModule;
    wgpu::ShaderModule mFsModule;

    ContextDawn *context;
};

#endif

