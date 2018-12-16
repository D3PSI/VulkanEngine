/*
*	File:		Engine.hpp
*	
*
*/
#pragma once
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <string>
#include <vector>

#include "Logger.hpp"

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

class Engine
{
public:
	void run(void);
private:
	VkResult									result;
	Logger										logger;
	GLFWwindow*									window;
	const unsigned int							width					= 1280;
	const unsigned int							height					= 780;
	const std::string							title					= "VULKAN by D3PSI";
	GLFWmonitor*								monitor					= nullptr; 
	const std::vector< const char* >			validationLayers		= {
	
		"VK_LAYER_LUNARG_standard_validation"
	
	};
	VkInstance									instance;

	void initWindow(void);
	void initVulkan(void);
	void createInstance(void);
	void mainLoop(void);
	void cleanup(void);
	bool checkValidationLayerSupport(void);
	std::vector< const char* > getRequiredExtensions(void);

	/*
	*	Function:
	*	Purpose:		Callback function for VULKAN validation layers
	*
	*/
	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(

		VkDebugUtilsMessageSeverityFlagBitsEXT			messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT					messageType,
		const VkDebugUtilsMessengerCallbackDataEXT*		pCallbackData,
		void*											pUserData

		) {

		std::string msg = pCallbackData->pMessage;

		logger.log(ERROR_LOG, "Validation layer: " + msg);

		return VK_FALSE;

	}

};

