project('Conemap renderer', 'cpp')

gl_dep = dependency('GL')
glew_dep = dependency('GLEW')
glm_dep = dependency('glm')
glfw_dep = dependency('glfw3')

subdir('external/imgui')

fs = import('fs')

#fs.copyfile('src/Conemap.vert', install : true, install_dir : '.')
#fs.copyfile('src/Conemap.frag', install : true, install_dir : '.')

executable('Conemap renderer',
           'src/main.cpp',
           dependencies : [gl_dep, glew_dep, glm_dep, glfw_dep, imgui_dep],
           install : true)
