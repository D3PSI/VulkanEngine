/*
*	File:		Engine.hpp
*	
*
*/
#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <map>
#include <algorithm>
#include <set>
#include <vector>
#include <optional>

#include "Logger.hpp"
#include "QueueFamilyIndices.cpp"
#include "SwapChainSupportDetails.cpp"

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
	static std::vector< char > readFile(const std::string& filename);
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
	const std::vector< const char* >			deviceExtensions = {
	
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	
	};
	VkInstance									instance;
	VkPhysicalDevice							physicalDevice			= VK_NULL_HANDLE;
	VkDevice									device;
	VkSurfaceKHR								surface;
	VkQueue										graphicsQueue;
	VkQueue										presentQueue;
	VkDebugUtilsMessengerEXT					callback;
	VkSwapchainKHR								swapChain;
	std::vector< VkImage >						swapChainImages;
	VkFormat									swapChainImageFormat;
	VkColorSpaceKHR								swapChainImageColorSpace;
	VkExtent2D									swapChainExtent;
	std::vector< VkImageView >					swapChainImageViews;

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
	void createSurface(void);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector< VkSurfaceFormatKHR >& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector< VkPresentModeKHR > availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilites);
	void createSwapChain(void);
	void createImageViews(void);
	VkShaderModule createShaderModule(const std::vector< char >& code);
	void createGraphicsPipeline(void);

};

