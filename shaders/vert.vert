#version 450

layout(binding = 0) uniform UniformBufferObject {
    vec3 pos;
    mat4 view;
    mat4 proj;
} camUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = camUBO.proj * camUBO.view * vec4(inPosition + camUBO.pos, 1.0);
    fragColor = inColor;
}