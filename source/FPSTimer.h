//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FPSTimer.h: Define fps timer.

#ifndef FPSTIMER_H
#define FPSTIMER_H

#include <chrono>
#include <ratio>
#include <vector>

constexpr int NUM_HISTORY_DATA = 100;
constexpr int NUM_FRAMES_TO_AVERAGE = 128;
constexpr int FPS_VALID_THRESHOLD = 5;

class FPSTimer {
public:
  // The unit of std::chrono::duration is second, while FPSTimer prefers
  // millisecond.
  typedef std::chrono::duration<
      std::chrono::steady_clock::duration::rep,
      std::ratio_multiply<std::chrono::steady_clock::duration::period,
                          std::kilo>>
      Duration;

  template <typename T>
  static Duration millisecondToDuration(const T &ms) {
    auto duration =
        std::chrono::duration_cast<std::chrono::steady_clock::duration>(
            std::chrono::duration<T, std::milli>(ms));
    return Duration(duration.count());
  }
  template <typename T>
  static T durationToMillisecond(const Duration &duration) {
    auto ms = std::chrono::duration_cast<std::chrono::duration<T, std::milli>>(
        std::chrono::steady_clock::duration(duration.count()));
    return ms.count();
  }

  FPSTimer();

  void update(Duration elapsedTime, Duration renderingTime, Duration testTime);
  double getAverageFPS() const { return mAverageFPS; }
  const float *getHistoryFps() const { return mHistoryFPS.data(); }
  const float *getHistoryFrameTime() const { return mHistoryFrameTime.data(); }
  int variance() const;

private:
  Duration mTotalTime;
  std::vector<Duration> mTimeTable;
  int mTimeTableCursor;

  std::vector<float> mHistoryFPS;
  std::vector<float> mHistoryFrameTime;
  std::vector<float> mLogFPS;

  double mAverageFPS;
};

#endif  // FPSTIMER_H
