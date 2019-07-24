//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FPSTimer.h: Define fps timer.

#pragma once
#ifndef FPS_TIMER
#define FPS_TIMER 1

#include <vector>

constexpr int NUM_FRAMES_TO_AVERAGE = 16;
constexpr int NUM_HISTORY_DATA = 100;

class FPSTimer
{
public:
  FPSTimer();

  void update(float elapsedTime);
  float getAverageFPS() const { return mAverageFPS; }
  float getInstantaneousFPS() const { return mInstantaneousFPS; }
  const float *getHistoryFps() const { return mHistoryFPS.data(); }
  const float *getHistoryFrameTime() const { return mHistoryFrameTime.data(); }

private:
  float mTotalTime;
  std::vector<float> mTimeTable;
  std::vector<float> mHistoryFPS;
  std::vector<float> mHistoryFrameTime;
  int mTimeTableCursor;
  float mInstantaneousFPS;
  float mAverageFPS;
};

#endif
