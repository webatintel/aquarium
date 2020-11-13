//
// Copyright (c) 2020 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef PLATFORMCONTEXTDAWN_H
#define PLATFORMCONTEXTDAWN_H

#include "dawn/dawn_wsi.h"

#include "../Aquarium.h"
#include "ContextDawn.h"

class PlatformContextDawn : public ContextDawn {
public:
  explicit PlatformContextDawn(BACKENDTYPE backendType);
  ~PlatformContextDawn() override;

private:
  DawnSwapChainImplementation *getSwapChainImplementation(
      wgpu::BackendType backendType) override;
  wgpu::TextureFormat getPreferredSwapChainTextureFormat(
      wgpu::BackendType backendType) override;

  DawnSwapChainImplementation mSwapchainImpl;
};

#endif  // PLATFORMCONTEXTDAWN_H
