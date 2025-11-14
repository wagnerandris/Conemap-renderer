project('Conemap renderer', 'cpp', default_options: ['cpp_std=c++20'])

# deps
gl_dep = dependency('GL')
glew_dep = dependency('GLEW')
glm_dep = dependency('glm')
glfw_dep = dependency('glfw3')

subdir('external/imgui')
conemap_dep = dependency('conemap', fallback: ['Conemap', 'conemap_dep'])

fs = import('fs')

# shaders
fs.copyfile('src/ConeStepMapping.vert', install : true, install_dir : '.')
fs.copyfile('src/RelaxedConeStepMapping.frag', install : true, install_dir : '.')

executable('Conemap renderer',
           'src/main.cpp',
           'src/scene.cpp',
           'src/ConeSteppingObject.cpp',
           'src/Camera.cpp',
           'src/Controls.cpp',
           'src/file_utils.cpp',
           dependencies : [gl_dep, glew_dep, glm_dep, glfw_dep, imgui_dep, conemap_dep],
           install : true)
