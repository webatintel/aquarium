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

  # Revisions of Chromium's subtree mirrors
  # To ensure these 3rd-party dependencies can nicely work together, they are rolled as the
  # following:
  # 1) go to {chromium_git}/chromium/src/+log/{chromium_rev}/path/to/dependency, and take down the
  #    latest Git revision and/or Gerrit Change-Id there
  # 2) go to {chromium_git}/chromium/src/path/to/dependency/+log, and find the revision with a
  #    matching Change-Id / GitOrigin-RevId / Cr-Mirrored-Commit trailer in the commit message
  'build_revision': '068753b02547cf5af14ebb732f8af65309382124',
  'buildtools_revision': 'fba2905150c974240f14aa5334c3e5c93f873032',
  'clang_revision': 'd663c274354764e7eeb74e5566d4c259f1259f12',
  'googletest_gn_revision': '17bbed2084d3127bd7bcd27283f18d7a5861bea8',
  'jinja2_revision': '6906af9d94ae10e895af4b7d07a34206e8de1424',
  'jsoncpp_gn_revision': '9d9b186fefa51728b0b257a95fe1b31c98fb58d1',
  'libpng_revision': 'eee63db00955234e51954839d2dd47db529d05ce',
  'markupsafe_revision': '0944e71f4b2cb9a871bcbe353f95e889b64a611a',
  'testing_revision': '73587aa74e599b605c911674c0c306260f0c385b',
  'zlib_revision': 'd0e636edaa95e2e04f56b84014f6b5f799acf0f0',

  # The following revisions are picked up from {chromium_git}/src/DEPS
  'angle_revision': '363d9cb6536886ca4e2e75bef58a32af2bc9bb0a',
  'catapult_revision': 'edf4e0e88f4e636bd9fc249d9b26c589ead820ec',
  'dawn_revision': '7f7f8130415a8a6ad950b9dd987a89cdaa2f88a1',
  'dsymutil_revision': 'M56jPzDv1620Rnm__jTMYS62Zi8rxHVq7yw0qeBFEgkC',
  'glfw_revision': '2de2589f910b1a85905f425be4d32f33cec092df',
  'gn_version': 'git_revision:39a87c0b36310bdf06b692c098f199a0d97fc810',
  'libcxx_revision': '8fa87946779682841e21e2da977eccfb6cb3bded',
  'libcxxabi_revision': 'da3e6cbc62dd1e3df45f522ba313d1c581867094',
  'libjpeg_turbo_revision': 'b7bef8c05b7cdb1a038ae271a2c2b6647af4c879',
  'libunwind_revision': 'c8d0fb870a8d5325c80f5588d7421ecb5faa658d',
  'nasm_revision': '19f3fad68da99277b2882939d3b2fa4c4b8d51d9',
  'swiftshader_revision': 'f504d9ef5615cad38cb983b02b5dda6f9b3b613b',
  'vulkan_deps_revision': 'f7463350a894cb26ad7952af498cf109f8194180',
  'vulkan_memory_allocator_revision': '732a76d9d3c70d6aa487216495eeb28518349c3a',

  # For some 3rd-party dependencies, gclient will:
  # 1) fetch a dummy repo and check out the revision given by '*_revision'
  # 2) for the actual source, recurse into the dummy repo by overriding the revision variable in its
  #    DEPS file with the value here
  'googletest_revision': 'a3460d1aeeaa43fdf137a6adefef10ba0b59fe4b',
  'jsoncpp_revision': '9059f5cad030ba11d37818847443a53918c327b1',

  # Revisions of other 3rd-party dependencies directly used by Aquarium, whose rolling strategy
  # remains to be discussed
  'cxxopts_revision': '07f5cb24f1d75aad6c27eafd83863a78a37f16cb',
  'stb_revision': 'c7110588a4d24c4bb5155c184fbb77dd90b3116e',
}

deps = {
  'build': {
    'url': '{chromium_git}/chromium/src/build.git@{build_revision}',
  },
  'buildtools': {
    'url': '{chromium_git}/chromium/src/buildtools.git@{buildtools_revision}',
  },
  'buildtools/linux64': {
    'packages': [
      {
        'package': 'gn/gn/linux-amd64',
        'version': '{gn_version}',
      },
    ],
    'dep_type': 'cipd',
    'condition': 'host_os == "linux"',
  },
  'buildtools/mac': {
    'packages': [
      {
        'package': 'gn/gn/mac-${{arch}}',
        'version': '{gn_version}',
      },
    ],
    'dep_type': 'cipd',
    'condition': 'host_os == "mac"',
  },
  'buildtools/third_party/libc++/trunk': {
    'url': '{chromium_git}/external/github.com/llvm/llvm-project/libcxx.git@{libcxx_revision}',
  },
  'buildtools/third_party/libc++abi/trunk': {
    'url': '{chromium_git}/external/github.com/llvm/llvm-project/libcxxabi.git@{libcxxabi_revision}',
  },
  'buildtools/third_party/libunwind/trunk': {
    'url': '{chromium_git}/external/github.com/llvm/llvm-project/libunwind.git@{libunwind_revision}',
  },
  'buildtools/win': {
    'packages': [
      {
        'package': 'gn/gn/windows-amd64',
        'version': '{gn_version}',
      },
    ],
    'dep_type': 'cipd',
    'condition': 'host_os == "win"',
  },
  'testing': {
    'url': '{chromium_git}/chromium/src/testing.git@{testing_revision}',
  },
  'third_party/angle': {
    'url': '{chromium_git}/angle/angle.git@{angle_revision}',
  },
  'third_party/angle/third_party/libpng/src': {
    'url': '{android_git}/platform/external/libpng.git@{libpng_revision}',
  },
  'third_party/catapult':
    Var('chromium_git') + '/catapult.git' + '@' + Var('catapult_revision'),
  'third_party/cxxopts': {
    'url': '{github_git}/jarro2783/cxxopts.git@{cxxopts_revision}',
  },
  'third_party/dawn': {
    'url': '{dawn_git}/dawn.git@{dawn_revision}',
  },
  'tools/clang/dsymutil': {
    'packages': [
      {
        'package': 'chromium/llvm-build-tools/dsymutil',
        'version': '{dsymutil_revision}',
      },
    ],
    'condition': 'checkout_mac',
    'dep_type': 'cipd',
  },
  'third_party/glfw': {
    'url': '{github_git}/glfw/glfw.git@{glfw_revision}',
  },
  'third_party/googletest': {
    'url': '{chromium_git}/chromium/src/third_party/googletest.git@{googletest_gn_revision}',
  },
  'third_party/jinja2': {
    'url': '{chromium_git}/chromium/src/third_party/jinja2.git@{jinja2_revision}',
  },
  'third_party/jsoncpp': {
    'url': '{chromium_git}/chromium/src/third_party/jsoncpp.git@{jsoncpp_gn_revision}',
  },
  'third_party/libjpeg_turbo': {
    'url': '{chromium_git}/chromium/deps/libjpeg_turbo.git@{libjpeg_turbo_revision}',
  },
  'third_party/markupsafe': {
    'url': '{chromium_git}/chromium/src/third_party/markupsafe.git@{markupsafe_revision}',
  },
  'third_party/nasm': {
    'url': '{chromium_git}/chromium/deps/nasm.git@{nasm_revision}',
  },
  'third_party/stb': {
    'url': '{github_git}/nothings/stb.git@{stb_revision}',
  },
  'third_party/swiftshader': {
    'url': '{swiftshader_git}/SwiftShader.git@{swiftshader_revision}',
  },
  'third_party/vulkan-deps': {
    'url': '{chromium_git}/vulkan-deps@{vulkan_deps_revision}',
  },
  'third_party/vulkan_memory_allocator': {
    'url': '{chromium_git}/external/github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git@{vulkan_memory_allocator_revision}',
  },
  'third_party/zlib': {
    'url': '{chromium_git}/chromium/src/third_party/zlib.git@{zlib_revision}',
  },
  'tools/clang': {
    'url': '{chromium_git}/chromium/src/tools/clang.git@{clang_revision}',
  },
}

hooks = [
  {
    'name': 'clang',
    'pattern': '.',
    'action': ['python', 'tools/clang/scripts/update.py'],
  },
  # Pull clang-format binaries using checked-in hashes.
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
    # Update LASTCHANGE.
    'name': 'lastchange',
    'pattern': '.',
    'action': ['python3', 'build/util/lastchange.py',
               '-o', 'build/util/LASTCHANGE'],
  },
  # Pull rc binaries using checked-in hashes.
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
  # Pull the compilers and system libraries for hermetic builds
  {
    'name': 'sysroot_x64',
    'pattern': '.',
    'condition': 'checkout_linux and (checkout_x64)',
    'action': ['python', 'build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=x64'],
  },
  {
    'name': 'sysroot_x86',
    'pattern': '.',
    'condition': 'checkout_linux and ((checkout_x86 or checkout_x64))',
    'action': ['python', 'build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=x86'],
  },
]

recursedeps = [
  'third_party/angle',
  'third_party/dawn',
  'third_party/googletest',
  'third_party/jsoncpp',
  'third_party/vulkan-deps',
]
