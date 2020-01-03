//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FPSTimer.cpp: Implement fps timer.

#include "FPSTimer.h"

#include "AQUARIUM_ASSERT.h"

#include <cmath>
#include <iostream>

FPSTimer::FPSTimer()
    : mTotalTime(static_cast<double>(NUM_FRAMES_TO_AVERAGE)),
      mTimeTable(NUM_FRAMES_TO_AVERAGE, 1.0f),
	  mTimeTableCursor(0),
      mHistoryFPS(NUM_HISTORY_DATA, 1.0f),
      mHistoryFrameTime(NUM_HISTORY_DATA, 100.0f),
      mRecordFpsFrequencyCursor(0),
      mAverageFPS(0.0)
{}

void FPSTimer::update(double elapsedTime, double renderingTime, int logCount)
{
    mTotalTime += elapsedTime - mTimeTable[mTimeTableCursor];
    mTimeTable[mTimeTableCursor] = elapsedTime;

    ++mTimeTableCursor;
    if (mTimeTableCursor == NUM_FRAMES_TO_AVERAGE)
    {
        mTimeTableCursor = 0;
    }

    mAverageFPS = floor((1.0f / (mTotalTime / static_cast<double>(NUM_FRAMES_TO_AVERAGE))) + 0.5);

    for (int i = 0; i < NUM_HISTORY_DATA; i++)
    {
        mHistoryFPS[i]       = mHistoryFPS[i + 1];
        mHistoryFrameTime[i] = mHistoryFrameTime[i + 1];
    }
    mHistoryFPS[NUM_HISTORY_DATA - 1]       = mAverageFPS;
    mHistoryFrameTime[NUM_HISTORY_DATA - 1] = 1000.0 / mAverageFPS;

    // Ignore first 5s.
    if (renderingTime < 5)
	{
       return;
	}

    if (mRecordFpsFrequencyCursor % logCount == 0)
    {
        mRecordFps.push_back(mAverageFPS);
        mRecordFpsFrequencyCursor = 0;
    }
    mRecordFpsFrequencyCursor++;
}

