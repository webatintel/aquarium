//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FPSTimer.cpp: Implement fps timer.

#include "FPSTimer.h"

#include <cmath>

FPSTimer::FPSTimer()
    : mTotalTime(static_cast<float>(NUM_FRAMES_TO_AVERAGE)),
      mTimeTable(NUM_FRAMES_TO_AVERAGE, 1.0f),
      mHistoryFPS(NUM_HISTORY_DATA, 1.0f),
      mHistoryFrameTime(NUM_HISTORY_DATA, 100.0f),
      mTimeTableCursor(0),
      mInstantaneousFPS(0.0f),
      mAverageFPS(0.0f)
{
}

void FPSTimer::update(float elapsedTime)
{
    mTotalTime += elapsedTime - mTimeTable[mTimeTableCursor];
    mTimeTable[mTimeTableCursor] = elapsedTime;

    ++mTimeTableCursor;
    if (mTimeTableCursor == NUM_FRAMES_TO_AVERAGE)
    {
        mTimeTableCursor = 0;
    }

    mInstantaneousFPS = floor(1.0f / elapsedTime + 0.5f);
    mAverageFPS = floor((1.0f / (mTotalTime / static_cast<float>(NUM_FRAMES_TO_AVERAGE))) + 0.5f);

    for (int i = 0; i < NUM_HISTORY_DATA; i++)
    {
        mHistoryFPS[i]       = mHistoryFPS[i + 1];
        mHistoryFrameTime[i] = mHistoryFrameTime[i + 1];
    }

    mHistoryFPS[NUM_HISTORY_DATA - 1]       = mAverageFPS;
    mHistoryFrameTime[NUM_HISTORY_DATA - 1] = 1000.0f / mAverageFPS;
}
