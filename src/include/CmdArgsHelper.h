//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// CmdArgsHelper.h: Define cmd arg strings.

#pragma once
#ifndef CMDARGSHELPER
#define CMDARGSHELPER 1

const char *cmdArgsStrAquarium = R"(Options and arguments:
--backend               : specifies running a certain backend, 'opengl', 'dawn_d3d12', 'dawn_vulkan', 'dawn_metal', 'dawn_opengl', 'angle', 'd3d12'.
--num-fish              : specifies how many fishes will be rendered.
--allow-instanced-draws : specifies rendering fishes by instanced draw. By default, fishes are rendered by individual draw.Instanced rendering is only supported on dawn and d3d12 backend now.
--enable-msaa           : Enable 4 samples MSAA. MSAA of angle backend is not supported now.
--disable-dynamic-buffer-offset : The path is to test individual draw by creating many binding groups on dawn backend. By default, dynamic buffer offset is enabled. This option is only supported on dawn backend.)";

const char *cmdArgsStrAquariumDirectMap = R"(Options and arguments:
--backend               : specifies running a certain backend, only 'opengl' is supported for aquarium-direct-map.
--num-fish              : specifies how many fishes will be rendered.
--enable-msaa           : Enable 4 samples MSAA.)";

#endif
