//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Model.cpp: Implements Model class. Apply program for its model.
// Update uniforms, textures and buffers for each frame.

#include "Model.h"

#include "Globals.h"
#include "common/AQUARIUM_ASSERT.h"

Model::Model(
    Program *program_,
    const std::unordered_map<std::string, const AttribBuffer *> &arrays,
    const std::unordered_map<std::string, Texture *> *textures)
    : buffers(), textures(textures), program(program_), mode(GL_TRIANGLES) {
  setBuffers(arrays);

  program->setTextureUnits(*textures);
}

Model::~Model() {
  for (auto &buffer : buffers) {
    delete buffer.second;
    buffer.second = nullptr;
  }
}

void Model::setBuffer(const std::string &name, const AttribBuffer &array) {
  GLenum target = name == "indices" ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;

  if (buffers.find(name) == buffers.end()) {
    buffers[name] = new Buffer(array, target);
  }
}

void Model::setBuffers(
    const std::unordered_map<std::string, const AttribBuffer *> &arrays) {
  for (auto iter = arrays.cbegin(); iter != arrays.cend(); ++iter) {
    setBuffer(iter->first, *iter->second);
  }
}

void Model::applyBuffers() const {
  GLuint mVAO;
  glGenVertexArrays(1, &mVAO);
  glBindVertexArray(mVAO);

  // Apply array buffer and element buffer
  for (const auto &buffer : buffers) {
    if (buffer.first == "indices") {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.second->getBuffer());
    } else {
      glBindBuffer(GL_ARRAY_BUFFER, buffer.second->getBuffer());
      auto &attribLocs = program->getAttribLocs();
      if (attribLocs.find(buffer.first) == attribLocs.end()) {
        continue;
      }
      program->setAttrib(*buffer.second, buffer.first);
    }
  }
}

void Model::applyTextures() const {
  // Apply textures
  for (auto it = textures->cbegin(); it != textures->cend(); ++it) {
    program->setUniform(it->first, *it->second);
  }
}

void Model::prepareForDraw(const GenericConst &constUniforms) {
  program->use();

  applyBuffers();
  applyTextures();

  // Apply other uniforms
  program->setUniform("ambient", *constUniforms.ambient);
  program->setUniform("fogColor", *constUniforms.fogColor);
  program->setUniform("fogMult", constUniforms.fogMult);
  program->setUniform("fogOffset", constUniforms.fogOffset);
  program->setUniform("fogPower", constUniforms.fogPower);
  program->setUniform("lightColor", *constUniforms.lightColor);
  program->setUniform("shininess", constUniforms.shininess);
  program->setUniform("specular", *constUniforms.specular);
  program->setUniform("specularFactor", constUniforms.specularFactor);
  program->setUniform("lightWorldPos", *constUniforms.lightWorldPos);
  program->setUniform("viewInverse", *constUniforms.viewInverse);
  program->setUniform("viewProjection", *constUniforms.viewProjection);

  // The belowing uniforms belongs to innerConst
  program->setUniform("eta", constUniforms.eta);
  program->setUniform("refractionFudge", constUniforms.refractionFudge);
  program->setUniform("tankColorFudge", constUniforms.tankColorFudge);
}

void Model::prepareForDraw(const FishConst &fishConst) {
  prepareForDraw(fishConst.genericConst);

  program->setUniform("fishBendAmount", fishConst.constUniforms.fishBendAmount);
  program->setUniform("fishLength", fishConst.constUniforms.fishLength);
  program->setUniform("fishWaveLength", fishConst.constUniforms.fishWaveLength);
}

void Model::drawFunc() {
  int totalComponents = 0;

  if (buffers.find("indices") != buffers.end()) {
    totalComponents = buffers["indices"]->getTotalComponents();
    GLenum type = buffers["indices"]->getType();
    glDrawElements(mode, totalComponents, type, 0);
  } else {
    totalComponents = buffers["positions"]->getNumElements();
    glDrawArrays(mode, 0, totalComponents);
  }
}

void Model::draw(const GenericPer &perUniforms) {
  program->setUniform("world", *perUniforms.world);
  program->setUniform("worldInverse", *perUniforms.worldInverse);
  program->setUniform("worldInverseTranspose",
                      *perUniforms.worldInverseTranspose);
  program->setUniform("worldViewProjection", *perUniforms.worldViewProjection);

  drawFunc();
  ASSERT(glGetError() == GL_NO_ERROR);
}

void Model::draw(const FishPer &fishPer) {
  program->setUniform("worldPosition", fishPer.worldPosition);
  program->setUniform("nextPosition", fishPer.nextPosition);
  program->setUniform("scale", fishPer.scale);
  program->setUniform("time", fishPer.time);

  drawFunc();
  ASSERT(glGetError() == GL_NO_ERROR);
}
