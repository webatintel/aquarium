//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Texture.h: Define Texture class.

#ifndef TEXTURE_H
#define TEXTURE_H

#include <unordered_map>
#include <vector>

#include "glad/glad.h"

#include "common/Path.h"

class Texture {
public:
  ~Texture();
  Texture(const Path &url, bool flip);
  Texture(const std::vector<Path> &urls);

  GLuint getTexture() const { return texture; }
  GLenum getTarget() const { return target; }
  void setTexture(GLuint texId) { texture = texId; }
  bool loadImageBySTB(Path &filename, uint8_t **pixels);
  void DestroyImageData(uint8_t *pixels);

private:
  void setParameter(GLenum, GLint);
  void uploadTextures();
  bool isPowerOf2(int value);

  std::vector<Path> urls;
  GLenum target;
  GLuint texture;
  std::unordered_map<GLenum, GLint> params;
  int width;
  int height;
  bool flip;
};

#endif  // TEXTURE_H
