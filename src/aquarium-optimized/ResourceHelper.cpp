//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#include "ResourceHelper.h"

#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <direct.h>
#include "Windows.h"
const std::string slash = "\\";
#elif __APPLE__
#include <mach-o/dyld.h>
const std::string slash = "/";
#else
#include <unistd.h>
const std::string slash = "/";
#endif

static const char *shaderFolder   = "shaders";
static const char *resourceFolder = "assets";

const std::vector<std::string> skyBoxUrls = {
    "GlobeOuter_EM_positive_x.jpg", "GlobeOuter_EM_negative_x.jpg", "GlobeOuter_EM_positive_y.jpg",
    "GlobeOuter_EM_negative_y.jpg", "GlobeOuter_EM_positive_z.jpg", "GlobeOuter_EM_negative_z.jpg"};

ResourceHelper::ResourceHelper(const std::string &mBackendName,
                               const std::string &mShaderVersion,
                               BACKENDTYPE backendType)
    : mBackendName(mBackendName), mBackendType(backendType), mShaderVersion(mShaderVersion)
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
    TCHAR temp[200];
    GetModuleFileName(nullptr, temp, MAX_PATH);
    std::wstring ws(temp);
    mPath = std::string(ws.begin(), ws.end());
#elif __APPLE__
    char temp[200];
    uint32_t size = sizeof(temp);
    _NSGetExecutablePath(temp, &size);
    mPath = std::string(temp);
#else
    char temp[200];
    readlink("/proc/self/exe", temp, sizeof(temp));
    mPath = std::string(temp);
#endif

    size_t nPos = mPath.find_last_of(slash);
    std::ostringstream pathStream;
    pathStream << mPath.substr(0, nPos) << slash << ".." << slash << ".." << slash;
    mPath = pathStream.str();

    std::ostringstream programStream;
    programStream << mPath << shaderFolder << slash << mBackendName << slash << mShaderVersion
                  << slash;
    mProgramPath = programStream.str();

#ifdef __EMSCRIPTEN__
    std::ostringstream placementStream;
    placementStream << resourceFolder << slash << "PropPlacement.js";
    mPropPlacementPath = placementStream.str();

    std::ostringstream imageStream;
    imageStream << resourceFolder << slash;
    mImagePath = imageStream.str();
#else
    std::ostringstream placementStream;
    placementStream << mPath << resourceFolder << slash << "PropPlacement.js";
    mPropPlacementPath = placementStream.str();

    std::ostringstream imageStream;
    imageStream << mPath << resourceFolder << slash;
    mImagePath = imageStream.str();
#endif



    switch (mBackendType)
    {
        case BACKENDTYPE::BACKENDTYPEDAWND3D12:
        {
            mBackendTypeStr = "Dawn D3D12";
            break;
        }
        case BACKENDTYPE::BACKENDTYPEDAWNVULKAN:
        {
            mBackendTypeStr = "Dawn Vulkan";
            break;
        }
        case BACKENDTYPE::BACKENDTYPEDAWNMETAL:
        {
            mBackendTypeStr = "Dawn Metal";
            break;
        }
        case BACKENDTYPE::BACKENDTYPEANGLE:
        {
            mBackendTypeStr = "ANGLE";
            break;
        }
        case BACKENDTYPE::BACKENDTYPEOPENGL:
        {
            mBackendTypeStr = "OPENGL";
            break;
        }
        case BACKENDTYPE::BACKENDTYPED3D12:
        {
            mBackendTypeStr = "D3D12";
            break;
        }
        default:
        {
            std::cerr << "Backend type can not reached." << std::endl;
        }
    }
}

void ResourceHelper::getSkyBoxUrls(std::vector<std::string> *skyUrls) const
{
    for (auto &str : skyBoxUrls)
    {
        std::ostringstream url;
        url << mPath << resourceFolder << slash << str;

        skyUrls->emplace_back(url.str());
    }
}

std::string ResourceHelper::getModelPath(const std::string &modelName) const
{
    std::ostringstream modelStream;
    modelStream << mImagePath << modelName << ".js";
    return modelStream.str();
}

const std::string &ResourceHelper::getProgramPath() const
{
    return mProgramPath;
}

const std::string &ResourceHelper::getRendererInfo() const
{
    return mRendererInfo;
}

void ResourceHelper::setRenderer(const std::string &renderer)
{
    mRenderer = renderer;

    std::ostringstream rendererInfoStream;
#ifdef EGL_EGL_PROTOTYPES
    rendererInfoStream << mRenderer;
#else
    rendererInfoStream << mRenderer << " " << mBackendTypeStr << " " << mShaderVersion;
#endif

    mRendererInfo = rendererInfoStream.str();
}
