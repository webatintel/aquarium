//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// SeaweedModel.h: Define seaweed model.

#ifndef SEAWEEDMODEL_H
#define SEAWEEDMODEL_H

#include "Model.h"

class SeaweedModel : public Model
{
public:
  SeaweedModel(MODELGROUP type, MODELNAME name, bool blend)
      : Model(type, name, blend)
  {
  }

  virtual void updateSeaweedModelTime(float time) = 0;
};

#endif  // SEAWEEDMODEL_H
