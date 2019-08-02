//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Program.h: Define base class for Programs of specific backends.

#ifndef PROGRAM_H
#define PROGRAM_H 1

#include <string>

enum UNIFORMNAME : short;

class Program
{
  public:
    Program() {}
    Program(const std::string &mVertexShader, const std::string &fragmentShader)
        : mVId(mVertexShader), mFId(fragmentShader)
    {
    }
    virtual ~Program() {}
    virtual void setProgram() {}

  protected:
    std::string mVId;
    std::string mFId;
};

#endif  // !PROGRAM_H
