//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Model.h: Define Class Model for all of the models.
// Contains programs, textures and buffers info of models.

#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <unordered_map>

#include "Buffer.h"
#include "Program.h"
#include "Texture.h"

struct FishConst;
struct FishPer;
struct GenericConst;
struct GenericPer;

class Model
{
public:
  Model() {}
  ~Model();
  Model(Program *program,
        const std::unordered_map<std::string, const AttribBuffer *> &arrays,
        const std::unordered_map<std::string, Texture *> *textures);

  void prepareForDraw(const GenericConst &constUniforms);
  void prepareForDraw(const FishConst &fishConst);
  void draw(const GenericPer &perUniforms);
  void draw(const FishPer &fishPer);

private:
  void setBuffers(
      const std::unordered_map<std::string, const AttribBuffer *> &arrays);
  void setBuffer(const std::string &name, const AttribBuffer &array);
  void applyBuffers() const;
  void applyTextures() const;
  void drawFunc();

  std::unordered_map<std::string, Buffer *> buffers;
  const std::unordered_map<std::string, Texture *> *textures;
  Program *program;

  GLenum mode;
};

#endif  // MODEL_H
