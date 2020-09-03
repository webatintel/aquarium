//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// AttribBuffer.h: Define AttribBuffer Class. Store vertex attributes such as
// positions and indexes.

#ifndef ATTRIBBUFFER_H
#define ATTRIBBUFFER_H

#include <string>
#include <vector>

class AttribBuffer
{
public:
  AttribBuffer() {}
  AttribBuffer(int numComponents,
               const std::vector<float> &buffer,
               int size,
               const std::string &opt_type);
  AttribBuffer(int numComponents,
               const std::vector<unsigned short> &buffer,
               int size,
               const std::string &opt_type);

  int getNumComponents() const { return numComponents; }
  int getNumElements() const { return numElements; }

  const std::vector<float> &getBufferFloat() const { return bufferFloat; }
  const std::vector<unsigned short> &getBufferUShort() const
  {
    return bufferUShort;
  }
  const std::string &getType() const { return type; }

private:
  std::string type;
  std::vector<float> bufferFloat;
  std::vector<unsigned short> bufferUShort;
  int numComponents;
  int numElements;
};

#endif  // ATTRIBBUFFER_H
