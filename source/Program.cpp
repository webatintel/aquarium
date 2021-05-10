//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Program.cpp: Load programs from shaders.

#include "Program.h"

#include <fstream>

void Program::loadProgram() {
  std::ifstream VertexShaderStream(mVId, std::ios::in);
  VertexShaderCode =
      std::string((std::istreambuf_iterator<char>(VertexShaderStream)),
                  std::istreambuf_iterator<char>());
  VertexShaderStream.close();

  // Read the Fragment Shader code from the file
  std::ifstream FragmentShaderStream(mFId, std::ios::in);
  FragmentShaderCode =
      std::string((std::istreambuf_iterator<char>(FragmentShaderStream)),
                  std::istreambuf_iterator<char>());
  FragmentShaderStream.close();
}
