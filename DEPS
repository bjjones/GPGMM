use_relative_paths = True

gclient_gn_args_file = 'build/config/gclient_args.gni'
gclient_gn_args = [
  'build_with_chromium',
  'generate_location_tags',
]

vars = {
  'chromium_git': 'https://chromium.googlesource.com',
  'github_git': 'https://github.com',

  'gpgmm_standalone': True,
  'build_with_chromium': False,

  # Required by Chromium's //testing to generate directory->tags mapping used by ResultDB.
  'generate_location_tags': False,

  # GN CIPD package version.
  'gpgmm_gn_version': 'git_revision:bd99dbf98cbdefe18a4128189665c5761263bcfb',
}

deps = {
  # Dependencies required to use GN/Clang in standalone
  'build': {
    'url': '{chromium_git}/chromium/src/build@72ea026a7f1e2ab10f01ae21710f62fa8b6f91f1',
    'condition': 'gpgmm_standalone',
  },
  'buildtools': {
    'url': '{chromium_git}/chromium/src/buildtools@d866575165a824e166eed558162de3877f80fd9b',
    'condition': 'gpgmm_standalone',
  },
  'buildtools/clang_format/script': {
    'url': '{chromium_git}/external/github.com/llvm/llvm-project/clang/tools/clang-format.git@99803d74e35962f63a775f29477882afd4d57d94',
    'condition': 'gpgmm_standalone',
  },

  'buildtools/linux64': {
    'packages': [{
      'package': 'gn/gn/linux-amd64',
      'version': Var('gpgmm_gn_version'),
    }],
    'dep_type': 'cipd',
    'condition': 'gpgmm_standalone and host_os == "linux"',
  },
  'buildtools/mac': {
    'packages': [{
      'package': 'gn/gn/mac-${{arch}}',
      'version': Var('gpgmm_gn_version'),
    }],
    'dep_type': 'cipd',
    'condition': 'gpgmm_standalone and host_os == "mac"',
  },
  'buildtools/win': {
    'packages': [{
      'package': 'gn/gn/windows-amd64',
      'version': Var('gpgmm_gn_version'),
    }],
    'dep_type': 'cipd',
    'condition': 'gpgmm_standalone and host_os == "win"',
  },

  'buildtools/third_party/libc++/trunk': {
    'url': '{chromium_git}/external/github.com/llvm/llvm-project/libcxx.git@bac95aa2e352b64f784bea6fb2ffa4b34603b551',
    'condition': 'gpgmm_standalone',
  },
  'buildtools/third_party/libc++abi/trunk': {
    'url': '{chromium_git}/external/github.com/llvm/llvm-project/libcxxabi.git@db2a783a7d1ef0f0ef31da4b6e3de0c31fcfd93f',
    'condition': 'gpgmm_standalone',
  },
  'tools/clang': {
    'url': '{chromium_git}/chromium/src/tools/clang@3ac5eee82895df37b4c72d09dc1eca5373ec3094',
    'condition': 'gpgmm_standalone',
  },
  'tools/clang/dsymutil': {
    'packages': [
      {
        'package': 'chromium/llvm-build-tools/dsymutil',
        'version': 'M56jPzDv1620Rnm__jTMYS62Zi8rxHVq7yw0qeBFEgkC',
      }
    ],
    'condition': 'checkout_mac or checkout_ios',
    'dep_type': 'cipd',
  },

  # Testing, GTest and GMock
  'testing': {
    'url': '{chromium_git}/chromium/src/testing@c2eb333023f95f253a6f18c00f28517eb9c85a84',
    'condition': 'gpgmm_standalone',
  },
  'third_party/googletest': {
    'url': '{chromium_git}/external/github.com/google/googletest@9fbb6575035e3ac97ad9547d042c026a1296c705',
    'condition': 'gpgmm_standalone',
  },
  'third_party/vulkan-deps': {
    'url': '{chromium_git}/vulkan-deps@5f5bc7d1f02f06c7f3d207aa25f552b935106708',
    'condition': 'gpgmm_standalone',
  },
  # Dependency of //testing
  'third_party/catapult': {
    'url': '{chromium_git}/catapult.git@7628c078b5ab080404c28fb08d97f26c82f412e4',
    'condition': 'gpgmm_standalone',
  },
  'third_party/jsoncpp/source': {
    'url': '{chromium_git}/external/github.com/open-source-parsers/jsoncpp@8190e061bc2d95da37479a638aa2c9e483e58ec6',
    'condition': 'gpgmm_standalone',
  },
  # Fuzzing
  'third_party/libFuzzer/src': {
    'url': '{chromium_git}/chromium/llvm-project/compiler-rt/lib/fuzzer.git@debe7d2d1982e540fbd6bd78604bf001753f9e74',
    'condition': 'gpgmm_standalone',
  },
  'third_party/google_benchmark/src': {
    'url': '{chromium_git}/external/github.com/google/benchmark.git@e991355c02b93fe17713efe04cbc2e278e00fdbd',
    'condition': 'gpgmm_standalone',
  },
}

hooks = [
  # Pull the compilers and system libraries for hermetic builds
  {
    'name': 'sysroot_x86',
    'pattern': '.',
    'condition': 'checkout_linux and ((checkout_x86 or checkout_x64) and gpgmm_standalone)',
    'action': ['python3', 'build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=x86'],
  },
  {
    'name': 'sysroot_x64',
    'pattern': '.',
    'condition': 'checkout_linux and (checkout_x64 and gpgmm_standalone)',
    'action': ['python3', 'build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=x64'],
  },
  {
    # Update the Mac toolchain if possible, this makes builders use "hermetic XCode" which is
    # is more consistent (only changes when rolling build/) and is cached.
    'name': 'mac_toolchain',
    'pattern': '.',
    'condition': 'checkout_mac',
    'action': ['python3', 'build/mac_toolchain.py'],
  },
  {
    # Update the Windows toolchain if necessary. Must run before 'clang' below.
    'name': 'win_toolchain',
    'pattern': '.',
    'condition': 'checkout_win and gpgmm_standalone',
    'action': ['python3', 'build/vs_toolchain.py', 'update', '--force'],
  },
  {
    # Note: On Win, this should run after win_toolchain, as it may use it.
    'name': 'clang',
    'pattern': '.',
    'action': ['python3', 'tools/clang/scripts/update.py'],
    'condition': 'gpgmm_standalone',
  },
  {
    # Pull rc binaries using checked-in hashes.
    'name': 'rc_win',
    'pattern': '.',
    'condition': 'checkout_win and (host_os == "win" and gpgmm_standalone)',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-browser-clang/rc',
                '-s', 'build/toolchain/win/rc/win/rc.exe.sha1',
    ],
  },
  # Pull clang-format binaries using checked-in hashes.
  {
    'name': 'clang_format_win',
    'pattern': '.',
    'condition': 'host_os == "win"',
    'action': [ 'download_from_google_storage',
                '--no_resume',
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
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', 'buildtools/linux64/clang-format.sha1',
    ],
  },
  # Update build/util/LASTCHANGE.
  {
    'name': 'lastchange',
    'pattern': '.',
    'condition': 'gpgmm_standalone',
    'action': ['python3', 'build/util/lastchange.py',
               '-o', 'build/util/LASTCHANGE'],
  },
]

recursedeps = [
  # vulkan-deps provides vulkan-headers, spirv-tools, and gslang
  'third_party/vulkan-deps',
]
