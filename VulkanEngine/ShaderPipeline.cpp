/*
*	File:		ShaderPipeline.cpp
*	
*/
#include "ShaderPipeline.hpp"

/*
*	Function:		ShaderPipeline(const std::string& vertShaderPath_, const std::string& fragShaderPath_)
*	Purpose:		Contructor
*	
*/
ShaderPipeline::ShaderPipeline(const std::string& vertShaderPath_, const std::string& fragShaderPath_) {

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

}

/*
*	Function:		ShaderModule getVertShaderModule()
*	Purpose:		Returns the vertex shader module
*
*/
ShaderModule ShaderPipeline::getVertShaderModule(void) {
	
	return vertShaderModule;

}

/*
*	Function:		ShaderModule getFragShaderModule()
*	Purpose:		Returns the fragment shader module
*
*/
ShaderModule ShaderPipeline::getFragShaderModule(void) {

	return fragShaderModule;

}
/*
*	Function:		VkPipelineShaderStageCreateInfo getVertStageInfo()
*	Purpose:		Returns the vertex shader stage create info
*
*/
VkPipelineShaderStageCreateInfo ShaderPipeline::getVertStageInfo(void) {

	return vertShaderStageInfo;

}

/*
*	Function:		VkPipelineShaderStageCreateInfo getFragStageInfo()
*	Purpose:		Returns the fragment shader stage create info
*
*/
VkPipelineShaderStageCreateInfo ShaderPipeline::getFragStageInfo(void) {
	
	return fragShaderStageInfo;

}

/*
*	Function:		~ShaderPipeline()
*	Purpose:		Default destructor
*	
*/
ShaderPipeline::~ShaderPipeline() {

		

}
