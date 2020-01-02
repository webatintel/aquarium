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
constexpr int INITTIMETABLE    = 64;
static int NUM_FRAMES_TO_AVERAGE = INITTIMETABLE;
constexpr int MAX_TIME_TABLE     = 1024;
constexpr int MIN_TIME_TABLE     = 32;
constexpr double VARIANCE_UPPER  = 3;
constexpr double VARIANCE_LOWER  = 0.01;

class FPSTimer
{
public:
  FPSTimer();

  void update(double elapsedTime, double renderingTime, int logCount);
  double getAverageFPS() const { return mAverageFPS; }
  const float *getHistoryFps() const { return mHistoryFPS.data(); }
  const float *getHistoryFrameTime() const { return mHistoryFrameTime.data(); }
  std::vector<float> &getRecordFps() { return mRecordFps; }
  void reset();

private:
  float variance();
  double mTotalTime;
  std::vector<double> mTimeTable;
  int mTimeTableCursor;

  std::vector<float> mHistoryFPS;
  std::vector<float> mHistoryFrameTime;
  int mHistoryFPSCursor;
  std::vector<float> mRecordFps;
  int mRecordFpsFrequencyCursor;
  double mAverageFPS;
};

#endif
