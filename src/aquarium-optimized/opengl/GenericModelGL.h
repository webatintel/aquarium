//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// GenericModel.h: Defines generic model of OpenGL.

#ifndef GENERICMODELGL_H
#define GENERICMODELGL_H

#include "../Model.h"
#include "ContextGL.h"
#include "ProgramGL.h"

class GenericModelGL : public Model
{
  public:
    GenericModelGL(const ContextGL *context,
                   Aquarium *aquarium,
                   MODELGROUP type,
                   MODELNAME name,
                   bool blend);
    void prepareForDraw() override;
    void updatePerInstanceUniforms(const WorldUniforms &worldUniforms) override;
    void init() override;
    void draw() override;

    std::pair<float *, int> mWorldViewProjectionUniform;
    std::pair<float *, int> mWorldUniform;
    std::pair<float *, int> mWorldInverseTransposeUniform;

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

    std::pair<TextureGL *, int> mDiffuseTexture;
    std::pair<TextureGL *, int> mNormalTexture;

    std::pair<BufferGL *, int> mPositionBuffer;
    std::pair<BufferGL *, int> mNormalBuffer;
    std::pair<BufferGL *, int> mTexCoordBuffer;

    std::pair<BufferGL *, int> mTangentBuffer;
    std::pair<BufferGL *, int> mBiNormalBuffer;

    BufferGL *mIndicesBuffer;

  private:
    const ContextGL *mContextGL;
};

#endif  // GENERICMODELGL_H
