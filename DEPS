#
# Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#

gclient_gn_args_file = 'build/config/gclient_args.gni'
gclient_gn_args = [
]

vars = {
  'build_with_chromium': True, # suppress some nested 3rd-parties and gclient hooks by pretending to be Chromium
  'dawn_standalone': False,

  'android_git': 'https://android.googlesource.com',
  'chromium_git': 'https://chromium.googlesource.com',
  'dawn_git': 'https://dawn.googlesource.com',
  'github_git': 'https://github.com',
  'swiftshader_git': 'https://swiftshader.googlesource.com',

  # The following revisions are picked up from {chromium_git}/chromium/src/+/{chromium_rev}/DEPS
  'angle_rev': 'cc958e0e87034f591ea2a76d2809b13dad186c93',
  'dawn_rev': '4a486be696b44151bbda5e837a784560b2d9154f',
  'glfw_rev': '2de2589f910b1a85905f425be4d32f33cec092df',
  'glslang_rev': 'd253278f986574cb8e80ec9a4e84094ec1b25349',
  'nasm_rev': '19f3fad68da99277b2882939d3b2fa4c4b8d51d9',
  'spirv_cross_rev': '7778792aec717e982cbe07009e4db1de706f6619',
  'spirv_headers_rev': '3fdabd0da2932c276b25b9b4a988ba134eba1aa6',
  'spirv_tools_rev': 'df859f77dab392dbc78c213d36ba19216b3d068e',
  'swiftshader_rev': 'cda86eff64617e9b48167f1aa6a31bcdb823c10b',
  'tools_clang_dsymutil_ver': 'M56jPzDv1620Rnm__jTMYS62Zi8rxHVq7yw0qeBFEgkC',

  # Revisions of Chromium's subtree mirrors
  # To ensure these 3rd-party dependencies can nicely work together, they are rolled as the
  # following:
  # 1) go to {chromium_git}/chromium/src/+log/{chromium_rev}/path/to/dependency, and take down the
  #    latest Git revision and/or Gerrit Change-Id there
  # 2) go to {chromium_git}/chromium/src/path/to/dependency/+log, and find the revision with a
  #    matching Change-Id / GitOrigin-RevId / Cr-Mirrored-Commit trailer in the commit message
  'abseil_cpp_rev': 'e0e35eba3327467cdb7d7a2b5e462ccc795703a6',
  'build_rev': '1ef37ef29ef0c86bad47de3abfc42769dacf9298',
  'buildtools_rev': 'b00ad0af636401e5eb4b5d0ab01b65164dca1914',
  'googletest_rev': 'e3c3f879eee34ec81b1e562d8fecd207716d8945',
  'testing_rev': '91e579faaeb5d827e4643e14cb67c2ba29475418',
  'tools_clang_rev': 'faabb9a8ea407bb38928800b04a529971d228473',
  'zlib_rev': '7492de9a52f656b070f41968e39a6efa603590d5',

  # Revisions picked up from {chromium_git}/angle/angle/+/{angle_rev}/DEPS
  'angle_jsoncpp_rev': 'ec647b85b61f525a1a74e4da7477b0c5371c50f4',
  'angle_libjpeg_turbo_rev': '0241a1304fd183ee24fbdfe6891f18fdedea38f9',
  'angle_libpng_rev': '094e181e79a3d6c23fd005679025058b7df1ad6c',
  'angle_vulkan_memory_allocator_rev': '431d6e57284aeb08118ff428dfbd51c94342faa1',

  # Revisions picked up from {dawn_git}/dawn/+/{dawn_rev}/DEPS
  'dawn_jinja2_rev': 'b41863e42637544c2941b574c7877d3e1f663e25',
  'dawn_markupsafe_rev': '8f45f5cfa0009d2a70589bcda0349b8cb2b72783',
  'dawn_shaderc_rev': 'c626b912dc4568867b29c5b221df6aac75390ab5',
  'dawn_tint_rev': '5e7ef27ca7b1ddedd9a9553bc873e4034cd0c735',

  # Revisions of other 3rd-party dependencies directly used by Aquarium, whose rolling strategy
  # remains to be discussed
  'cxxopts_rev': '07f5cb24f1d75aad6c27eafd83863a78a37f16cb',
  'stb_rev': 'c7110588a4d24c4bb5155c184fbb77dd90b3116e',

  # For some 3rd-party dependencies, gclient will:
  # 1) fetch a dummy repo and check out the revision given by '*_rev'
  # 2) for the actual source, recurse into the dummy repo by overriding the revision variable in its
  #    DEPS file with the value here
  'googletest_revision': '4fe018038f87675c083d0cfb6a6b57c274fb1753',
  'jsoncpp_revision': '645250b6690785be60ab6780ce4b58698d884d11',
}

deps = {
  'build': {
    'url': '{chromium_git}/chromium/src/build.git@{build_rev}',
  },
  'buildtools': {
    'url': '{chromium_git}/chromium/src/buildtools.git@{buildtools_rev}',
  },
  'testing': {
    'url': '{chromium_git}/chromium/src/testing.git@{testing_rev}',
  },
  'third_party/SPIRV-Cross': {
    'url': '{github_git}/KhronosGroup/SPIRV-Cross.git@{spirv_cross_rev}',
  },
  'third_party/SPIRV-Headers': {
    'url': '{github_git}/KhronosGroup/SPIRV-Headers.git@{spirv_headers_rev}',
  },
  'third_party/SPIRV-Tools': {
    'url': '{github_git}/KhronosGroup/SPIRV-Tools.git@{spirv_tools_rev}',
  },
  'third_party/abseil-cpp': {
    'url': '{chromium_git}/chromium/src/third_party/abseil-cpp.git@{abseil_cpp_rev}',
  },
  'third_party/angle': {
    'url': '{chromium_git}/angle/angle.git@{angle_rev}',
  },
  'third_party/angle/third_party/jsoncpp': {
    'url': '{chromium_git}/chromium/src/third_party/jsoncpp.git@{angle_jsoncpp_rev}',
  },
  'third_party/angle/third_party/libjpeg_turbo': {
    'url': '{chromium_git}/chromium/deps/libjpeg_turbo.git@{angle_libjpeg_turbo_rev}',
  },
  'third_party/angle/third_party/libpng/src': {
    'url': '{android_git}/platform/external/libpng.git@{angle_libpng_rev}',
  },
  'third_party/angle/third_party/vulkan_memory_allocator': {
    'url': '{chromium_git}/external/github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git@{angle_vulkan_memory_allocator_rev}',
  },
  'third_party/cxxopts': {
    'url': '{github_git}/jarro2783/cxxopts.git@{cxxopts_rev}',
  },
  'third_party/dawn': {
    'url': '{dawn_git}/dawn.git@{dawn_rev}',
  },
  'third_party/dawn/third_party/jinja2': {
    'url': '{chromium_git}/chromium/src/third_party/jinja2.git@{dawn_jinja2_rev}',
  },
  'third_party/dawn/third_party/markupsafe': {
    'url': '{chromium_git}/chromium/src/third_party/markupsafe.git@{dawn_markupsafe_rev}',
  },
  'third_party/dawn/third_party/shaderc': {
    'url': '{github_git}/google/shaderc.git@{dawn_shaderc_rev}',
  },
  'third_party/dawn/third_party/tint': {
    'url': '{dawn_git}/tint.git@{dawn_tint_rev}',
  },
  'third_party/glfw': {
    'url': '{github_git}/glfw/glfw.git@{glfw_rev}',
  },
  'third_party/glslang': {
    'url': '{github_git}/KhronosGroup/glslang.git@{glslang_rev}',
  },
  'third_party/googletest': {
    'url': '{chromium_git}/chromium/src/third_party/googletest.git@{googletest_rev}',
  },
  'third_party/nasm': {
    'url': '{chromium_git}/chromium/deps/nasm.git@{nasm_rev}',
  },
  'third_party/stb': {
    'url': '{github_git}/nothings/stb.git@{stb_rev}',
  },
  'third_party/swiftshader': {
    'url': '{swiftshader_git}/SwiftShader.git@{swiftshader_rev}',
  },
  'third_party/zlib': {
    'url': '{chromium_git}/chromium/src/third_party/zlib.git@{zlib_rev}',
  },
  'tools/clang': {
    'url': '{chromium_git}/chromium/src/tools/clang.git@{tools_clang_rev}',
  },
  'tools/clang/dsymutil': {
    'packages': [
      {
        'package': 'chromium/llvm-build-tools/dsymutil',
        'version': '{tools_clang_dsymutil_ver}',
      },
    ],
    'condition': 'checkout_mac',
    'dep_type': 'cipd',
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
  'buildtools',
  'third_party/angle',
  'third_party/angle/third_party/jsoncpp',
  'third_party/dawn',
  'third_party/googletest',
]
