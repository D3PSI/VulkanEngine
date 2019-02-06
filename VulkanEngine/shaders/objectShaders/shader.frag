#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform LightingUniformBuffer {

    vec3 lightColor;
    vec3 objectColor;

} lbo;

void main() {

    outColor = vec4(lbo.lightColor * lbo.objectColor, 1.0);

}