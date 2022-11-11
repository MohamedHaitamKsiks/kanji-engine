#version 450

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;

layout (location = 2) out vec3 outColor;

layout (push_constant) uniform Push {
    vec2 offset;
} push;

void main() {
    gl_Position = vec4(position + push.offset, 0.0, 1.0);
    outColor = color;
}