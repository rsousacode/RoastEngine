#version 450    // GLSL 4.5

// output color for vertex
layout(location = 0) out vec3 fragColor;

// Coordinate positions
vec3 positions[3] = vec3[] (
    vec3(0.0, -0.4, 0.4),
    vec3(0.4, 0.4, 0.0),
    vec3(-0.4, 0.4, 0.0)
);

// RGB COLORS
vec3 colors[3] = vec3[] (
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

// Todo: Change name main function
void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 1.0);
    fragColor = colors[gl_VertexIndex];
}