# Copyright 2026 David Robillard <d@drobilla.net>
# SPDX-License-Identifier: 0BSD OR MIT

tool_suppressions = []
warning_level = get_option('warning_level')
if cpp.get_id() in ['clang', 'emscripten']
  if warning_level == 'everything'
    tool_suppressions += ['-Wno-c++98-compat-pedantic']
  endif
elif cpp.get_id() == 'gcc'
  tool_suppressions += ['-Wno-conditionally-supported']
endif

gendata = executable(
  'gendata',
  files('gendata.cpp'),
  cpp_args: cpp_suppressions + tool_suppressions,
  dependencies: [lv2_dep],
  implicit_include_directories: false,
  install: false,
)
