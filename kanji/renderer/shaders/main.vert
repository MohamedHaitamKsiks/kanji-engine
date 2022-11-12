#version 450

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;

layout (location = 2) out vec3 outColor;

layout (push_constant) uniform Push {
    float transform[3][3];
} push;

void main() {
    mat3 transform;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++)
            transform[i][j] = push.transform[j][i];
    }
    vec2 translate = vec2(transform[0][2], transform[1][2]);
    gl_Position = vec4((vec3(position, 0.0) * transform).xy + translate, 0.0, 1.0);
    outColor = color;
}