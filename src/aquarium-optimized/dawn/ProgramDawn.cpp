//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "ProgramDawn.h"

#include <fstream>
#include <regex>
#include <string>

#include "ContextDawn.h"
#include "common/AQUARIUM_ASSERT.h"

ProgramDawn::ProgramDawn(ContextDawn *context,
                         const std::string &mVId,
                         const std::string &mFId)
    : Program(mVId, mFId),
      mVsModule(nullptr),
      mFsModule(nullptr),
      context(context) {
}

ProgramDawn::~ProgramDawn() {
  mVsModule = nullptr;
  mFsModule = nullptr;
}

void ProgramDawn::compileProgram(bool enableBlending,
                                 const std::string &alpha) {
  loadProgram();

  FragmentShaderCode = std::regex_replace(
      FragmentShaderCode, std::regex(R"(\n.*?// #noReflection)"), "");
  FragmentShaderCode = std::regex_replace(
      FragmentShaderCode, std::regex(R"(\n.*?// #noNormalMap)"), "");

  if (enableBlending) {
    FragmentShaderCode = std::regex_replace(
        FragmentShaderCode, std::regex(R"(diffuseColor.a)"), alpha);
  }

  mVsModule = context->createShaderModule(utils::SingleShaderStage::Vertex,
                                          VertexShaderCode);
  mFsModule = context->createShaderModule(utils::SingleShaderStage::Fragment,
                                          FragmentShaderCode);
}
