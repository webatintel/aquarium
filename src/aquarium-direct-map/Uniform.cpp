//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "Uniform.h"

Uniform::Uniform(const std::string &name,
                 GLenum type,
                 int length,
                 int size,
                 GLint index)
    : name(name), type(type), length(length), size(size), index(index)
{
}
