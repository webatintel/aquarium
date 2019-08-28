//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// ProgramGL.cpp: Implement Program wrapper of OpenGL.
// Load shaders from folder shaders/opengl.
// Compiles OpenGL shaders and check if compiled success.
// Apply Buffers, Textures and Uniforms to program.

#ifdef EGL_EGL_PROTOTYPES
#include <angle_gl.h>
#include <memory>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "EGL/eglext_angle.h"
#include "EGL/eglplatform.h"
#include "EGLWindow.h"
#elif __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#else
#include "glad/glad.h"
#endif

#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>

#include "common/AQUARIUM_ASSERT.h"
#include "ProgramGL.h"

#include "../Buffer.h"
#include "../Texture.h"

ProgramGL::ProgramGL(ContextGL *context, std::string mVId, std::string mFId)
    : Program(mVId, mFId), mProgramId(0u), mContext(context)
{
    mProgramId= context->generateProgram();
    mVAO = context->generateVAO();
}

ProgramGL::~ProgramGL()
{
    mContext->deleteVAO(mVAO);
    mContext->deleteProgram(mProgramId);
}

void ProgramGL::loadProgram()
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

    const std::string fogUniforms =
        R"(uniform float fogPower;
        uniform float fogMult;
        uniform float fogOffset;
        uniform vec4 fogColor;)";
    const std::string fogCode =
        R"(outColor = mix(outColor, vec4(fogColor.rgb, diffuseColor.a),
        clamp(pow((v_position.z / v_position.w), fogPower) * fogMult - fogOffset,0.0,1.0));)";

#ifdef __APPLE__
    VertexShaderCode   = std::regex_replace(VertexShaderCode, std::regex(R"(#version 450 core)"),
                                          R"(#version 410 core)");
    FragmentShaderCode = std::regex_replace(FragmentShaderCode, std::regex(R"(#version 450 core)"),
                                            R"(#version 410 core)");
#endif

    // enable fog, reflection and normalMaps
    FragmentShaderCode =
        std::regex_replace(FragmentShaderCode, std::regex(R"(// #fogUniforms)"), fogUniforms);
    FragmentShaderCode =
        std::regex_replace(FragmentShaderCode, std::regex(R"(// #fogCode)"), fogCode);

    FragmentShaderCode =
        std::regex_replace(FragmentShaderCode, std::regex(R"(\n.*?// #noReflection)"), "");
    FragmentShaderCode =
        std::regex_replace(FragmentShaderCode, std::regex(R"(\n.*?// #noNormalMap)"), "");

    bool status = mContext->compileProgram(mProgramId, VertexShaderCode, FragmentShaderCode);
    ASSERT(status);
    if (!status) {
        std::cout << "Error occurs in compiling program!" << std::endl;
    }
}

void ProgramGL::setProgram()
{
    mContext->setProgram(mProgramId);
}
