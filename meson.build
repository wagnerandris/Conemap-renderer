project('Conemap-renderer', ['cpp', 'c'], default_options: ['cpp_std=c++20'])

# deps
gl_dep = dependency('GL')
glfw_dep = dependency('glfw3', fallback: ['glfw', 'glfw_dep'])
glm_dep = dependency('glm', fallback: ['glm', 'glm_dep'])
imgui_dep = dependency('imgui', fallback: ['imgui', 'imgui_dep'])
conemap_dep = dependency('conemap', fallback: ['Conemap', 'conemap_dep'])

subdir('subprojects/glad')

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
           dependencies : [gl_dep, glm_dep, glad_dep, glfw_dep, imgui_dep, conemap_dep],
           install : true)
