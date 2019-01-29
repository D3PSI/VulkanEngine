#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

#include "Logger.hpp"

extern Logger logger;

namespace game {

	extern VkDevice				globalDevice;

	std::vector< char > readFile(const std::string& fileName_);

}

class ShaderModule {
public:
	ShaderModule();
	ShaderModule(const std::string& fileName_);
	VkShaderModule getModule(void);
	~ShaderModule();
private:
	VkShaderModule				shaderModule;

};

