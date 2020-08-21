//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelGL.h: Defines fish model of OpenGL.

#ifndef FISHMODELGL_H
#define FISHMODELGL_H

#include <string>

#include "../FishModel.h"

class TextureGL;
class BufferGL;

class FishModelGL : public FishModel
{
  public:
    FishModelGL(const ContextGL *context, Aquarium *aquarium, MODELGROUP type, MODELNAME name, bool blend);
    void prepareForDraw() override;
    void updatePerInstanceUniforms(const WorldUniforms &worldUniforms) override;

    void init() override;
    void draw() override;

    void updateFishPerUniforms(float x,
                               float y,
                               float z,
                               float nextX,
                               float nextY,
                               float nextZ,
                               float scale,
                               float time,
                               int index) override;

    std::pair<float *, int> mViewInverseUniform;
    std::pair<float *, int> mLightWorldPosUniform;
    std::pair<float *, int> mLightColorUniform;
    std::pair<float *, int> mSpecularUniform;
    std::pair<float, int> mShininessUniform;
    std::pair<float, int> mSpecularFactorUniform;

    std::pair<float *, int> mAmbientUniform;

    std::pair<float, int> mFogPowerUniform;
    std::pair<float, int> mFogMultUniform;
    std::pair<float, int> mFogOffsetUniform;
    std::pair<float *, int> mFogColorUniform;

    std::pair<float *, int> mViewProjectionUniform;
    std::pair<float, int> mFishLengthUniform;
    std::pair<float, int> mFishWaveLengthUniform;
    std::pair<float, int> mFishBendAmountUniform;

    std::pair<float[3], int> mWorldPositionUniform;
    std::pair<float[3], int> mNextPositionUniform;
    std::pair<float, int> mScaleUniform;
    std::pair<float, int> mTimeUniform;

    std::pair<TextureGL *, int> mDiffuseTexture;
    std::pair<TextureGL *, int> mNormalTexture;
    std::pair<TextureGL *, int> mReflectionTexture;
    std::pair<TextureGL *, int> mSkyboxTexture;

    std::pair<BufferGL *, int> mPositionBuffer;
    std::pair<BufferGL *, int> mNormalBuffer;
    std::pair<BufferGL *, int> mTexCoordBuffer;

    std::pair<BufferGL *, int> mTangentBuffer;
    std::pair<BufferGL *, int> mBiNormalBuffer;

    BufferGL *mIndicesBuffer;

  private:
    const ContextGL *mContextGL;
};

#endif  // FISHMODELGL_H
