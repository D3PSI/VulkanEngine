#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 Normal;
layout(location = 1) in vec3 FragPos;
layout(location = 2) in vec3 fragColor;
layout(location = 3) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform LightingUniformBuffer {

    vec3 lightColor;
    vec3 objectColor;
	vec3 lightPos;
	vec3 viewPos;

} lbo;

void main() {

	float ambientStrength		= 0.1;
	vec3 ambient				= ambientStrength * lbo.lightColor;

	vec3 norm					= normalize(Normal);
	vec3 lightDir				= normalize(lbo.lightPos - FragPos);

	float diff					= max(dot(norm, lightDir), 0.0);
	vec3 diffuse				= diff * lbo.lightColor;

	float specularStrength		= 0.5;
	vec3 viewDir				= normalize(lbo.viewPos - FragPos);
	vec3 reflectDir				= reflect(-lightDir, norm);
	float spec					= pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular				= specularStrength * spec * lbo.lightColor;

	vec3 result					= (ambient + diffuse) * lbo.objectColor;

    outColor					= vec4(result, 1.0);

}