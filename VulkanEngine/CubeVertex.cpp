#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <array>

struct CubeVertex {

	float pos[3];
	float normal[3];

	static VkVertexInputBindingDescription getBindingDescription() {
	
		VkVertexInputBindingDescription bindingDescription		= {};
		bindingDescription.binding								= 0;
		bindingDescription.stride								= sizeof(CubeVertex);
		bindingDescription.inputRate							= VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;

	}

	static std::array< VkVertexInputAttributeDescription, 2 > getAttributeDescriptions() {
	
		std::array< VkVertexInputAttributeDescription, 2 > attributeDescriptions = {};

		attributeDescriptions[0].binding			= 0;
		attributeDescriptions[0].location			= 0;
		attributeDescriptions[0].format				= VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset				= offsetof(CubeVertex, pos);
	
		attributeDescriptions[1].binding			= 0;
		attributeDescriptions[1].location			= 1;
		attributeDescriptions[1].format				= VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset				= offsetof(CubeVertex, normal);

		return attributeDescriptions;

	}

	bool operator==(const CubeVertex& other) const {

		return pos == other.pos;

	}

};