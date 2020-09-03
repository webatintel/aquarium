//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModel.cpp: Implement common functions of sun class of fish models.

#include "FishModel.h"

void FishModel::prepareForDraw()
{
  mFishPerOffset = 0;
  for (int i = 0; i < mName - MODELNAME::MODELSMALLFISHA; i++)
  {
    const Fish &fishInfo = fishTable[i];
    mFishPerOffset +=
        mAquarium->fishCount[fishInfo.modelName - MODELNAME::MODELSMALLFISHA];
  }

  const Fish &fishInfo = fishTable[mName - MODELNAME::MODELSMALLFISHA];
  mCurInstance =
      mAquarium->fishCount[fishInfo.modelName - MODELNAME::MODELSMALLFISHA];
}
