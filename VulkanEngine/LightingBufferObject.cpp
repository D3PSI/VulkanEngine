#pragma once
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <array>

struct LightingBufferObject {

	glm::vec3 lightColor;
	glm::vec3 objectColor;
	glm::vec3 lightPos;

};