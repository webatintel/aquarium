//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "ProgramD3D12.h"

#include <cstring>
#include <regex>

#include "ContextD3D12.h"

ProgramD3D12::ProgramD3D12(ContextD3D12 *context, const std::string &mVId, const std::string &mFId)
    : Program(mVId, mFId), mVertexShader(nullptr), mPixelShader(nullptr), context(context)
{
}

ProgramD3D12::~ProgramD3D12() {}

void ProgramD3D12::compileProgram(bool enableBlending, const std::string& alpha)
{
    loadProgram();

    if (enableBlending)
    {
        FragmentShaderCode =
            std::regex_replace(FragmentShaderCode, std::regex(R"(diffuseColor.w)"), alpha);
    }

    mVertexShader = context->createShaderModule("VS", VertexShaderCode);
    mPixelShader  = context->createShaderModule("PS", FragmentShaderCode);
}
