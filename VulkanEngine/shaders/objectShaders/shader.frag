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

layout(binding = 2) uniform MaterialUniformBuffer {

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;

} mat;

void main() {

	vec3 ambient				= mat.ambient * lbo.lightColor;

	vec3 norm					= normalize(Normal);
	vec3 lightDir				= normalize(lbo.lightPos - FragPos);

	float diff					= max(dot(norm, lightDir), 0.0);
	vec3 diffuse				= (diff * mat.diffuse) * lbo.lightColor;

	vec3 viewDir				= normalize(lbo.viewPos - FragPos);
	vec3 reflectDir				= reflect(-lightDir, norm);
	float spec					= pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
	vec3 specular				= (mat.specular * spec) * lbo.lightColor;

	vec3 result					= ambient + diffuse + specular;

    outColor					= vec4(result, 1.0);

}