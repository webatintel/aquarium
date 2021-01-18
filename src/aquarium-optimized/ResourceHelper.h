//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef RESOURCEHELPER_H
#define RESOURCEHELPER_H

#include <string>
#include <vector>

#include "Aquarium.h"
#include "common/Path.h"

class ResourceHelper {
public:
  ResourceHelper(const std::string &mBackendName,
                 const std::string &mShaderVersion,
                 BACKENDTYPE backendType);
  void getSkyBoxUrls(std::vector<Path> *skyUrls) const;
  Path getPropPlacementPath() const { return mPropPlacementPath; }
  Path getImagePath() const { return mImagePath; }
  Path getModelPath(const std::string &modelName) const;
  Path getProgramPath() const;
  Path getFishBehaviorPath() const { return mFishBehaviorPath; }
  const std::string &getBackendName() const { return mBackendName; }
  BACKENDTYPE getBackendType() const { return mBackendType; }
  const std::string &getShaderVersion() const { return mShaderVersion; }
  const std::string &getRendererInfo() const;
  void setRenderer(const std::string &renderer);

private:
  Path mPath;
  Path mImagePath;
  Path mProgramPath;
  Path mPropPlacementPath;
  Path mModelPath;
  Path mFishBehaviorPath;

  std::string mBackendName;
  BACKENDTYPE mBackendType;
  std::string mBackendTypeStr;

  std::string mShaderVersion;

  std::string mRenderer;
  std::string mRendererInfo;
};

#endif  // RESOURCEHELPER_H
