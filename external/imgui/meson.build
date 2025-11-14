imgui_src = files(
  'src/imgui.cpp',
  'src/imgui_draw.cpp',
  'src/imgui_widgets.cpp',
  'src/imgui_tables.cpp',
  'src/imgui_demo.cpp',  # TODO remove
  'src/backends/imgui_impl_glfw.cpp',
  'src/backends/imgui_impl_opengl3.cpp'
)

imgui_dep = declare_dependency(
  sources: imgui_src,
  include_directories: include_directories('src')
)
