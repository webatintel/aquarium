//
// Copyright (c) 2021 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef PATH_H
#define PATH_H

#include <string>
#include <vector>

class Path {
public:
  static Path getVoidPath();
  static Path getExecutablePath();

  explicit Path(const std::string &path);
  template <typename T>
  explicit Path(const T &path) : Path(std::string(path)) {}
  ~Path();
  bool isAbsolute() const;
  bool isRelative() const;
  Path &push(const Path &path);
  template <typename T>
  Path &push(const T &path) {
    return push(Path(path));
  }
  Path &pop();
  operator std::string();

private:
  static bool isSeparator(char ch);

  static const std::string kCurrentDirectory;
  static const std::string kParentDirectory;
  static const char kSeparator;
  static const std::vector<char> kAlternativeSeparators;

  Path();
  void normalize();

  std::string mRootName;
  std::vector<std::string> mFilenames;
};

#endif  // PATH_H
