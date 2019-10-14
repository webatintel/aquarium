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
--buffer-mapping-async  : Upload uniforms by buffer mapping async for Dawn backend.
--disable-dynamic-buffer-offset : The path is to test individual draw by creating many binding groups on dawn backend. By default, dynamic buffer offset is enabled. This option is only supported on dawn backend.
--discrete-gpu          : Choose discrete gpu to render the application. This is only supported on Dawn and D3D12 backend.
--enable-instanced-draws : specifies rendering fishes by instanced draw. By default, fishes are rendered by individual draw.Instanced rendering is only supported on dawn and d3d12 backend now.
--enable-msaa           : Enable 4 samples MSAA. MSAA of angle backend is not supported now.
--enable-full-screen-mode       : Render aquarium in full screen mode instead of window mode.
--integrated-gpu        : Choose integrated gpu to render the application. This is only supported on Dawn and D3D12 backend.
--num-fish [count]      : specifies how many fishes will be rendered.
--record-fps-frequency [count]  : Run aquarium 10 min and print fps log when exit.)";

const char *cmdArgsStrAquariumDirectMap = R"(Options and arguments:
--backend               : specifies running a certain backend, only 'opengl' is supported for aquarium-direct-map.
--enable-msaa           : Enable 4 samples MSAA.
--num-fish              : specifies how many fishes will be rendered.)";

#endif
