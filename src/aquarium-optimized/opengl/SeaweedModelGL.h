//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// SeaweedModelGL.h: Define seaweed model of OpeGL.

#ifndef SEAWEEDMODELGL_H
#define SEAWEEDMODELGL_H

#include "../SeaweedModel.h"
#include "ContextGL.h"
#include "ProgramGL.h"

class SeaweedModelGL : public SeaweedModel
{
  public:
    SeaweedModelGL(const ContextGL *context,
                   Aquarium *aquarium,
                   MODELGROUP type,
                   MODELNAME name,
                   bool blend);
    void prepareForDraw() override;
    void updatePerInstanceUniforms(const WorldUniforms &worldUniforms) override;
    void init() override;
    void draw() override;

    void updateSeaweedModelTime(float time) override;

    std::pair<float *, int> mWorldUniform;

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
    std::pair<float, int> mTimeUniform;

    std::pair<TextureGL *, int> mDiffuseTexture;

    std::pair<BufferGL *, int> mPositionBuffer;
    std::pair<BufferGL *, int> mNormalBuffer;
    std::pair<BufferGL *, int> mTexCoordBuffer;

    BufferGL *mIndicesBuffer;

  private:
    const ContextGL *mContextGL;
};

#endif  // SEAWEEDMODELGL_H
