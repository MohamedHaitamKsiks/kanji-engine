
#version 450

//ubo
layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

//get info from vertex buffer
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

//transform
//output color
layout (location = 2) out vec3 outColor;

//transform
layout (push_constant) uniform Push {
    mat4 transform;
} push;

//vertex main
void main() {
    //get transform
    mat4 transform = push.transform;
    //set vertex position
    gl_Position =  transpose(transform) * vec4(position, 1.0);

    //outColor = texture(textureSampler uv).rgb;
    outColor = vec3(uv, 1.0);
}