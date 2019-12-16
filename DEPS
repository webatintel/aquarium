#
# Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#

vars = {
  'chromium_git': 'https://chromium.googlesource.com',
  'github_git': 'https://github.com',
  'dawn_git': 'https://dawn.googlesource.com',
  'dawn_revision': 'ce8bf128ec1327b28d933a15e055bda286f31231',
  'imgui_git': 'https://github.com/ocornut',
  'imgui_revision': 'e16564e67a2e88d4cbe3afa6594650712790fba3',
  'angle_root': 'third_party/angle',
  'angle_revision': '6c824a1bc17b286b86cf05a0228ec549875351eb',
  'glslang_revision': '38b4db48f98c4e3a9cc405de3a76547b857e1c37',
  'build_revision': 'f3d0ca5f46b7b190dbbdc6be508ca11dd5c54302',
  'buildtools_revision': '74cfb57006f83cfe050817526db359d5c8a11628',
  'tools_clang_revision': '3605577b67603ec5776afcfada9e0ff4ea05cf0e',
  'spirv_tools_revision': '85f3e93d13f32d45bd7f9999aa51baddf2452aae',
  'jsoncpp_revision': '571788934b5ee8643d53e5d054534abbe6006168',
  'jsoncpp_source_revision': '645250b6690785be60ab6780ce4b58698d884d11',
  'vulkan-headers_revision':'5b44df19e040fca0048ab30c553a8c2d2cb9623e',
  'vulkan-validation-layers_revision':'9fba37afae13a11bd49ae942bf82e5bf1098e381',
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
  'third_party/SPIRV-Tools': {
    'url': '{chromium_git}/external/github.com/KhronosGroup/SPIRV-Tools@{spirv_tools_revision}',
  },
  'third_party/imgui': {
    'url': '{imgui_git}/imgui.git@{imgui_revision}',
  },
  'third_party/jsoncpp': {
    'url': '{chromium_git}/chromium/src/third_party/jsoncpp@{jsoncpp_revision}',
    'condition': 'dawn_standalone',
  },
  'third_party/jsoncpp/source': {
    'url' : '{chromium_git}/external/github.com/open-source-parsers/jsoncpp@{jsoncpp_source_revision}',
    'condition': 'dawn_standalone',
  },
  'third_party/vulkan-headers': {
    'url': '{chromium_git}/external/github.com/KhronosGroup/Vulkan-Headers@{vulkan-headers_revision}',
    'condition': 'dawn_standalone',
  },
  'third_party/vulkan-validation-layers': {
    'url': '{chromium_git}/external/github.com/KhronosGroup/Vulkan-ValidationLayers@{vulkan-validation-layers_revision}',
    'condition': 'dawn_standalone',
  },
}

hooks = [
  # Pull clang-format binaries using checked-in hashes.
  {
    'name': 'clang_format_win',
    'pattern': '.',
    'condition': 'host_os == "win"',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--platform=win32',
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', 'buildtools/win/clang-format.exe.sha1',
    ],
  },
  {
    'name': 'clang_format_mac',
    'pattern': '.',
    'condition': 'host_os == "mac"',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--platform=darwin',
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', 'buildtools/mac/clang-format.sha1',
    ],
  },
  {
    'name': 'clang_format_linux',
    'pattern': '.',
    'condition': 'host_os == "linux"',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--platform=linux*',
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', 'buildtools/linux64/clang-format.sha1',
    ],
  },
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
