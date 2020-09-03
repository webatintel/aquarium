//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Main.cpp: Entry class of Aquarium.

#include "Aquarium.h"

int main(int argc, char **argv)
{
  Aquarium aquarium;
  if (!aquarium.init(argc, argv))
  {
    return -1;
  }

  aquarium.display();

  return 0;
}
