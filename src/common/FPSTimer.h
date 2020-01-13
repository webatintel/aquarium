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

constexpr int NUM_HISTORY_DATA = 100;
constexpr int NUM_FRAMES_TO_AVERAGE = 128;
constexpr int FPS_VALID_THRESHOLD   = 5;

class FPSTimer
{
public:
  FPSTimer();

  void update(double elapsedTime, double renderingTime, int logCount);
  double getAverageFPS() const { return mAverageFPS; }
  const float *getHistoryFps() const { return mHistoryFPS.data(); }
  const float *getHistoryFrameTime() const { return mHistoryFrameTime.data(); }
  int variance() const;

private:
  double mTotalTime;
  std::vector<double> mTimeTable;
  int mTimeTableCursor;

  std::vector<float> mHistoryFPS;
  std::vector<float> mHistoryFrameTime;

  double mAverageFPS;
};

#endif
