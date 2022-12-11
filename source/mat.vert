
#version 450

//get info from vertex buffer
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

//output color
layout (location = 2) out vec3 outColor;

//transform
layout (push_constant) uniform Push {
    float transform[4][4];
} push;

void main() {
    mat4 transform;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++)
            transform[i][j] = push.transform[j][i];
    }
    gl_Position =  transform * vec4(position + vec3(0.2, 0.0, 0.0), 1.0);
    
    //outColor = texture(textureSampler uv).rgb;
    outColor = vec3(uv, 0.0);
}