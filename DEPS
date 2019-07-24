#
# Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#

vars = {
  'chromium_git': 'https://chromium.googlesource.com',
  'github_git': 'https://github.com',
  'dawn_git': 'https://dawn.googlesource.com',
  'dawn_revision': '49aae0f3bde201d10f1b17d362b0d5b8f85dceb4',
  'imgui_git': 'https://github.com/ocornut',
  'imgui_revision': 'e16564e67a2e88d4cbe3afa6594650712790fba3',
  'angle_root': 'third_party/angle',
  'angle_revision': '6c824a1bc17b286b86cf05a0228ec549875351eb',
  'glslang_revision': '11805e41d992a9950a9f7f84c505bb7305fbd09f',
  'build_revision': '54ea0e7fd122348de2f73ac21d1b6eafb9b78969',
  'buildtools_revision': 'd5c58b84d50d256968271db459cd29b22bff1ba2',
  'tools_clang_revision': '210f1dc3ebf8504ae246d925e9110ec427eef43f',
  'spirv_tools_revision': '2090d7a2d26cb9bb0b8738f36a156ed3084a7ab0',
  'spirv_headers_revision': 'de99d4d834aeb51dd9f099baa285bd44fd04bb3d',
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
