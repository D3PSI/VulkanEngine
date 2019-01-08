#pragma once
#include "VERSION.cpp"
#include <glm/glm.hpp>
struct UniformBufferObject {

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;

};