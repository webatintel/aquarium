//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModel.h: Define fish model. Update fish specific uniforms.

#ifndef FISHMODEL_H
#define FISHMODEL_H

#include "Model.h"

class FishModel : public Model
{
  public:
    FishModel(MODELGROUP type, MODELNAME name, bool blend, Aquarium *aquarium)
        : Model(type, name, blend),
          mPreInstance(0),
          mCurInstance(0),
          mFishPerOffset(0),
          mAquarium(aquarium)
    {
    }

    virtual void updateFishPerUniforms(float x,
                                       float y,
                                       float z,
                                       float nextX,
                                       float nextY,
                                       float nextZ,
                                       float scale,
                                       float time,
                                       int index) = 0;
    void prepareForDraw();

  protected:
    int mPreInstance;
    int mCurInstance;
    int mFishPerOffset;

    Aquarium *mAquarium;
};

#endif  // FISHMODEL_H
