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
      mHistoryFPSCursor(0),
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
    mHistoryFPSCursor++;
	// Ignore first 5s.
    if (renderingTime < 5)
	{
       return;
	}

	// Increase time table if fps is not stable.
        if (NUM_FRAMES_TO_AVERAGE < MAX_TIME_TABLE && mHistoryFPSCursor >= MAX_TIME_TABLE &&
            variance() > VARIANCE_UPPER)
	{
        std::cout << " rendering time" << renderingTime << std::endl;
        int pre_avg = NUM_FRAMES_TO_AVERAGE;
        NUM_FRAMES_TO_AVERAGE *= 2;
        mTimeTable.resize(NUM_FRAMES_TO_AVERAGE);
        double cur = 0;
        for (int i = 0; i < pre_avg; i++)
	    {
            mTimeTable[pre_avg + i] = mTimeTable[i];
            cur += mTimeTable[i];
		}
        mTotalTime = cur * 2;
        mHistoryFPSCursor = 0;
    }

    if (mRecordFpsFrequencyCursor % logCount == 0)
    {
        mRecordFps.push_back(mAverageFPS);
        mRecordFpsFrequencyCursor = 0;
    }
    mRecordFpsFrequencyCursor++;
}

void FPSTimer::reset()
{
    NUM_FRAMES_TO_AVERAGE = INITTIMETABLE;
    mTimeTable.resize(NUM_FRAMES_TO_AVERAGE);
    double cur = 0;
    for(int i=0; i < NUM_FRAMES_TO_AVERAGE; i++)
	{
        cur += mTimeTable[i];
	}
    mTotalTime = cur;
    mHistoryFPSCursor = 0;
}

float FPSTimer::variance()
{
    float avg = 0.f;
    for (int i = 0; i < NUM_HISTORY_DATA; i++)
    {
        avg += mHistoryFPS[i];
	}
    avg /= NUM_HISTORY_DATA;

	float var = 0.f;
    for (int i = 0; i < NUM_HISTORY_DATA; i++)
	{
        var += pow(mHistoryFPS[i] - avg, 2);
	}
    var /= NUM_HISTORY_DATA;

	std::cout << var;
    return var;
}
