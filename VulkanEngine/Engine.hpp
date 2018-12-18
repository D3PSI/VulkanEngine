/*
*	File:		Engine.hpp
*	
*
*/
#pragma once
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <string>
#include <map>
#include <vector>
#include <optional>

#include "Logger.hpp"
#include "QueueFamilyIndices.cpp"

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

namespace game {

	VkResult CreateDebugUtilsMessengerEXT(

		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pCallback

	);
	void DestroyDebugUtilsMessengerEXT(

		VkInstance instance,
		VkDebugUtilsMessengerEXT callback,
		const VkAllocationCallbacks* pAllocator

	);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	int init();

}

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
	VkPhysicalDevice							physicalDevice			= VK_NULL_HANDLE;
	VkDevice									device;
	VkQueue										graphicsQueue;
	VkDebugUtilsMessengerEXT					callback;

	void initWindow(void);
	void initVulkan(void);
	void createInstance(void);
	void mainLoop(void);
	void cleanup(void);
	bool checkValidationLayerSupport(void);
	std::vector< const char* > getRequiredExtensions(void);

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(

		VkDebugUtilsMessageSeverityFlagBitsEXT			messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT					messageType,
		const VkDebugUtilsMessengerCallbackDataEXT*		pCallbackData,
		void*											pUserData

	);
	void setupDebugCallback(void);
	void pickPhysicalDevice(void);
	bool isDeviceSuitable(VkPhysicalDevice device);
	void createLogicalDevice(void);

};

