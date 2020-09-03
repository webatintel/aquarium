//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelGL.h: Implements fish model of OpenGL.

#include "FishModelGL.h"

#include "ContextGL.h"
#include "ProgramGL.h"

FishModelGL::FishModelGL(const ContextGL *mContextGL,
                         Aquarium *aquarium,
                         MODELGROUP type,
                         MODELNAME name,
                         bool blend)
    : FishModel(type, name, blend, aquarium), mContextGL(mContextGL) {
  mViewInverseUniform.first = aquarium->lightWorldPositionUniform.viewInverse;
  mLightWorldPosUniform.first =
      aquarium->lightWorldPositionUniform.lightWorldPos;
  mLightColorUniform.first     = aquarium->lightUniforms.lightColor;
  mSpecularUniform.first       = aquarium->lightUniforms.specular;
  mShininessUniform.first      = 5.0f;
  mSpecularFactorUniform.first = 0.3f;
  mAmbientUniform.first        = aquarium->lightUniforms.ambient;
  mFogPowerUniform.first       = g_fogPower;
  mFogMultUniform.first        = g_fogMult;
  mFogOffsetUniform.first      = g_fogOffset;
  mFogColorUniform.first       = aquarium->fogUniforms.fogColor;

  mViewProjectionUniform.first =
      aquarium->lightWorldPositionUniform.viewProjection;
  mScaleUniform.first = 1;

  const Fish &fishInfo         = fishTable[name - MODELNAME::MODELSMALLFISHA];
  mFishLengthUniform.first     = fishInfo.fishLength;
  mFishBendAmountUniform.first = fishInfo.fishBendAmount;
  mFishWaveLengthUniform.first = fishInfo.fishWaveLength;
}

void FishModelGL::init() {
  ProgramGL *programGL = static_cast<ProgramGL *>(mProgram);
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
  mFishLengthUniform.second =
      mContextGL->getUniformLocation(programGL->getProgramId(), "fishLength");
  mFishWaveLengthUniform.second = mContextGL->getUniformLocation(
      programGL->getProgramId(), "fishWaveLength");
  mFishBendAmountUniform.second = mContextGL->getUniformLocation(
      programGL->getProgramId(), "fishBendAmount");

  mWorldPositionUniform.second = mContextGL->getUniformLocation(
      programGL->getProgramId(), "worldPosition");
  mNextPositionUniform.second =
      mContextGL->getUniformLocation(programGL->getProgramId(), "nextPosition");
  mScaleUniform.second =
      mContextGL->getUniformLocation(programGL->getProgramId(), "scale");
  mTimeUniform.second =
      mContextGL->getUniformLocation(programGL->getProgramId(), "time");

  mDiffuseTexture.first = static_cast<TextureGL *>(textureMap["diffuse"]);
  mDiffuseTexture.second =
      mContextGL->getUniformLocation(programGL->getProgramId(), "diffuse");
  mNormalTexture.first = static_cast<TextureGL *>(textureMap["normalMap"]);
  mNormalTexture.second =
      mContextGL->getUniformLocation(programGL->getProgramId(), "normalMap");
  mReflectionTexture.first =
      static_cast<TextureGL *>(textureMap["reflectionMap"]);
  mReflectionTexture.second = mContextGL->getUniformLocation(
      programGL->getProgramId(), "reflectionMap");
  mSkyboxTexture.first = static_cast<TextureGL *>(textureMap["skybox"]);
  mSkyboxTexture.second =
      mContextGL->getUniformLocation(programGL->getProgramId(), "skybox");

  mPositionBuffer.first = static_cast<BufferGL *>(bufferMap["position"]);
  mPositionBuffer.second =
      mContextGL->getAttribLocation(programGL->getProgramId(), "position");
  mNormalBuffer.first = static_cast<BufferGL *>(bufferMap["normal"]);
  mNormalBuffer.second =
      mContextGL->getAttribLocation(programGL->getProgramId(), "normal");
  mTexCoordBuffer.first = static_cast<BufferGL *>(bufferMap["texCoord"]);
  mTexCoordBuffer.second =
      mContextGL->getAttribLocation(programGL->getProgramId(), "texCoord");
  mTangentBuffer.first = static_cast<BufferGL *>(bufferMap["tangent"]);
  mTangentBuffer.second =
      mContextGL->getAttribLocation(programGL->getProgramId(), "tangent");
  mBiNormalBuffer.first = static_cast<BufferGL *>(bufferMap["binormal"]);
  mBiNormalBuffer.second =
      mContextGL->getAttribLocation(programGL->getProgramId(), "binormal");

  mIndicesBuffer = static_cast<BufferGL *>(bufferMap["indices"]);
}

void FishModelGL::draw() {
  mContextGL->drawElements(*mIndicesBuffer);
}

void FishModelGL::prepareForDraw() {
  mProgram->setProgram();
  mContextGL->enableBlend(mBlend);

  ProgramGL *programGL = static_cast<ProgramGL *>(mProgram);
  mContextGL->bindVAO(programGL->getVAOId());

  mContextGL->setAttribs(*mPositionBuffer.first, mPositionBuffer.second);
  mContextGL->setAttribs(*mNormalBuffer.first, mNormalBuffer.second);
  mContextGL->setAttribs(*mTexCoordBuffer.first, mTexCoordBuffer.second);

  mContextGL->setAttribs(*mTangentBuffer.first, mTangentBuffer.second);
  mContextGL->setAttribs(*mBiNormalBuffer.first, mBiNormalBuffer.second);

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
  mContextGL->setUniform(mFishBendAmountUniform.second,
                         &mFishBendAmountUniform.first, GL_FLOAT);
  mContextGL->setUniform(mFishLengthUniform.second, &mFishLengthUniform.first,
                         GL_FLOAT);
  mContextGL->setUniform(mFishWaveLengthUniform.second,
                         &mFishWaveLengthUniform.first, GL_FLOAT);

  // Fish models includes small, medium and big. Some of them contains
  // reflection and skybox texture, but some doesn't.
  mContextGL->setTexture(*mDiffuseTexture.first, mDiffuseTexture.second, 0);
  mContextGL->setTexture(*mNormalTexture.first, mNormalTexture.second, 1);
  if (mSkyboxTexture.second != -1 && mReflectionTexture.second != -1) {
    mContextGL->setTexture(*mReflectionTexture.first, mReflectionTexture.second,
                           2);
    mContextGL->setTexture(*mSkyboxTexture.first, mSkyboxTexture.second, 3);
  }
}

void FishModelGL::updatePerInstanceUniforms(
    const WorldUniforms &WorldUniforms) {
  mContextGL->setUniform(mScaleUniform.second, &mScaleUniform.first, GL_FLOAT);
  mContextGL->setUniform(mTimeUniform.second, &mTimeUniform.first, GL_FLOAT);
  mContextGL->setUniform(mWorldPositionUniform.second,
                         mWorldPositionUniform.first, GL_FLOAT_VEC3);
  mContextGL->setUniform(mNextPositionUniform.second,
                         mNextPositionUniform.first, GL_FLOAT_VEC3);
}

void FishModelGL::updateFishPerUniforms(float x,
                                        float y,
                                        float z,
                                        float nextX,
                                        float nextY,
                                        float nextZ,
                                        float scale,
                                        float time,
                                        int index) {
  mWorldPositionUniform.first[0] = x;
  mWorldPositionUniform.first[1] = y;
  mWorldPositionUniform.first[2] = z;
  mNextPositionUniform.first[0]  = nextX;
  mNextPositionUniform.first[1]  = nextY;
  mNextPositionUniform.first[2]  = nextZ;
  mScaleUniform.first            = scale;
  mTimeUniform.first             = time;
}
