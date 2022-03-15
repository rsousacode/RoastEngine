#!/bin/zsh
if [ ! -d "VkShaders/bin" ]; then
 mkdir VkShaders/bin
fi

glslangValidator -V VkShaders/RShader.vert -o VkShaders/bin/vert.spv
glslangValidator -V VkShaders/RShader.frag -o VkShaders/bin/frag.spv

