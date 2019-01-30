/*	
*	File:		ShaderPipeline.hpp
*
*/
#pragma once
#include <Windows.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

#include "Vertex.cpp"
#include "ShaderModule.hpp"

class ShaderPipeline {
public:
	ShaderPipeline();
	ShaderPipeline(const std::string& vertShaderPath_, const std::string& fragShaderPath_);
	ShaderModule getVertShaderModule(void);
	ShaderModule getFragShaderModule(void);
	VkPipelineShaderStageCreateInfo getVertStageInfo(void);
	VkPipelineShaderStageCreateInfo getFragStageInfo(void);
	~ShaderPipeline();
private:
	ShaderModule										vertShaderModule;
	ShaderModule										fragShaderModule; 
	VkPipelineShaderStageCreateInfo						vertShaderStageInfo;
	VkPipelineShaderStageCreateInfo						fragShaderStageInfo;
};

