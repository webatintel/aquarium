//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#include <cstring>
#include <fstream>

#include "ContextD3D12.h"
#include "ProgramD3D12.h"

ProgramD3D12::ProgramD3D12(ContextD3D12 *context, const std::string &mVId, const std::string &mFId)
    : Program(mVId, mFId), mVertexShader(nullptr), mPixelShader(nullptr), context(context)
{
}

ProgramD3D12::~ProgramD3D12() {}

void ProgramD3D12::loadProgram()
{
    std::ifstream VertexShaderStream(mVId, std::ios::in);
    std::string VertexShaderCode((std::istreambuf_iterator<char>(VertexShaderStream)),
                                 std::istreambuf_iterator<char>());
    VertexShaderStream.close();

    // Read the Fragment Shader code from the file
    std::ifstream FragmentShaderStream(mFId, std::ios::in);
    std::string FragmentShaderCode((std::istreambuf_iterator<char>(FragmentShaderStream)),
                                   std::istreambuf_iterator<char>());
    FragmentShaderStream.close();

    mVertexShader = context->createShaderModule("VS", VertexShaderCode);
    mPixelShader  = context->createShaderModule("PS", FragmentShaderCode);
}
