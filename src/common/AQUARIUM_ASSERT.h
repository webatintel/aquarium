//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef AQUARIUM_ASSERT_H
#define AQUARIUM_ASSERT_H

#include <cstdio>

//TODO(yizhou) : replace this ASSERT by the code template of ANGLE or Chromium
#ifndef NDEBUG
#define ASSERT(expression)                { \
        if (!(expression))                { \
            printf("Assertion(%s) failed: file \"%s\", line %d\n", \
                #expression, __FILE__, __LINE__); \
            abort();                        \
        }                                   \
    }
#else
#define ASSERT(expression) NULL;
#endif

#ifndef NDEBUG
#define SWALLOW_ERROR(expression)                { \
        if (!(expression))                { \
            printf("Assertion(%s) failed: file \"%s\", line %d\n", \
                #expression, __FILE__, __LINE__); \
        }                                         \
    }
#else
#define SWALLOW_ERROR(expression) expression
#endif

#endif  // AQUARIUM_ASSERT_H
