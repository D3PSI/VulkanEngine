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
*						const std::string&										vertShaderPath_,
*						const std::string&										fragShaderPath_,
*						const VkPipelineVertexInputStateCreateInfo*				vertexInputInfo_,
*						const VkPipelineInputAssemblyStateCreateInfo*			inputAssembly_,
*						const VkPipelineViewportStateCreateInfo*				viewportState_,
*						const VkPipelineRasterizationStateCreateInfo*			rasterizer_,
*						const VkPipelineMultisampleStateCreateInfo*				multisampling_,
*						const VkPipelineDepthStencilStateCreateInfo*			depthStencil_,
*						const VkPipelineColorBlendStateCreateInfo*				colorBlending_,
*						const VkPipelineDynamicStateCreateInfo*					dynamicState_,
*						VkRenderPass											renderPass_,
*						uint32_t												subPass_,
*						VkPipeline												basePipeline_,
*						int32_t													basePipelineIndex_,
*						const std::vector< VkDescriptorSetLayoutBinding >*		bindings_,
*						VkDescriptorPool										descriptorPool_
*
*					)
*	Purpose:		Constructor
*
*/
Pipeline::Pipeline(

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
	const std::vector< VkDescriptorSetLayoutBinding >*		bindings_,
	VkDescriptorPool										descriptorPool_


) {

	vertShaderModule													= ShaderModule(vertShaderPath_);
	fragShaderModule													= ShaderModule(fragShaderPath_);

	vertShaderStageInfo													= {};
	vertShaderStageInfo.sType											= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage											= VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module											= vertShaderModule.getModule();
	vertShaderStageInfo.pName											= "main";

	fragShaderStageInfo													= {};
	fragShaderStageInfo.sType											= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage											= VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module											= fragShaderModule.getModule();
	fragShaderStageInfo.pName											= "main";

	VkPipelineShaderStageCreateInfo shaderStages[]						= { vertShaderStageInfo, fragShaderStageInfo };

	createDescriptorSets(bindings_, descriptorPool_);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo						= {};
	pipelineLayoutInfo.sType											= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount									= 1;
	pipelineLayoutInfo.pSetLayouts										= &descriptorSetLayout;

	if (vkCreatePipelineLayout(

		engine.device,
		&pipelineLayoutInfo,
		nullptr,
		&pipelineLayout

	) != VK_SUCCESS) {

		logger.log(ERROR_LOG, "Failed to create pipeline layout!");

	}

	VkGraphicsPipelineCreateInfo pipelineInfo							= {};
	pipelineInfo.sType													= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount												= 2;
	pipelineInfo.pStages												= shaderStages;
	pipelineInfo.pVertexInputState										= vertexInputInfo_;
	pipelineInfo.pInputAssemblyState									= inputAssembly_;
	pipelineInfo.pViewportState											= viewportState_;
	pipelineInfo.pRasterizationState									= rasterizer_;
	pipelineInfo.pMultisampleState										= multisampling_;
	pipelineInfo.pDepthStencilState										= depthStencil_;
	pipelineInfo.pColorBlendState										= colorBlending_;
	pipelineInfo.pDynamicState											= dynamicState_;
	pipelineInfo.layout													= pipelineLayout;
	pipelineInfo.renderPass												= renderPass_;
	pipelineInfo.subpass												= subPass_;
	pipelineInfo.basePipelineHandle										= basePipeline_;
	pipelineInfo.basePipelineIndex										= basePipelineIndex_;

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

	createUniformBuffer();

}

/*
*	Function:		void descriptorSetWrites(std::function< void() > descriptorWritesFunc_)
*	Purpose:		Creates the descriptor sets from a lambda function
*
*/
void Pipeline::descriptorSetWrites(std::function< void() > descriptorWritesFunc_) {

	descriptorWritesFunc_();

}

/*
*	Function:		void updateUBOs(uint32_t imageIndex_)
*	Purpose:		Updates uniforms and sends them to the shaders
*
*/
void Pipeline::updateUBOs(uint32_t imageIndex_) {

	void* data;

	vkMapMemory(

		engine.device,
		uniformBufferMemory[imageIndex_],
		0,
		sizeof(ubo),
		0,
		&data

	);

	memcpy(

		data,
		&ubo,
		sizeof(ubo)

	);

	vkUnmapMemory(engine.device, uniformBufferMemory[imageIndex_]);

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

	vkDestroyDescriptorSetLayout(
	
		engine.device,
		descriptorSetLayout,
		nullptr
	
	);

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

/*
*	Function:		void createDescriptorSets(const std::vector< VkDescriptorSetLayoutBinding >* bindings_, VkDescriptorPool descriptorPool_)
*	Purpose:		Creates descriptor sets for each uniform in the shader sets
*
*/
void Pipeline::createDescriptorSets(const std::vector< VkDescriptorSetLayoutBinding >* bindings_, VkDescriptorPool descriptorPool_) {

	VkDescriptorSetLayoutCreateInfo layoutInfo		= {};
	layoutInfo.sType								= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount							= static_cast< uint32_t >(bindings_->size());
	layoutInfo.pBindings							= bindings_->data();

	if (vkCreateDescriptorSetLayout(

		engine.device,
		&layoutInfo,
		nullptr,
		&descriptorSetLayout

	) != VK_SUCCESS) {

		logger.log(ERROR_LOG, "Failed to create descriptor set layout!");

	}

	std::vector< VkDescriptorSetLayout > layouts(engine.swapChainImages.size(), descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo			= {};
	allocInfo.sType									= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool						= descriptorPool_;
	allocInfo.descriptorSetCount					= static_cast< uint32_t >(engine.swapChainImages.size());
	allocInfo.pSetLayouts							= layouts.data();

	descriptorSets.resize(engine.swapChainImages.size());

	if (vkAllocateDescriptorSets(

		engine.device,
		&allocInfo,
		descriptorSets.data()

	) != VK_SUCCESS) {

		logger.log(ERROR_LOG, "Failed to allocate descriptor sets!");

	}

}

/*
*	Function:		void createUniformBuffer()
*	Purpose:		Creates the UBO's for the shader set
*
*/
void Pipeline::createUniformBuffer() {

	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(engine.swapChainImages.size());
	uniformBufferMemory.resize(engine.swapChainImages.size());

	for (size_t i = 0; i < engine.swapChainImages.size(); i++) {

		engine.createBuffer(

			bufferSize,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniformBuffers[i],
			uniformBufferMemory[i]

		);

	}

}
