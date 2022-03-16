#!/bin/zsh
if [ ! -d "VkShaders/bin" ]; then
 mkdir VkShaders/bin
fi

glslangValidator -V src/shaders/VkShader.vert -o src/shaders/bin/vert.spv
glslangValidator -V src/shaders/VkShader.frag -o src/shaders/bin/frag.spv

