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
      mAverageFPS(0.0)
{}

void FPSTimer::update(double elapsedTime, double renderingTime, int testTime)
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

    if (testTime - renderingTime > 5 && testTime - renderingTime < 25)
    {
        mLogFPS.push_back(mAverageFPS);
    }
}

int FPSTimer::variance() const
{
    float avg = 0.f;

    for (size_t i = 0; i < mLogFPS.size(); i++)
    {
        avg += mLogFPS[i];
    }
    avg /= mLogFPS.size();

    float var = 0.f;
    for (size_t i = 0; i < mLogFPS.size(); i++)
    {
        var += pow(mLogFPS[i] - avg, 2);
    }
    var /= mLogFPS.size();

    if (var < FPS_VALID_THRESHOLD)
    {
        return ceil(avg);
    }

    return 0;
}
