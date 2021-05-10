//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// SeaweedModelGL.cpp: Impplment seaweed model of OpenGL.

#include "SeaweedModelGL.h"

SeaweedModelGL::SeaweedModelGL(const ContextGL *context,
                               Aquarium *aquarium,
                               MODELGROUP type,
                               MODELNAME name,
                               bool blend)
    : SeaweedModel(type, name, blend), mContextGL(context) {
  mViewInverseUniform.first = aquarium->lightWorldPositionUniform.viewInverse;
  mLightWorldPosUniform.first =
      aquarium->lightWorldPositionUniform.lightWorldPos;
  mLightColorUniform.first = aquarium->lightUniforms.lightColor;
  mSpecularUniform.first = aquarium->lightUniforms.specular;
  mShininessUniform.first = 50.0f;
  mSpecularFactorUniform.first = 1.0f;
  mAmbientUniform.first = aquarium->lightUniforms.ambient;
  mWorldUniform.first = aquarium->worldUniforms.world;
  mFogPowerUniform.first = g_fogPower;
  mFogMultUniform.first = g_fogMult;
  mFogOffsetUniform.first = g_fogOffset;
  mFogColorUniform.first = aquarium->fogUniforms.fogColor;
  mViewProjectionUniform.first =
      aquarium->lightWorldPositionUniform.viewProjection;
}

void SeaweedModelGL::init() {
  ProgramGL *programGL = static_cast<ProgramGL *>(mProgram);
  mWorldUniform.second =
      mContextGL->getUniformLocation(programGL->getProgramId(), "world");

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

  mViewProjectionUniform.second = mContextGL->getUniformLocation(
      programGL->getProgramId(), "viewProjection");
  mTimeUniform.second =
      mContextGL->getUniformLocation(programGL->getProgramId(), "time");

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

void SeaweedModelGL::draw() {
  mContextGL->drawElements(*mIndicesBuffer);
}

void SeaweedModelGL::prepareForDraw() {
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
  mContextGL->setUniform(mViewProjectionUniform.second,
                         mViewProjectionUniform.first, GL_FLOAT_MAT4);

  mContextGL->setTexture(*mDiffuseTexture.first, mDiffuseTexture.second, 0);
}

void SeaweedModelGL::updatePerInstanceUniforms(
    const WorldUniforms &worldUniforms) {
  mContextGL->setUniform(mWorldUniform.second, mWorldUniform.first,
                         GL_FLOAT_MAT4);
  mContextGL->setUniform(mTimeUniform.second, &mTimeUniform.first, GL_FLOAT);
}

void SeaweedModelGL::updateSeaweedModelTime(float time) {
  mTimeUniform.first = time;
}
