#
# Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#

solutions = [
  { "name"        : ".",
    "url"         : "https://github.com/webatintel/Aquarium.git",
    "deps_file"   : "DEPS",
    "managed"     : False,
    # Due to crbug.com/1215885, we have to use "\\" on Windows.
    "custom_deps" : {
      "third_party\\angle\\third_party\\VK-GL-CTS\\src": None,
      "third_party/angle/third_party/VK-GL-CTS/src": None,
    },
  },
]
