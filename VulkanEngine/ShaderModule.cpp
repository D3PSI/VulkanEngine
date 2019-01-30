/*
*	File:		ShaderModule.cpp
*
*/
#include "ShaderModule.hpp"
#include "Engine.hpp"

extern Engine				engine;
/*
*	Function:		ShaderModule()
*	Purpose:		Default constructor
*
*/
ShaderModule::ShaderModule() {



}

/*
*	Function:		ShaderModule(const std::string& fileName_)
*	Purpose:		Constructor with filename
*
*/
ShaderModule::ShaderModule(const std::string& fileName_) {

	std::vector< char > code = game::readFile(fileName_);

	VkShaderModuleCreateInfo createInfo			= {};
	createInfo.sType							= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize							= code.size();
	createInfo.pCode							= reinterpret_cast< const uint32_t* >(code.data());
	if (vkCreateShaderModule(

		engine.device,
		&createInfo,
		nullptr,
		&shaderModule

	) != VK_SUCCESS) {

		logger.log(ERROR_LOG, "Failed to create shader module!");

	}

}

/*
*	Function:		VkShaderModule getModule()
*	Purpose:		Returns the shader module
*	
*/
VkShaderModule ShaderModule::getModule(void) {

	return shaderModule;

}

/*
*	Function:		~ShaderModule()
*	Purpose:		Default destructor
*
*/
ShaderModule::~ShaderModule() {

	

}
