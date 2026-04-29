project('glad', 'c')

glad_lib = static_library(
  'glad',
  'src/gl.c',
  include_directories: include_directories('include'),
)
glad_dep = declare_dependency(
  link_with: glad_lib,
  include_directories: include_directories('include')
)
#
# glad_src = files(
#   'src/gl.c',
# )
#
# glad_dep = declare_dependency(
#   sources: glad_src,
#   include_directories: include_directories('include')
# )
