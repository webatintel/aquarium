//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FPSTimer.cpp: Implement fps timer.

#include "FPSTimer.h"

#include "AQUARIUM_ASSERT.h"

#include <cmath>

FPSTimer::FPSTimer()
    : mHistoryFPS(NUM_HISTORY_DATA, 1.0),
      mHistoryFrameTime(NUM_HISTORY_DATA, 100.0),
      mRecordFpsFrequencyCursor(0),
      mAverageFPS(0.0)
{}

void FPSTimer::update(double renderingTime, int fpsCount, int logCount)
{
    mAverageFPS = floor((1.0 / (renderingTime / fpsCount)) + 0.5);

    for (int i = 0; i < NUM_HISTORY_DATA; i++)
    {
        mHistoryFPS[i]       = mHistoryFPS[i + 1];
        mHistoryFrameTime[i] = mHistoryFrameTime[i + 1];
    }
    mHistoryFPS[NUM_HISTORY_DATA - 1]       = mAverageFPS;
    mHistoryFrameTime[NUM_HISTORY_DATA - 1] = 1000.0 / mAverageFPS;

    ASSERT(logCount != 0);
    if (mRecordFpsFrequencyCursor % logCount == 0)
    {
        mRecordFps.push_back(mAverageFPS);
        mRecordFpsFrequencyCursor = 0;
    }
    mRecordFpsFrequencyCursor++;
}
