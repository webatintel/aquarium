//
// Copyright (c) 2020 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Behavior.h: Define base class for Behavior of Dawn backend.

#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include <string>

enum UNIFORMNAME : short;

class Behavior
{
public:
  Behavior() {}
  Behavior(int frame, std::string &op, int count)
      : mFrame(frame), mOp(op), mCount(count)
  {
  }

  int getFrame() const { return mFrame; }
  const std::string &getOp() const { return mOp; }
  int getCount() const { return mCount; }
  void setFrame(int frame) { mFrame = frame; }

private:
  int mFrame;
  std::string mOp;
  int mCount;
};

#endif  // BEHAVIOR_H
