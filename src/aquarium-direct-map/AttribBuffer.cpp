//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// AttribBuffer.cpp: Implement AttribBuffer.

#include "AttribBuffer.h"

AttribBuffer::AttribBuffer(int numComponents,
                           const std::vector<float> &buffer,
                           int size,
                           const std::string &opt_type)
    : type(opt_type),
      bufferFloat(buffer),
      bufferUShort(),
      numComponents(numComponents),
      numElements(size / numComponents) {
}

AttribBuffer::AttribBuffer(int numComponents,
                           const std::vector<unsigned short> &buffer,
                           int size,
                           const std::string &opt_type)
    : type(opt_type),
      bufferFloat(),
      bufferUShort(buffer),
      numComponents(numComponents),
      numElements(size / numComponents) {
}
