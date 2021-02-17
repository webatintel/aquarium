//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FPSTimer.cpp: Implement fps timer.

#include "FPSTimer.h"

#include <cmath>
#include <iostream>

#include "AQUARIUM_ASSERT.h"

FPSTimer::FPSTimer()
    : mTotalTime(millisecondToDuration(NUM_FRAMES_TO_AVERAGE * 1000)),
      mTimeTable(NUM_FRAMES_TO_AVERAGE, millisecondToDuration(1000)),
      mTimeTableCursor(0),
      mHistoryFPS(NUM_HISTORY_DATA, 1.0f),
      mHistoryFrameTime(NUM_HISTORY_DATA, 100.0f),
      mAverageFPS(0.0) {
}

void FPSTimer::update(Duration elapsedTime,
                      Duration renderingTime,
                      Duration testTime) {
  mTotalTime += elapsedTime - mTimeTable[mTimeTableCursor];
  mTimeTable[mTimeTableCursor] = elapsedTime;

  ++mTimeTableCursor;
  if (mTimeTableCursor == NUM_FRAMES_TO_AVERAGE) {
    mTimeTableCursor = 0;
  }

  Duration frameTime = mTotalTime / NUM_FRAMES_TO_AVERAGE;
  mAverageFPS = floor(1000.0 / durationToMillisecond<double>(frameTime) + 0.5);

  for (int i = 0; i < NUM_HISTORY_DATA - 1; i++) {
    mHistoryFPS[i] = mHistoryFPS[i + 1];
    mHistoryFrameTime[i] = mHistoryFrameTime[i + 1];
  }
  mHistoryFPS[NUM_HISTORY_DATA - 1] = mAverageFPS;
  mHistoryFrameTime[NUM_HISTORY_DATA - 1] = 1000.0 / mAverageFPS;

  if (testTime - renderingTime > millisecondToDuration(5000) &&
      testTime - renderingTime < millisecondToDuration(25000)) {
    mLogFPS.push_back(mAverageFPS);
  }
}

int FPSTimer::variance() const {
  float avg = 0.f;

  for (size_t i = 0; i < mLogFPS.size(); i++) {
    avg += mLogFPS[i];
  }
  avg /= mLogFPS.size();

  float var = 0.f;
  for (size_t i = 0; i < mLogFPS.size(); i++) {
    var += pow(mLogFPS[i] - avg, 2);
  }
  var /= mLogFPS.size();

  if (var < FPS_VALID_THRESHOLD) {
    return ceil(avg);
  }

  return 0;
}
