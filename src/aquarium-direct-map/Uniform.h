//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Uniform.h: Define Uniform Class. Store uniform infos queried from shaders.

#ifndef UNIFORM_H
#define UNIFORM_H

#include <string>

#include "glad/glad.h"

class Uniform
{
public:
  Uniform() {}
  Uniform(const std::string &name,
          GLenum type,
          int length,
          int size,
          GLint index);

  std::string getName() const { return name; }
  GLenum getType() const { return type; }
  GLsizei getLength() const { return length; }
  GLsizei getSize() const { return size; }
  GLint getIndex() const { return index; }

private:
  std::string name;
  GLenum type;
  GLsizei length, size;
  GLint index;
};

#endif  // UNIFORM_H
