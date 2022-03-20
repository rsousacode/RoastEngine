#!/bin/sh

VK_SHADERS_RD_PATH="src/vk/shaders"

if [ ! -d  "${VK_SHADERS_RD_PATH}/bin" ]; then
 mkdir $VK_SHADERS_RD_PATH/bin
fi

glslangValidator -V $VK_SHADERS_RD_PATH/VkShader.vert -o $VK_SHADERS_RD_PATH/bin/vert.spv
glslangValidator -V $VK_SHADERS_RD_PATH/VkShader.frag -o $VK_SHADERS_RD_PATH/bin/frag.spv

