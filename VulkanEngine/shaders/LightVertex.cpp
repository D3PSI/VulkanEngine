#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <array>

struct LightVertex {

	glm::vec3 pos;

	static VkVertexInputBindingDescription getBindingDescriptio() {
	
		VkVertexInputBindingDescription bindingDescription		= {};
		bindingDescription.binding								= 0;
		bindingDescription.stride								= sizeof(LightVertex);
		bindingDescription.inputRate							= VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;

	}

	static std::array< VkVertexInputAttributeDescription, 1 > getAttributeDescriptions() {
	
		std::array< VkVertexInputAttributeDescription, 1 > attributeDescriptions = {};

		attributeDescriptions[0].binding			= 0;
		attributeDescriptions[0].location			= 0;
		attributeDescriptions[0].format				= VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset				= offsetof(LightVertex, pos);
	
		return attributeDescriptions;

	}

};