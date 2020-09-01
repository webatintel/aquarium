//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Scene.h: Defines Scene class.

#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "AttribBuffer.h"
#include "Texture.h"

class Model;

class Scene
{
  public:
    Scene() {}
    ~Scene();
    Scene(const std::string opt_programIds[2]);

    void load(const std::string &path, const std::string &name);
    const std::vector<Model *> &getModels() const { return models; }

    bool loaded;

  private:
    void setupSkybox(const std::string &path);

    std::string programIds[2];
    std::string url;
    std::vector<Model *> models;
    std::unordered_map<std::string, Texture *> textureMap;
    std::unordered_map<std::string, const AttribBuffer *> arrayMap;
};

#endif  // SCENE_H
