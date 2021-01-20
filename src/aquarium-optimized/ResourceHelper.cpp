//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "ResourceHelper.h"

#include <iostream>
#include <sstream>

#include "common/AQUARIUM_ASSERT.h"

static const char *shaderFolder = "shaders";
static const char *resourceFolder = "assets";

const std::vector<std::string> skyBoxUrls = {
    "GlobeOuter_EM_positive_x.jpg", "GlobeOuter_EM_negative_x.jpg",
    "GlobeOuter_EM_positive_y.jpg", "GlobeOuter_EM_negative_y.jpg",
    "GlobeOuter_EM_positive_z.jpg", "GlobeOuter_EM_negative_z.jpg"};

ResourceHelper::ResourceHelper(const std::string &mBackendName,
                               const std::string &mShaderVersion,
                               BACKENDTYPE backendType)
    : mPath(Path::getVoidPath()),
      mImagePath(Path::getVoidPath()),
      mProgramPath(Path::getVoidPath()),
      mPropPlacementPath(Path::getVoidPath()),
      mModelPath(Path::getVoidPath()),
      mFishBehaviorPath(Path::getVoidPath()),
      mBackendName(mBackendName),
      mBackendType(backendType),
      mShaderVersion(mShaderVersion) {
  mPath.push(Path::getExecutablePath()).pop().pop().pop();
  mPropPlacementPath.push(mPath).push(resourceFolder).push("PropPlacement.js");
  mImagePath.push(mPath).push(resourceFolder);
  mProgramPath.push(mPath)
      .push(shaderFolder)
      .push(mBackendName)
      .push(mShaderVersion);
  mFishBehaviorPath.push(mPath).push("FishBehavior.json");

  int expo = 0;
  while (1 << expo++ != BACKENDTYPE::BACKENDTYPENONE) {
    if (mBackendType & 1 << expo) {
      if (mBackendTypeStr != "")
        mBackendTypeStr += " ";
      if (1 << expo == BACKENDTYPE::BACKENDTYPEANGLE)
        mBackendTypeStr += "ANGLE";
      else if (1 << expo == BACKENDTYPE::BACKENDTYPEDAWN)
        mBackendTypeStr += "Dawn";
      else if (1 << expo == BACKENDTYPE::BACKENDTYPED3D11)
        mBackendTypeStr += "D3D11";
      else if (1 << expo == BACKENDTYPE::BACKENDTYPED3D12)
        mBackendTypeStr += "D3D12";
      else if (1 << expo == BACKENDTYPE::BACKENDTYPEMETAL)
        mBackendTypeStr += "Metal";
      else if (1 << expo == BACKENDTYPE::BACKENDTYPEOPENGL)
        mBackendTypeStr += "OpenGL";
      else if (1 << expo == BACKENDTYPE::BACKENDTYPEVULKAN)
        mBackendTypeStr += "Vulkan";
    }
  }
}

void ResourceHelper::getSkyBoxUrls(std::vector<Path> *skyUrls) const {
  for (auto &str : skyBoxUrls) {
    Path url = Path(mPath).push(resourceFolder).push(str);
    skyUrls->emplace_back(url);
  }
}

Path ResourceHelper::getModelPath(const std::string &modelName) const {
  return Path(mImagePath).push(modelName + ".js");
}

Path ResourceHelper::getProgramPath() const {
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
