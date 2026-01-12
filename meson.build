project(
  'dactyl',
  'cpp',
  version: '0.1.0',
  default_options: [
    'cpp_std=c++17',
    'warning_level=2',
  ],
)

dactyl_inc = include_directories('src', 'src/util')

subdir('src/util')

glm_inc = include_directories('glm/glm')
glm_dep = declare_dependency(
  include_directories: glm_inc,
)

exe = executable(
  'dactyl',
  ['src/dactyl.cc', 'src/key_data.cc'],
  include_directories: dactyl_inc,
  dependencies: [glm_dep],
  link_with: [util_lib],
  install: false,
)

