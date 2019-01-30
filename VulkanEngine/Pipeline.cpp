/*
*	File:		Pipeline.cpp
*	
*/
#include "Pipeline.hpp"
#include "Engine.hpp"
extern Engine engine;

/*
*	Function:		Pipeline()
*	Purpose:		Default constructor
*	
*/
Pipeline::Pipeline() {



}

/*
*	Function:		Pipeline(
*
*						const std::string&									vertShaderPath_,
*						const std::string&									fragShaderPath_,
*						const VkPipelineVertexInputStateCreateInfo*			vertexInputInfo_,
*						const VkPipelineInputAssemblyStateCreateInfo*		inputAssembly_,
*						const VkPipelineViewportStateCreateInfo*			viewportState_,
*						const VkPipelineRasterizationStateCreateInfo*		rasterizer_,
*						const VkPipelineMultisampleStateCreateInfo*			multisampling_,
*						const VkPipelineDepthStencilStateCreateInfo*		depthStencil_,
*						const VkPipelineColorBlendStateCreateInfo*			colorBlending_,
*						const VkPipelineDynamicStateCreateInfo*				dynamicState_,
*						VkRenderPass										renderPass_,
*						uint32_t											subPass_,
*						VkPipeline											basePipeline_,
*						int32_t												basePipelineIndex_,
*						const VkDescriptorSetLayout*						descriptorSetLayout_
*
*					)
*	Purpose:		Constructor
*	
*/
Pipeline::Pipeline(
	
	const std::string&									vertShaderPath_,
	const std::string&									fragShaderPath_,
	const VkPipelineVertexInputStateCreateInfo*			vertexInputInfo_,
	const VkPipelineInputAssemblyStateCreateInfo*		inputAssembly_,
	const VkPipelineViewportStateCreateInfo*			viewportState_,
	const VkPipelineRasterizationStateCreateInfo*		rasterizer_,
	const VkPipelineMultisampleStateCreateInfo*			multisampling_,
	const VkPipelineDepthStencilStateCreateInfo*		depthStencil_,
	const VkPipelineColorBlendStateCreateInfo*			colorBlending_,
	const VkPipelineDynamicStateCreateInfo*				dynamicState_,
	VkRenderPass										renderPass_,
	uint32_t											subPass_,
	VkPipeline											basePipeline_,
	int32_t												basePipelineIndex_,
	const VkDescriptorSetLayout*						descriptorSetLayout_


) {

	vertShaderModule											= ShaderModule(vertShaderPath_);
	fragShaderModule											= ShaderModule(fragShaderPath_);

	vertShaderStageInfo											= {};
	vertShaderStageInfo.sType									= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage									= VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module									= vertShaderModule.getModule();
	vertShaderStageInfo.pName									= "main";

	fragShaderStageInfo											= {};
	fragShaderStageInfo.sType									= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage									= VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module									= fragShaderModule.getModule();
	fragShaderStageInfo.pName									= "main";

	VkPipelineShaderStageCreateInfo shaderStages[]				= { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineLayoutCreateInfo pipelineLayoutInfo				= {};
	pipelineLayoutInfo.sType									= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount							= 1;
	pipelineLayoutInfo.pSetLayouts								= descriptorSetLayout_;

	if (vkCreatePipelineLayout(

		engine.device,
		&pipelineLayoutInfo,
		nullptr,
		&pipelineLayout

	) != VK_SUCCESS) {

		logger.log(ERROR_LOG, "Failed to create pipeline layout!");

	}

	VkGraphicsPipelineCreateInfo pipelineInfo					= {};
	pipelineInfo.sType											= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount										= 2;
	pipelineInfo.pStages										= shaderStages;
	pipelineInfo.pVertexInputState								= vertexInputInfo_;
	pipelineInfo.pInputAssemblyState							= inputAssembly_;
	pipelineInfo.pViewportState									= viewportState_;
	pipelineInfo.pRasterizationState							= rasterizer_;
	pipelineInfo.pMultisampleState								= multisampling_;
	pipelineInfo.pDepthStencilState								= depthStencil_;
	pipelineInfo.pColorBlendState								= colorBlending_;
	pipelineInfo.pDynamicState									= dynamicState_;
	pipelineInfo.layout											= pipelineLayout;
	pipelineInfo.renderPass										= renderPass_;
	pipelineInfo.subpass										= subPass_;
	pipelineInfo.basePipelineHandle								= basePipeline_;
	pipelineInfo.basePipelineIndex								= basePipelineIndex_;

	if (vkCreateGraphicsPipelines(

		engine.device,
		VK_NULL_HANDLE,
		1,
		&pipelineInfo,
		nullptr,
		&pipeline

	) != VK_SUCCESS) {

		logger.log(ERROR_LOG, "Failed to create graphics pipeline!");

	}

}

/*
*	Function:		void bind(VkCommandBuffer commandBuffer_, VkDescriptorSet* descriptorSet)
*	Purpose:		Binds a pipeline
*
*/
void Pipeline::bind(VkCommandBuffer commandBuffer_, VkDescriptorSet* descriptorSet_) {

	vkCmdBindPipeline(

		commandBuffer_,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipeline

	);

	bindDescriptorSets(commandBuffer_, descriptorSet_);

}

/*
*	Function:		void bindDescriptorSets(VkCommandBuffer commandBuffer_, VkDescriptorSet* descriptorSet_)
*	Purpose:		Binds descriptor sets
*
*/
void Pipeline::bindDescriptorSets(VkCommandBuffer commandBuffer_, VkDescriptorSet* descriptorSet_) {

	vkCmdBindDescriptorSets(

		commandBuffer_,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipelineLayout,
		0,
		1,
		descriptorSet_,
		0,
		nullptr

	);

}

/*
*	Function:		void destroy()
*	Purpose:		Destroys all resources used by pipeline
*
*/
void Pipeline::destroy(void) {

	vkDestroyPipeline(

		engine.device,
		pipeline,
		nullptr

	);
	vkDestroyPipelineLayout(

		engine.device,
		pipelineLayout,
		nullptr

	);

}

/*
*	Function:		ShaderModule getVertShaderModule()
*	Purpose:		Returns the vertex shader module
*
*/
ShaderModule Pipeline::getVertShaderModule(void) {
	
	return vertShaderModule;

}

/*
*	Function:		ShaderModule getFragShaderModule()
*	Purpose:		Returns the fragment shader module
*
*/
ShaderModule Pipeline::getFragShaderModule(void) {

	return fragShaderModule;

}
/*
*	Function:		VkPipelineShaderStageCreateInfo getVertStageInfo()
*	Purpose:		Returns the vertex shader stage create info
*
*/
VkPipelineShaderStageCreateInfo Pipeline::getVertStageInfo(void) {

	return vertShaderStageInfo;

}

/*
*	Function:		VkPipelineShaderStageCreateInfo getFragStageInfo()
*	Purpose:		Returns the fragment shader stage create info
*
*/
VkPipelineShaderStageCreateInfo Pipeline::getFragStageInfo(void) {
	
	return fragShaderStageInfo;

}

/*
*	Function:		~Pipeline()
*	Purpose:		Default destructor
*	
*/
Pipeline::~Pipeline() {

		

}
