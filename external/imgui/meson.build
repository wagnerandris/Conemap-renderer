imgui_src = files(
  'imgui/imgui.cpp',
  'imgui/imgui_draw.cpp',
  'imgui/imgui_widgets.cpp',
  'imgui/imgui_tables.cpp',
  'imgui/imgui_demo.cpp',  # TODO remove
  'imgui/backends/imgui_impl_glfw.cpp',
  'imgui/backends/imgui_impl_opengl3.cpp'
)

imgui_dep = declare_dependency(
  sources: imgui_src,
  include_directories: include_directories('imgui')
)
