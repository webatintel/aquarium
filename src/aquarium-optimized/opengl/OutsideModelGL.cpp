//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// OutsideModelGL.cpp: Implement outside model of OpenGL.

#include "OutsideModelGL.h"

OutsideModelGL::OutsideModelGL(const ContextGL *context,
                               Aquarium *aquarium,
                               MODELGROUP type,
                               MODELNAME name,
                               bool blend)
    : Model(type, name, blend), mContextGL(context) {
  mViewInverseUniform.first = aquarium->lightWorldPositionUniform.viewInverse;
  mLightWorldPosUniform.first =
      aquarium->lightWorldPositionUniform.lightWorldPos;
  mLightColorUniform.first     = aquarium->lightUniforms.lightColor;
  mSpecularUniform.first       = aquarium->lightUniforms.specular;
  mShininessUniform.first      = 50.0f;
  mSpecularFactorUniform.first = 0.0f;
  mAmbientUniform.first        = aquarium->lightUniforms.ambient;
  mWorldUniform.first          = aquarium->worldUniforms.world;
  mWorldViewProjectionUniform.first =
      aquarium->worldUniforms.worldViewProjection;
  mWorldInverseTransposeUniform.first =
      aquarium->worldUniforms.worldInverseTranspose;
  mFogPowerUniform.first  = 0;
  mFogMultUniform.first   = 0;
  mFogOffsetUniform.first = 0;
  mFogColorUniform.first  = aquarium->fogUniforms.fogColor;
}

void OutsideModelGL::init() {
  ProgramGL *programGL               = static_cast<ProgramGL *>(mProgram);
  mWorldViewProjectionUniform.second = mContextGL->getUniformLocation(
      programGL->getProgramId(), "worldViewProjection");
  mWorldUniform.second =
      mContextGL->getUniformLocation(programGL->getProgramId(), "world");
  mWorldInverseTransposeUniform.second = mContextGL->getUniformLocation(
      programGL->getProgramId(), "worldInverseTranspose");

  mViewInverseUniform.second =
      mContextGL->getUniformLocation(programGL->getProgramId(), "viewInverse");
  mLightWorldPosUniform.second = mContextGL->getUniformLocation(
      programGL->getProgramId(), "lightWorldPos");
  mLightColorUniform.second =
      mContextGL->getUniformLocation(programGL->getProgramId(), "lightColor");
  mSpecularUniform.second =
      mContextGL->getUniformLocation(programGL->getProgramId(), "specular");
  mAmbientUniform.second =
      mContextGL->getUniformLocation(programGL->getProgramId(), "ambient");
  mShininessUniform.second =
      mContextGL->getUniformLocation(programGL->getProgramId(), "shininess");
  mSpecularFactorUniform.second = mContextGL->getUniformLocation(
      programGL->getProgramId(), "specularFactor");

  mFogPowerUniform.second =
      mContextGL->getUniformLocation(programGL->getProgramId(), "fogPower");
  mFogMultUniform.second =
      mContextGL->getUniformLocation(programGL->getProgramId(), "fogMult");
  mFogOffsetUniform.second =
      mContextGL->getUniformLocation(programGL->getProgramId(), "fogOffset");
  mFogColorUniform.second =
      mContextGL->getUniformLocation(programGL->getProgramId(), "fogColor");

  mDiffuseTexture.first = static_cast<TextureGL *>(textureMap["diffuse"]);
  mDiffuseTexture.second =
      mContextGL->getUniformLocation(programGL->getProgramId(), "diffuse");

  mPositionBuffer.first = static_cast<BufferGL *>(bufferMap["position"]);
  mPositionBuffer.second =
      mContextGL->getAttribLocation(programGL->getProgramId(), "position");
  mNormalBuffer.first = static_cast<BufferGL *>(bufferMap["normal"]);
  mNormalBuffer.second =
      mContextGL->getAttribLocation(programGL->getProgramId(), "normal");
  mTexCoordBuffer.first = static_cast<BufferGL *>(bufferMap["texCoord"]);
  mTexCoordBuffer.second =
      mContextGL->getAttribLocation(programGL->getProgramId(), "texCoord");

  mIndicesBuffer = static_cast<BufferGL *>(bufferMap["indices"]);
}

void OutsideModelGL::draw() {
  mContextGL->drawElements(*mIndicesBuffer);
}

void OutsideModelGL::prepareForDraw() {
  mProgram->setProgram();
  mContextGL->enableBlend(mBlend);

  ProgramGL *programGL = static_cast<ProgramGL *>(mProgram);
  mContextGL->bindVAO(programGL->getVAOId());

  mContextGL->setAttribs(*mPositionBuffer.first, mPositionBuffer.second);
  mContextGL->setAttribs(*mNormalBuffer.first, mNormalBuffer.second);
  mContextGL->setAttribs(*mTexCoordBuffer.first, mTexCoordBuffer.second);

  mContextGL->setIndices(*mIndicesBuffer);

  mContextGL->setUniform(mViewInverseUniform.second, mViewInverseUniform.first,
                         GL_FLOAT_MAT4);
  mContextGL->setUniform(mLightWorldPosUniform.second,
                         mLightWorldPosUniform.first, GL_FLOAT_VEC3);
  mContextGL->setUniform(mLightColorUniform.second, mLightColorUniform.first,
                         GL_FLOAT_VEC4);
  mContextGL->setUniform(mSpecularUniform.second, mSpecularUniform.first,
                         GL_FLOAT_VEC4);
  mContextGL->setUniform(mShininessUniform.second, &mShininessUniform.first,
                         GL_FLOAT);
  mContextGL->setUniform(mSpecularFactorUniform.second,
                         &mSpecularFactorUniform.first, GL_FLOAT);
  mContextGL->setUniform(mAmbientUniform.second, mAmbientUniform.first,
                         GL_FLOAT_VEC4);
  mContextGL->setUniform(mFogPowerUniform.second, &mFogPowerUniform.first,
                         GL_FLOAT);
  mContextGL->setUniform(mFogMultUniform.second, &mFogMultUniform.first,
                         GL_FLOAT);
  mContextGL->setUniform(mFogOffsetUniform.second, &mFogOffsetUniform.first,
                         GL_FLOAT);
  mContextGL->setUniform(mFogColorUniform.second, mFogColorUniform.first,
                         GL_FLOAT_VEC4);

  mContextGL->setTexture(*mDiffuseTexture.first, mDiffuseTexture.second, 0);
}

void OutsideModelGL::updatePerInstanceUniforms(
    const WorldUniforms &worldUniforms) {
  mContextGL->setUniform(mWorldUniform.second, mWorldUniform.first,
                         GL_FLOAT_MAT4);
  mContextGL->setUniform(mWorldViewProjectionUniform.second,
                         mWorldViewProjectionUniform.first, GL_FLOAT_MAT4);
  mContextGL->setUniform(mWorldInverseTransposeUniform.second,
                         mWorldInverseTransposeUniform.first, GL_FLOAT_MAT4);
}
