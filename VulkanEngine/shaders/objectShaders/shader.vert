#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {

    mat4 model;
    mat4 view;
    mat4 proj;

} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 Normal;
layout(location = 1) out vec3 FragPos;

void main() {

    gl_Position			= ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
	FragPos				= vec3(ubo.model * vec4(inPosition, 1.0));
	Normal				= inNormal;

}