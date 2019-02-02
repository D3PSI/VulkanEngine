/*	
*	File:		Pipeline.hpp
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

class Pipeline {
public:
	Pipeline();
	Pipeline(
	
		const std::string&										vertShaderPath_,
		const std::string&										fragShaderPath_,
		const VkPipelineVertexInputStateCreateInfo*				vertexInputInfo_,
		const VkPipelineInputAssemblyStateCreateInfo*			inputAssembly_,
		const VkPipelineViewportStateCreateInfo*				viewportState_,
		const VkPipelineRasterizationStateCreateInfo*			rasterizer_,
		const VkPipelineMultisampleStateCreateInfo*				multisampling_,
		const VkPipelineDepthStencilStateCreateInfo*			depthStencil_,
		const VkPipelineColorBlendStateCreateInfo*				colorBlending_,
		const VkPipelineDynamicStateCreateInfo*					dynamicState_,
		VkRenderPass											renderPass_,
		uint32_t												subPass_,
		VkPipeline												basePipeline_,
		int32_t													basePipelineIndex_,
		const std::vector< VkDescriptorSetLayoutBinding >*		bindings_
	
	);
	void bind(VkCommandBuffer commandBuffer_, VkDescriptorSet* descriptorSet_);
	void bindDescriptorSets(VkCommandBuffer commandBuffer_, VkDescriptorSet* descriptorSet_);
	void destroy(void);
	ShaderModule getVertShaderModule(void);
	ShaderModule getFragShaderModule(void);
	VkPipelineShaderStageCreateInfo getVertStageInfo(void);
	VkPipelineShaderStageCreateInfo getFragStageInfo(void);
	~Pipeline();
private:
	VkPipeline												pipeline;
	VkPipelineLayout										pipelineLayout;
	ShaderModule											vertShaderModule;
	ShaderModule											fragShaderModule; 
	VkPipelineShaderStageCreateInfo							vertShaderStageInfo;
	VkPipelineShaderStageCreateInfo							fragShaderStageInfo;
	VkDescriptorSetLayout									descriptorSetLayout;
	std::vector< VkDescriptorSet >							descriptorSets;
	std::vector< VkBuffer >									uniformBuffers;
	std::vector< VkDeviceMemory >							uniformBufferMemory;

	void createDescriptorSets(const std::vector< VkDescriptorSetLayoutBinding >* bindings_);
};

