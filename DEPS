#
# Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#

vars = {
  'chromium_git': 'https://chromium.googlesource.com',
  'github_git': 'https://github.com',
  'dawn_git': 'https://dawn.googlesource.com',
  'dawn_revision': '3b05a6e03160c1516a7e565b5cebe69dc0325bd5',
  'imgui_git': 'https://github.com/ocornut',
  'imgui_revision': 'e16564e67a2e88d4cbe3afa6594650712790fba3',
  'angle_root': 'third_party/angle',
  'angle_revision': '6c824a1bc17b286b86cf05a0228ec549875351eb',
  'glslang_revision': '4b97a1108114107a8082a55e9e0721a40f9536d3',
  'build_revision': 'f3d0ca5f46b7b190dbbdc6be508ca11dd5c54302',
  'buildtools_revision': '74cfb57006f83cfe050817526db359d5c8a11628',
  'tools_clang_revision': '3605577b67603ec5776afcfada9e0ff4ea05cf0e',
  'spirv_tools_revision': '253806adc4c654c087ffe1e8e64b538d00989b16',
  'spirv_headers_revision': 'b252a50953ac4375cb1864e94f4b0234db9d215d',
  'shaderc_revision': '65adcb504d024bb29f0ad8279b6a215d5f679832',
}

deps = {
  # Dependencies required to use GN/Clang in standalone
  # This revision should be the same as the one in third_party/angle/DEPS
  'build': {
    'url': '{chromium_git}/chromium/src/build@{build_revision}',
  },
  # This revision should be the same as the one in third_party/angle/DEPS
  'buildtools': {
    'url': '{chromium_git}/chromium/src/buildtools@{buildtools_revision}',
  },
  # This revision should be the same as the one in third_party/angle/DEPS
  'tools/clang': {
    'url': '{chromium_git}/chromium/src/tools/clang@{tools_clang_revision}',
  },
  # This revision should be the same as the one in third_party/angle/DEPS
  'testing': {
    'url': '{chromium_git}/chromium/src/testing@083d633e752e7a57cbe62a468a06e51e28c49ee9',
  },
  # This revision should be the same as the one in third_party/angle/DEPS
  'third_party/glslang': {
    'url': '{chromium_git}/external/github.com/KhronosGroup/glslang@{glslang_revision}',
  },

  'third_party/googletest': {
    'url': '{chromium_git}/external/github.com/google/googletest@5ec7f0c4a113e2f18ac2c6cc7df51ad6afc24081',
  },
  'third_party/stb': {
    'url': '{github_git}/nothings/stb.git@c7110588a4d24c4bb5155c184fbb77dd90b3116e',
  },
  'third_party/glfw': {
    'url': '{chromium_git}/external/github.com/glfw/glfw@2de2589f910b1a85905f425be4d32f33cec092df',
    'condition': 'dawn_standalone',
  },
  'third_party/rapidjson': {
    'url': '{github_git}/Tencent/rapidjson.git',
  },
  'third_party/dawn': {
    'url': '{dawn_git}/dawn.git@{dawn_revision}',
  },
  'third_party/angle': {
    'url': '{chromium_git}/angle/angle.git@{angle_revision}',
  },
  'third_party/spirv-tools': {
    'url': '{chromium_git}/external/github.com/KhronosGroup/SPIRV-Tools@{spirv_tools_revision}',
  },
  'third_party/spirv-headers': {
    'url': '{chromium_git}/external/github.com/KhronosGroup/SPIRV-Headers@{spirv_headers_revision}',
  },
  'third_party/imgui': {
    'url': '{imgui_git}/imgui.git@{imgui_revision}',
  },
  'third_party/shaderc': {
    'url': '{chromium_git}/external/github.com/google/shaderc@{shaderc_revision}',
    'condition': 'dawn_standalone',
  },
}

hooks = [
  # Pull the compilers and system libraries for hermetic builds
  {
    'name': 'sysroot_x86',
    'pattern': '.',
    'condition': 'checkout_linux and ((checkout_x86 or checkout_x64))',
    'action': ['python', 'build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=x86'],
  },
  {
    'name': 'sysroot_x64',
    'pattern': '.',
    'condition': 'checkout_linux and (checkout_x64)',
    'action': ['python', 'build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=x64'],
  },
  {
    # Update the Windows toolchain if necessary. Must run before 'clang' below.
    'name': 'win_toolchain',
    'pattern': '.',
    'condition': 'checkout_win',
    'action': ['python', 'build/vs_toolchain.py', 'update', '--force'],
  },
  {
    # Update the Mac toolchain if necessary.
    'name': 'mac_toolchain',
    'pattern': '.',
    'condition': 'checkout_mac',
    'action': ['python', '{angle_root}/build/mac_toolchain.py'],
  },
  {
    # Note: On Win, this should run after win_toolchain, as it may use it.
    'name': 'clang',
    'pattern': '.',
    'action': ['python', 'tools/clang/scripts/update.py'],
    'condition': 'dawn_standalone',
  },
  {
    # Pull rc binaries using checked-in hashes.
    'name': 'rc_win',
    'pattern': '.',
    'condition': 'checkout_win and (host_os == "win")',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-browser-clang/rc',
                '-s', 'build/toolchain/win/rc/win/rc.exe.sha1',
    ],
  },
  # Update build/util/LASTCHANGE.
  {
    'name': 'lastchange',
    'pattern': '.',
    'action': ['python', 'build/util/lastchange.py',
               '-o', 'build/util/LASTCHANGE'],
  },
]

recursedeps = [
  # buildtools provides clang_format, libc++, and libc++abi
  'buildtools',
  'third_party/dawn',
  'third_party/angle',
]
