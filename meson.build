project('Conemap renderer', 'cpp')

# deps
gl_dep = dependency('GL')
glew_dep = dependency('GLEW')
glm_dep = dependency('glm')
glfw_dep = dependency('glfw3')

subdir('external/imgui')

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
           dependencies : [gl_dep, glew_dep, glm_dep, glfw_dep, imgui_dep],
           install : true)
