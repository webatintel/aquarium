//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Buffer.h: Define abstract Buffer Class and buffer type enum.

#ifndef BUFFER_H
#define BUFFER_H

#include <vector>

class Buffer
{
  public:
    Buffer() {}
    Buffer(int numComponents, int numElements, const std::vector<float> &buffer){}
    Buffer(int numComponents, int numElements, const std::vector<short> &buffer){}
    virtual ~Buffer(){}
};

#endif  // BUFFER_H
