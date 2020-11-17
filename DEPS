#
# Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#

vars = {
  'chromium_git': 'https://chromium.googlesource.com',
  'github_git': 'https://github.com',
  'dawn_git': 'https://dawn.googlesource.com',
  'dawn_revision': '8575cb3ec773d43a86fe68eece70ae02cad6cfc5',
  'angle_root': 'third_party/angle',
  'angle_revision': '6c824a1bc17b286b86cf05a0228ec549875351eb',
  'glslang_revision': 'b5f003d7a3ece37db45578a8a3140b370036fc64',
  'build_revision': '6c8a5ae8ab293db1096590f234de8c04efb91606',
  'buildtools_revision': 'c8f5482fd0c9ec93bc4312a119982888c4df1c3d',
  'tools_clang_revision': '698732d5db36040c07d5cc5f9137fcc943494c11',
  'spirv_tools_revision': '85f3e93d13f32d45bd7f9999aa51baddf2452aae',
  'jsoncpp_revision': '571788934b5ee8643d53e5d054534abbe6006168',
  'jsoncpp_source_revision': '645250b6690785be60ab6780ce4b58698d884d11',
  'vulkan-headers_revision': '09531f27933bf04bffde9074acb302e026e8f181',
  'vulkan-validation-layers_revision': '5fab7f82a5486f8d36d6761dffcb3a149c9b0895',
  'cxxopts_revision': '07f5cb24f1d75aad6c27eafd83863a78a37f16cb',
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
    'url': '{github_git}/KhronosGroup/glslang.git@{glslang_revision}',
  },

  'third_party/googletest': {
    'url': '{github_git}/google/googletest.git@5ec7f0c4a113e2f18ac2c6cc7df51ad6afc24081',
  },
  'third_party/stb': {
    'url': '{github_git}/nothings/stb.git@c7110588a4d24c4bb5155c184fbb77dd90b3116e',
  },
  'third_party/glfw': {
    'url': '{github_git}/glfw/glfw.git@2de2589f910b1a85905f425be4d32f33cec092df',
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
    'url': '{github_git}/KhronosGroup/SPIRV-Tools.git@{spirv_tools_revision}',
  },
  'third_party/jsoncpp': {
    'url': '{chromium_git}/chromium/src/third_party/jsoncpp@{jsoncpp_revision}',
  },
  'third_party/jsoncpp/source': {
    'url' : '{github_git}/open-source-parsers/jsoncpp.git@{jsoncpp_source_revision}',
  },
  'third_party/vulkan-headers': {
    'url': '{github_git}/KhronosGroup/Vulkan-Headers.git@{vulkan-headers_revision}',
  },
  'third_party/vulkan-validation-layers': {
    'url': '{github_git}/KhronosGroup/Vulkan-ValidationLayers.git@{vulkan-validation-layers_revision}',
  },
  'third_party/cxxopts': {
    'url': '{github_git}/jarro2783/cxxopts.git@{cxxopts_revision}',
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
    'name': 'clang',
    'pattern': '.',
    'action': ['python', 'tools/clang/scripts/update.py'],
  },
  # Pull rc binaries using checked-in hashes.
  {
    'name': 'rc_win',
    'pattern': '.',
    'condition': 'checkout_win and host_os == "win"',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-browser-clang/rc',
                '-s', 'build/toolchain/win/rc/win/rc.exe.sha1',
    ],
  },
  {
    'name': 'rc_mac',
    'pattern': '.',
    'condition': 'checkout_win and host_os == "mac"',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-browser-clang/rc',
                '-s', 'build/toolchain/win/rc/mac/rc.sha1',
    ],
  },
  {
    'name': 'rc_linux',
    'pattern': '.',
    'condition': 'checkout_win and host_os == "linux"',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-browser-clang/rc',
                '-s', 'build/toolchain/win/rc/linux64/rc.sha1',
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
