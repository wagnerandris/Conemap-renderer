project('Conemap-renderer', 'cpp', default_options: ['cpp_std=c++20'])

# deps
gl_dep = dependency('GL')
glew_dep = dependency('GLEW')
glfw_dep = dependency('glfw3')

glm_inc = include_directories('external/glm')
subdir('external/imgui')
conemap_dep = dependency('conemap', fallback: ['Conemap', 'conemap_dep'])

fs = import('fs')

# shaders
fs.copyfile('src/ConeStepping.vert', install : true, install_dir : '.')
fs.copyfile('src/RelaxedConeStepping.frag', install : true, install_dir : '.')

executable('Conemap-renderer',
           'src/main.cpp',
           'src/Scene.cpp',
           'src/ConeSteppingObject.cpp',
           'src/Camera.cpp',
           'src/Controls.cpp',
           'src/file_utils.cpp',
           dependencies : [gl_dep, glew_dep, glfw_dep, imgui_dep, conemap_dep],
           include_directories: glm_inc,
           install : true)
