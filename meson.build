project('Triangle', 'c', 'cpp',
  version : '0.1',
  default_options : ['c_std=c11', 'cpp_std=c++11', 'warning_level=3'])

includeDir = include_directories('include')
glfw = dependency('glfw3')
subdir('src')

meson.get_compiler('cpp').find_library('opengl32')

exe = executable('Triangle', sources,
  install : true,
  include_directories: includeDir,
  dependencies: glfw,
  cpp_args: ['-std=c++20']
  )

test('basic', exe)
