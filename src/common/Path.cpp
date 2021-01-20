//
// Copyright (c) 2021 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "Path.h"

#include <iterator>

#include "build/build_config.h"

#include "AQUARIUM_ASSERT.h"

#if defined(OS_WIN)
#include <Windows.h>
#include <cctype>
#include <vector>
#endif
#if defined(OS_MAC)
#include <mach-o/dyld.h>
#include <vector>
#endif
#if defined(OS_LINUX) && !defined(OS_CHROMEOS)
#include <limits.h>
#include <unistd.h>
#include <vector>
#endif

const std::string Path::kCurrentDirectory = ".";
const std::string Path::kParentDirectory = "..";
#if defined(OS_WIN)
const char Path::kSeparator = '\\';
const std::vector<char> Path::kAlternativeSeparators = {'/'};
#endif
#if defined(OS_MAC) || (defined(OS_LINUX) && !defined(OS_CHROMEOS))
const char Path::kSeparator = '/';
const std::vector<char> Path::kAlternativeSeparators = {};
#endif

Path::Path(const std::string &path) {
  auto begin = path.cbegin();
  auto end = path.cbegin();

  // Parse root name
#if defined(OS_WIN)
  if (end != path.cend() &&
      std::isalpha(static_cast<unsigned char>(*end))) {  // DOS path
    ++end;
    if (end != path.cend() && *end == ':') {
      ++end;
      mRootName = std::string(begin, end);
      begin = end;
    }
  } else if (end != path.cend() && isSeparator(*end)) {  // UNC path or DOS
                                                         // device path
    ++end;
    if (end != path.cend() && isSeparator(*end)) {
      ++end;
      if (end != path.cend() && !isSeparator(*end)) {
        ++end;
        while (end != path.cend() && !isSeparator(*end)) {
          ++end;
        }
        mRootName =
            std::string({kSeparator, kSeparator}) + std::string(begin + 2, end);
        begin = end;
      }
    }
  }
  end = begin;
#endif

  // Split the remaining by directory separator(s)
  // N.B. there is an empty filename before the leading or after the trailing
  // separator.
  while (begin != path.cend()) {
    begin = end;
    while (end != path.cend() && !isSeparator(*end)) {
      ++end;
    }
    mFilenames.emplace_back(begin, end);
    begin = end;
    while (end != path.cend() && isSeparator(*end)) {
      ++end;
    }
  }

  normalize();
}

Path::Path() {
}

Path::~Path() {
}

Path Path::getVoidPath() {
  return Path();
}

Path Path::getExecutablePath() {
  std::string path;
#if defined(OS_WIN)
  std::vector<TCHAR> tstr;
  std::vector<CHAR> str;
  size_t size;
  tstr.resize(MAX_PATH);  // start from a reasonable buffer size
  do {
    tstr.push_back({});  // increment the buffer size if it's not large enough
    size = GetModuleFileName(nullptr, tstr.data(), tstr.size());
  } while (size == tstr.size());
  tstr.resize(size);
#if defined(UNICODE) || defined(_UNICODE)
  size = WideCharToMultiByte(CP_UTF8, 0, tstr.data(), tstr.size(), nullptr, 0,
                             nullptr, nullptr);
  str.resize(size);
  size = WideCharToMultiByte(CP_UTF8, 0, tstr.data(), tstr.size(), str.data(),
                             str.size(), nullptr, nullptr);
  path = std::string(str.data(), str.size());
#else
  path = std::string(tstr.data(), tstr.size());
#endif
#elif defined(OS_MAC)
  std::vector<char> str;
  uint32_t size = 0;
  _NSGetExecutablePath(nullptr, &size);
  str.resize(size);
  _NSGetExecutablePath(str.data(), &size);
  path = std::string(str.data());
#elif defined(OS_LINUX) && !defined(OS_CHROMEOS)
  const std::string link = "/proc/self/exe";
  std::vector<char> str;
  size_t size;
  str.resize(PATH_MAX);  // start from a reasonable buffer size
  do {
    str.push_back({});  // increment the buffer size if it's not large enough
    size = readlink(link.c_str(), str.data(), str.size());
  } while (size == str.size());
  str.resize(size);
  path = std::string(str.data(), str.size());
#else
  ASSERT(false);
#endif
  return Path(path);
}

bool Path::isSeparator(char ch) {
  if (ch == kSeparator) {
    return true;
  }
  for (auto separator : kAlternativeSeparators) {
    if (ch == separator) {
      return true;
    }
  }
  return false;
}

bool Path::isAbsolute() const {
  auto ch = mRootName.cbegin();
#if defined(OS_WIN)
  if (ch != mRootName.cend() && std::isalpha(static_cast<unsigned char>(*ch))) {
    ++ch;
    if (ch != mRootName.cend() && *ch == ':') {
      ++ch;
      return mFilenames.size() > 1 && mFilenames.front() == "";
    }
  } else if (ch != mRootName.cend() && isSeparator(*ch)) {
    ++ch;
    if (ch != mRootName.cend() && isSeparator(*ch)) {
      ++ch;
      if (ch != mRootName.cend() && !isSeparator(*ch)) {
        ++ch;
        return true;
      }
    }
  }
#elif defined(OS_MAC) || (defined(OS_LINUX) && !defined(OS_CHROMEOS))
  if (ch == mRootName.cend()) {
    return mFilenames.size() > 1 && mFilenames.front() == "";
  }
#else
  ASSERT(false);
#endif
  return false;
}

bool Path::isRelative() const {
  auto ch = mRootName.cbegin();
#if defined(OS_WIN)
  if (ch == mRootName.cend()) {
    // The special void path ("") is excluded, although C++17 views it as a
    // relative path.
    return !mFilenames.empty();
  } else if (std::isalpha(static_cast<unsigned char>(*ch))) {
    ++ch;
    if (ch != mRootName.cend() && *ch == ':') {
      ++ch;
      return mFilenames.size() <= 1 || mFilenames.front() != "";
    }
  }
#elif defined(OS_MAC) || (defined(OS_LINUX) && !defined(OS_CHROMEOS))
  if (ch == mRootName.cend()) {
    // The special void path ("") is excluded.
    if (!mFilenames.empty()) {
      return mFilenames.size() == 1 || mFilenames.front() != "";
    }
  }
#else
  ASSERT(false);
#endif
  return false;
}

Path &Path::push(const Path &path) {
  if (path.isAbsolute()) {
    *this = path;
    return *this;
  } else {
    if (path.mRootName != "" && path.mRootName != mRootName) {
      *this = path;
      return *this;
    }

    if (path.mFilenames.size() > 1 && path.mFilenames.front() == "") {
      mFilenames = path.mFilenames;
      return *this;
    }

    for (const auto &filename : path.mFilenames) {
      if (!mFilenames.empty() && mFilenames.back() == "") {
        mFilenames.pop_back();  // strip the trailing empty filename
      }
      mFilenames.push_back(filename);
    }
    return *this;
  }
}

Path &Path::pop() {
  if (isAbsolute()) {
    if (!mFilenames.empty() && mFilenames.back() == "") {
      mFilenames.pop_back();  // strip the trailing empty filename first
    }
    if (mFilenames.empty() || mFilenames.back() == "") {
      // Current root directory
      mFilenames.push_back("");
    } else if (mFilenames.back() == kParentDirectory) {
      mFilenames.push_back(kParentDirectory);
    } else {
      // Path ending with a normal filename
      // No matter if this is the only remaining filename, the resulting
      // absolute path should always have a trailing directory separator.
      ASSERT(mFilenames.back() != kCurrentDirectory);
      mFilenames.pop_back();
      mFilenames.push_back("");
    }
    return *this;
  } else {
    if (!mFilenames.empty() && mFilenames.back() == "") {
      mFilenames.pop_back();  // strip the trailing empty filename first
    }
    if (mFilenames.empty() || mFilenames.back() == kParentDirectory) {
      mFilenames.push_back(kParentDirectory);
    } else if (mFilenames.back() == "") {
      // Current root directory
      mFilenames.push_back("");
    } else {
      // Path ending with a normal filename
      // When popping the last remaining filename, a trailing empty filename
      // helps to distinguish the internal representation of "." from that of
      // "".
      ASSERT(mFilenames.back() != kCurrentDirectory);
      mFilenames.pop_back();
      mFilenames.push_back("");
    }
    return *this;
  }
}

void Path::normalize() {
  std::vector<std::string> filenames;
  if (isAbsolute()) {
    for (auto it = mFilenames.cbegin(); it != mFilenames.cend(); ++it) {
      if (*it == "") {
        if (std::next(it) != mFilenames.cend()) {
          // Current root directory
          ASSERT(it == mFilenames.cbegin());
          filenames.push_back(*it);
        }
      } else if (*it == kParentDirectory) {
        if (!filenames.empty() && filenames.back() != "") {
          // Parent directory following a normal filename
          ASSERT(filenames.back() != kCurrentDirectory);
          ASSERT(filenames.back() != kParentDirectory);
          filenames.pop_back();
        }
      } else if (*it != kCurrentDirectory) {
        // Normal filename
        filenames.push_back(*it);
        continue;
      }
      // Special filenames fall through here. A trailing directory separator is
      // added on demand.
      if (std::next(it) == mFilenames.cend()) {
        ASSERT(filenames.empty() || filenames.back() != kCurrentDirectory);
        ASSERT(filenames.empty() || filenames.back() != kParentDirectory);
        filenames.push_back("");
      }
    }
  } else {
    for (auto it = mFilenames.cbegin(); it != mFilenames.cend(); ++it) {
      if (*it == "") {
        if (std::next(it) != mFilenames.cend()) {
          // Current root directory
          ASSERT(it == mFilenames.cbegin());
          filenames.push_back(*it);
        }
      } else if (*it == kParentDirectory) {
        if (filenames.empty() || filenames.back() == kParentDirectory) {
          filenames.push_back(*it);
        } else if (filenames.back() != "") {
          // Parent directory following a normal filename
          ASSERT(filenames.back() != kCurrentDirectory);
          filenames.pop_back();
        }
      } else if (*it != kCurrentDirectory) {
        // Normal filename
        filenames.push_back(*it);
        continue;
      }
      // Special filenames fall through here. A trailing directory separator is
      // conditionally added depending on current path ending.
      if (std::next(it) == mFilenames.cend()) {
        ASSERT(filenames.empty() || filenames.back() != kCurrentDirectory);
        if (filenames.empty() || filenames.back() != kParentDirectory) {
          filenames.push_back("");
        }
      }
    }
  }
  mFilenames = filenames;
}

Path::operator std::string() {
  normalize();

  std::string path = mRootName;
  if (isAbsolute()) {
    for (auto it = mFilenames.cbegin(); it != mFilenames.cend(); ++it) {
      if (std::next(it) == mFilenames.cend() || *it != "") {
        // Ensure a directory separator will be inserted after the root name
        // when necessary
        path += kSeparator;
      }
      path += *it;
    }
  } else {
    for (auto it = mFilenames.cbegin(); it != mFilenames.cend(); ++it) {
      if (it != mFilenames.cbegin()) {
        path += kSeparator;
      } else if (std::next(it) == mFilenames.cend() && *it == "") {
        // Prevent path to current directory from being converted to ""
        if (path == "") {
          path = kCurrentDirectory;
        }
      }
      path += *it;
    }
  }
  return path;
}
