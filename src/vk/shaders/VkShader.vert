#version 450    // GLSL 4.5

layout(location = 0) in vec3 pos;
layout(location = 0) out vec3 fragCol;

layout(location = 1) in vec3 col;

void main() {
    gl_Position = vec4(pos, 1.0);
    fragCol = col;
}