//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "ResourceHelper.h"

#include <iostream>
#include <sstream>

#include "build/build_config.h"

#include "common/AQUARIUM_ASSERT.h"

#if defined(OS_WIN)
#include <Windows.h>
#include <direct.h>
const std::string slash = "\\";
#endif
#if defined(OS_MACOSX) && !defined(OS_IOS)
#include <mach-o/dyld.h>
const std::string slash = "/";
#endif
#if defined(OS_LINUX) && !defined(OS_CHROMEOS)
#include <unistd.h>
const std::string slash = "/";
#endif

static const char *shaderFolder = "shaders";
static const char *resourceFolder = "assets";

const std::vector<std::string> skyBoxUrls = {
    "GlobeOuter_EM_positive_x.jpg", "GlobeOuter_EM_negative_x.jpg",
    "GlobeOuter_EM_positive_y.jpg", "GlobeOuter_EM_negative_y.jpg",
    "GlobeOuter_EM_positive_z.jpg", "GlobeOuter_EM_negative_z.jpg"};

ResourceHelper::ResourceHelper(
    const std::string &mBackendName,
    const std::string &mShaderVersion,
    const std::pair<BACKENDTYPE, BACKENDTYPE> &backendType)
    : mBackendName(mBackendName),
      mBackendType(backendType),
      mShaderVersion(mShaderVersion) {
#if defined(OS_WIN)
  TCHAR temp[200];
  GetModuleFileName(nullptr, temp, MAX_PATH);
  std::wstring ws(temp);
  mPath = std::string(ws.begin(), ws.end());
#elif defined(OS_MACOSX) && !defined(OS_IOS)
  char temp[200];
  uint32_t size = sizeof(temp);
  _NSGetExecutablePath(temp, &size);
  mPath = std::string(temp);
#elif defined(OS_LINUX) && !defined(OS_CHROMEOS)
  char temp[200];
  readlink("/proc/self/exe", temp, sizeof(temp));
  mPath = std::string(temp);
#else
  ASSERT(false);
#endif

  size_t nPos = mPath.find_last_of(slash);
  std::ostringstream pathStream;
  pathStream << mPath.substr(0, nPos) << slash << ".." << slash << ".."
             << slash;
  mPath = pathStream.str();

  std::ostringstream placementStream;
  placementStream << mPath << resourceFolder << slash << "PropPlacement.js";
  mPropPlacementPath = placementStream.str();

  std::ostringstream imageStream;
  imageStream << mPath << resourceFolder << slash;
  mImagePath = imageStream.str();

  std::ostringstream programStream;
  programStream << mPath << shaderFolder << slash << mBackendName << slash
                << mShaderVersion << slash;
  mProgramPath = programStream.str();

  std::ostringstream fishBehaviorStream;
  fishBehaviorStream << mPath << "FishBehavior.json";
  mFishBehaviorPath = fishBehaviorStream.str();

  for (auto backend : {mBackendType.first, mBackendType.second}) {
    switch (backend) {
    case BACKENDTYPE::BACKENDTYPEDAWN:
      {
        mBackendTypeStr += "Dawn";
        mBackendTypeStr += " ";
        break;
      }
    case BACKENDTYPE::BACKENDTYPEANGLE:
      {
        mBackendTypeStr += "ANGLE";
#if 0
        mBackendTypeStr += " ";
#endif
        break;
      }
    case BACKENDTYPE::BACKENDTYPEOPENGL:
      {
        mBackendTypeStr += "OPENGL";
        break;
      }
    case BACKENDTYPE::BACKENDTYPED3D12:
      {
        mBackendTypeStr += "D3D12";
        break;
      }
    case BACKENDTYPE::BACKENDTYPEMETAL:
      {
        mBackendTypeStr += "Metal";
        break;
      }
    case BACKENDTYPE::BACKENDTYPEVULKAN:
      {
        mBackendTypeStr += "Vulkan";
        break;
      }
    case BACKENDTYPE::BACKENDTYPELAST:
      {
        break;
      }
    default:
      {
        std::cerr << "Backend type can not reached." << std::endl;
      }
    }
  }
}

void ResourceHelper::getSkyBoxUrls(std::vector<std::string> *skyUrls) const {
  for (auto &str : skyBoxUrls) {
    std::ostringstream url;
    url << mPath << resourceFolder << slash << str;

    skyUrls->emplace_back(url.str());
  }
}

std::string ResourceHelper::getModelPath(const std::string &modelName) const {
  std::ostringstream modelStream;
  modelStream << mImagePath << modelName << ".js";
  return modelStream.str();
}

const std::string &ResourceHelper::getProgramPath() const {
  return mProgramPath;
}

const std::string &ResourceHelper::getRendererInfo() const {
  return mRendererInfo;
}

void ResourceHelper::setRenderer(const std::string &renderer) {
  mRenderer = renderer;

  std::ostringstream rendererInfoStream;
#ifdef EGL_EGL_PROTOTYPES
  rendererInfoStream << mRenderer;
#else
  rendererInfoStream << mRenderer << " " << mBackendTypeStr << " "
                     << mShaderVersion;
#endif

  mRendererInfo = rendererInfoStream.str();
}
