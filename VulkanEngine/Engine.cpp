/*
*	File:	Engine.cpp
*
*
*
*/
#include "Engine.hpp"

/*
*	Function:		void run()
*	Purpose:		Initializes the application
*
*/
void Engine::run() {

	initWindow();
	initVulkan();
	mainLoop();	
	cleanup();

}

/*	
*	Function:		void initWindow()
*	Purpose:		Initializes GLFW and its window
*
*/
void Engine::initWindow() {

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(
		
		width,
		height, 
		title.c_str(), 
		monitor, 
		nullptr
	
	);

}

/*
*	Function:		void initVulkan()
*	Purpose:		Initializes Vulkan
*
*/
void Engine::initVulkan() {

	createInstance();
	setupDebugCallback();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();

}

/*
*	Function:		VkResult createInstance()
*	Purpose:		Creates the VkInstance
*
*/
void Engine::createInstance() {

	if (enableValidationLayers && !checkValidationLayerSupport()) {
	
		logger.log(ERROR_LOG, "Validation layers requested, but not available!");
		throw std::runtime_error("Validation layers requested, but not available!");
	
	}

	VkApplicationInfo appInfo		= {};
	appInfo.sType					= VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext					= nullptr;
	appInfo.pApplicationName		= title.c_str();
	appInfo.applicationVersion		= VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName				= title.c_str();
	appInfo.engineVersion			= VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion				= VK_API_VERSION_1_0;

	uint32_t glfwExtensionCount		= 0;
	const char**					glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(
		
		nullptr,
		&extensionCount,
		nullptr
	
	);
	
	std::vector< VkExtensionProperties > extensions(extensionCount);
	
	vkEnumerateInstanceExtensionProperties(
		
		nullptr,
		&extensionCount,
		extensions.data()
	
	);

	std::cout << "Available extensions:" << std::endl;

	for (const auto& extension : extensions) {

		std::cout << "\t" << extension.extensionName << std::endl;

	}

	VkInstanceCreateInfo createInfo			= {};
	createInfo.sType						= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext						= nullptr;
	createInfo.flags						= 0;
	createInfo.pApplicationInfo				= &appInfo;
	if (enableValidationLayers) {
	
		createInfo.enabledLayerCount		= static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames		= validationLayers.data();
	
	}
	else {

		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;

	}

	auto reqExtensions = getRequiredExtensions();
	createInfo.enabledExtensionCount		= static_cast< uint32_t >(reqExtensions.size());
	createInfo.ppEnabledExtensionNames		= reqExtensions.data();

	std::cout << "Enabled extensions: " << std::endl;

	for (unsigned int i = 0; i < reqExtensions.size(); i++) {
	
		std::cout << "\t" << reqExtensions[i] << std::endl;
	
	}

	result = vkCreateInstance(
	
		&createInfo,
		nullptr,
		&instance
	
	);

	if (result != VK_SUCCESS) {
	
		logger.log(ERROR_LOG, "ERROR: " + result);
		throw std::runtime_error("Error: " + result);
	
	}

}

/*
*	Function:		void mainLoop()
*	Purpose:		Contains the main game loop
*
*/
void Engine::mainLoop() {

	while (!glfwWindowShouldClose(window)) {
	
		glfwPollEvents();
	
	}

}

/*
*	Function:		void cleanup()
*	Purpose:		Handles all destruction and cleaning up tasks
*
*/
void Engine::cleanup() {

	for (auto imageView : swapChainImageViews) {
	
		vkDestroyImageView(device, imageView, nullptr);
	
	}

	vkDestroySwapchainKHR(

		device,
		swapChain,
		nullptr

	);

	vkDestroyDevice(device,	nullptr);

	if (enableValidationLayers) {
	
		game::DestroyDebugUtilsMessengerEXT(
		
			instance,
			callback,
			nullptr

		);
	
	}

	vkDestroySurfaceKHR(
		
		instance,
		surface, 
		nullptr
	
	);

	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);

	glfwTerminate();

}

/*
*	Function:		bool checkValidationLayerSupport()
*	Purpose:		Checks whether the targer system supports validation layers
*
*/
bool Engine::checkValidationLayerSupport() {

	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector< VkLayerProperties > availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
	
		bool layerFound = false;
		
		for (const auto& layerProperties : availableLayers) {
		
			if (strcmp(layerName, layerProperties.layerName) == 0) {
			
				layerFound = true;
				break;

			}
		
		}

		if (!layerFound) {
		
			return false;
		
		}

	}

	return true;

}

/*
*	Function:		std::vector< const char* > getRequiredExtensions()
*	Purpose:		Fetches the required extensions
*
*/
std::vector< const char* > Engine::getRequiredExtensions() {

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector< const char* > extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
	
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	}

	return extensions;

}

/*
*	Function:		VKAPI_ATTR VkBool32 VKAPI_CALL Engine::debugCallback(
*
*						VkDebugUtilsMessageSeverityFlagBitsEXT			messageSeverity,
*						VkDebugUtilsMessageTypeFlagsEXT					messageType,
*						const VkDebugUtilsMessengerCallbackDataEXT*		pCallbackData,
*						void*											pUserData
*							
*						)
*	Purpose:		Callback function for VULKAN validation layers
*
*/
VKAPI_ATTR VkBool32 VKAPI_CALL Engine::debugCallback(

	VkDebugUtilsMessageSeverityFlagBitsEXT			messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT					messageType,
	const VkDebugUtilsMessengerCallbackDataEXT*		pCallbackData,
	void*											pUserData

	) {

	std::string msg = pCallbackData->pMessage;

	std::cerr << "Validation layer: " << msg << std::endl;

	return VK_FALSE;

}

/*
*	Function:		
*	Purpose:		
*
*/
void Engine::setupDebugCallback() {

	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo		= {};
	createInfo.sType									= VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.flags									= 0;
	createInfo.messageSeverity							= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType								= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback							= debugCallback;
	createInfo.pUserData								= nullptr;
	
	result = game::CreateDebugUtilsMessengerEXT(

		instance,
		&createInfo,
		nullptr,
		&callback

	);

	if (result != VK_SUCCESS) {
		
		logger.log(ERROR_LOG, "Failed to set up debug callback!" + result);
		throw std::runtime_error("Failed to set up debug callback!");
	
	}

}

/*
*	Function:		void pickPhysicalDevice()
*	Purpose:		Enumerates devices and picks the best one (GPU)
*
*/
void Engine::pickPhysicalDevice() {

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(
		
		instance,
		&deviceCount,
		nullptr
	
	);

	if (deviceCount == 0) {

		logger.log(ERROR_LOG, "Failed to fing GPU's with Vulkan support!");
		throw std::runtime_error("failed to find GPUs with Vulkan support!");

	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(
		
		instance, 
		&deviceCount, 
		devices.data()
	
	);

	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {

		logger.log(ERROR_LOG, "Failed to find a suitable GPU!");
		throw std::runtime_error("Failed to find a suitable GPU!");

	}

}

/*
*	Function:		bool isDeviceSuitable(VkPhysicalDevice device)
*	Purpose:		Checks whether the device supports VULKAN by giving each device a score and picking the highest one
*
*/
bool Engine::isDeviceSuitable(VkPhysicalDevice device) {

	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
	
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate;

}

/*
*	Function:		void createLogicalDevice()
*	Purpose:		Creates the logical device from previously selected physical device
*
*/
void Engine::createLogicalDevice() {

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	std::vector< VkDeviceQueueCreateInfo > queueCreateInfos;
	std::set< uint32_t > uniqueQueueFamilies		= {indices.graphicsFamily.value(), indices.presentFamily.value()};
	float queuePriority								= 1.0f;

	for (uint32_t queueFamily : uniqueQueueFamilies) {
	
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType					= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex		= queueFamily;
		queueCreateInfo.queueCount				= 1;
		queueCreateInfo.pQueuePriorities		= &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);

	}

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo createInfo			= {};
	createInfo.sType						= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos			= queueCreateInfos.data();
	createInfo.queueCreateInfoCount			= static_cast< uint32_t >(queueCreateInfos.size());
	createInfo.pEnabledFeatures				= &deviceFeatures;
	createInfo.enabledExtensionCount		= static_cast< uint32_t >(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames		= deviceExtensions.data();

	if (enableValidationLayers) {
	
		createInfo.enabledLayerCount		= static_cast< uint32_t >(validationLayers.size());
		createInfo.ppEnabledLayerNames		= validationLayers.data();

	}
	else {
	
		createInfo.enabledLayerCount		= 0;
	
	}

	if (vkCreateDevice(
	
			physicalDevice,
			&createInfo,
			nullptr,
			&device

		) != VK_SUCCESS) {
	
		logger.log(ERROR_LOG, "Failed to create logical device!");
		throw std::runtime_error("Failed to create logical device!");

	}

	vkGetDeviceQueue(
		
		device, 
		indices.graphicsFamily.value(),
		0,
		&graphicsQueue
	
	);

	vkGetDeviceQueue(
		
		device,
		indices.presentFamily.value(),
		0, 
		&presentQueue
	
	);

}

/*
*	Function:		void createSurface()
*	Purpose:		Creates the surface handle using GLFW
*
*/
void Engine::createSurface() {

	if (glfwCreateWindowSurface(
		
			instance, 
			window, 
			nullptr,
			&surface
	
		) != VK_SUCCESS) {

		logger.log(ERROR_LOG, "Failed to create window surface!");
		throw std::runtime_error("Failed to create window surface!");
	
	}

}

/*
	*	Function:		QueueFamilyIndices game::findQueueFamilies(VkPhysicalDevice device)
	*	Purpose:		Finds the indices for the correct queue families for the physical device
	*
	*/
QueueFamilyIndices Engine::findQueueFamilies(VkPhysicalDevice device) {

	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(

		device,
		&queueFamilyCount,
		nullptr

	);

	std::vector< VkQueueFamilyProperties > queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(

		device,
		&queueFamilyCount,
		queueFamilies.data()

	);

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(

			device,
			i,
			surface,
			&presentSupport

		);

		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {

			indices.graphicsFamily = i;

		}

		if (indices.isComplete()) {

			break;

		}

		if (queueFamily.queueCount > 0 && presentSupport) {

			indices.presentFamily = i;

		}

		i++;

	}

	return indices;

}

/*
*	Function:		bool checkDeviceExtensionSupport(VkPhysicalDevice device)
*	Purpose:		Check whether the target device has support for swapchains
*
*/
bool Engine::checkDeviceExtensionSupport(VkPhysicalDevice device) {

	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(
		
		device,
		nullptr,
		&extensionCount,
		nullptr
	
	);

	std::vector< VkExtensionProperties > availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(
		
		device,
		nullptr,
		&extensionCount,
		availableExtensions.data()
	
	);

	std::set< std::string > requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
	
		requiredExtensions.erase(extension.extensionName);
	
	}

	return requiredExtensions.empty();

}

/*
*	Function:		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device)
*	Purpose:		Querys the system for swapchain support
*
*/
SwapChainSupportDetails Engine::querySwapChainSupport(VkPhysicalDevice device) {
	
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
	
		device,
		surface,
		&details.capabilities
	
	);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(
		
		device,
		surface,
		&formatCount, 
		nullptr
	
	);

	if (formatCount != 0) {
	
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(

			device,
			surface,
			&formatCount,
			details.formats.data()
		
		);

	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(
	
		device,
		surface,
		&presentModeCount,
		nullptr

	);

	if (presentModeCount != 0) {
	
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(
		
			device,
			surface,
			&presentModeCount,
			details.presentModes.data()
		
		);
	
	}

	return details;

}

/*
*	Function:		VkSurfaceFormatKHR chooseSwapSurfaceFormat(cosnt std::vector< VkSurfaceFormatKHR >& availableFormats)
*	Purpose:		Finds the optimal surface format for the specified logical device
*
*/
VkSurfaceFormatKHR Engine::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {

	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
	
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	
	}

	for (const auto& availableFormat : availableFormats) {
	
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
		
			return availableFormat;
		
		}
	
	}

	return availableFormats[0];
}

/*
*	Function:		VkPresentModeKHR chooseSwapPresentMode(const std::vector< VkPresentModeKHR > availablePresentModes)
*	Purpose:		Enumerates best present mode for swapchain
*
*/
VkPresentModeKHR Engine::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes) {
	
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : availablePresentModes) {
	
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
		
			return availablePresentMode;
		
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
		
			bestMode = availablePresentMode;
		
		}
	
	}

	return bestMode;

}

/*
*	Function:		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
*	Purpose:		Enumerates best image size for swapchain
*
*/
VkExtent2D Engine::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {

	if (capabilities.currentExtent.width != std::numeric_limits< uint32_t >::max()) {
	
		return capabilities.currentExtent;
	
	}
	else {
	
		VkExtent2D actualExtent = {width, height};
		actualExtent.width = std::max(
			
			capabilities.minImageExtent.width, 
			std::min(capabilities.maxImageExtent.width, actualExtent.width)
		
		);
		actualExtent.height = std::max(
		
			capabilities.minImageExtent.width,
			std::min(capabilities.maxImageExtent.width, actualExtent.height)

		);
		
		return actualExtent;

	}

}

/*
*	Function:		void createSwapChain()
*	Purpose:		Puts together the swapchain with desired options
*
*/
void Engine::createSwapChain(void) {

	SwapChainSupportDetails swapChainSupport	= querySwapChainSupport(physicalDevice);
	
	VkSurfaceFormatKHR surfaceFormat			= chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode				= chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent							= chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
	
		imageCount = swapChainSupport.capabilities.maxImageCount;
	
	}

	VkSwapchainCreateInfoKHR createInfo	= {};
	createInfo.sType					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface					= surface;
	createInfo.minImageCount			= imageCount;
	createInfo.imageFormat				= surfaceFormat.format;
	createInfo.imageColorSpace			= surfaceFormat.colorSpace;
	createInfo.imageExtent				= extent;
	createInfo.imageArrayLayers			= 1;
	createInfo.imageUsage				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices			= findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[]		= {indices.graphicsFamily.value(), indices.presentFamily.value()};

	if (indices.graphicsFamily != indices.presentFamily) {
	
		createInfo.imageSharingMode			= VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount	= 2;
		createInfo.pQueueFamilyIndices		= queueFamilyIndices;

	}
	else {
	
		createInfo.imageSharingMode			= VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount	= 0;
		createInfo.pQueueFamilyIndices		= nullptr;
	
	}

	createInfo.preTransform			= swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha		= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode			= presentMode;
	createInfo.clipped				= VK_TRUE;
	createInfo.oldSwapchain			= VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(
			
		device,
		&createInfo,
		nullptr,
		&swapChain
	
	) != VK_SUCCESS) {
	
		logger.log(ERROR_LOG, "Failed to create Swapchain!");
		throw std::runtime_error("Failed to create Swapchain!");

	}

	vkGetSwapchainImagesKHR(
	
		device,
		swapChain,
		&imageCount,
		nullptr
	
	);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(
	
		device,
		swapChain,
		&imageCount,
		swapChainImages.data()
	
	);

	swapChainImageFormat		= surfaceFormat.format;
	swapChainImageColorSpace	= surfaceFormat.colorSpace;
	swapChainExtent				= extent;

}

/*
*	Function:		void createImageViews()
*	Purpose:		Creates the VkImageView vector for the swapchain
*
*/
void Engine::createImageViews(void) {

	swapChainImageViews.resize(swapChainImages.size());
	for (size_t i = 0; i < swapChainImages.size(); i++) {
	
		VkImageViewCreateInfo createInfo			= {};
		createInfo.sType							= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image							= swapChainImages[i];
		createInfo.viewType							= VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format							= swapChainImageFormat;
		createInfo.components.r						= VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g						= VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b						= VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a						= VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel	= 0;
		createInfo.subresourceRange.levelCount		= 1;
		createInfo.subresourceRange.baseArrayLayer	= 0;
		createInfo.subresourceRange.layerCount		= 1;

		if (vkCreateImageView(

			device,
			&createInfo,
			nullptr,
			&swapChainImageViews[i]

		) != VK_SUCCESS) {

			logger.log(ERROR_LOG, "Failed to create image views!");
			throw std::runtime_error("Failed to create image views!");

		}

	}

	

}

/*
*	Function:		VkShaderModule createShaderModule(const std::vector< char >& code)
*	Purpose:		Creates a shader module from a given byte array of SPIR-V compiled GLSL-shading code
*
*/
VkShaderModule Engine::createShaderModule(const std::vector<char>& code) {

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(
	
		device,
		&createInfo,
		nullptr,
		&shaderModule

	) !=VK_SUCCESS) {
	
		logger.log(ERROR_LOG, "Failed to create shader module!");
		throw std::runtime_error("Failed to create shader module!");
	
	}
	
	return shaderModule;

}

/*
*	Function:		void createGraphicsPipeline()
*	Purpose:		Generates the graphics pipeline for rendering
*
*/
void Engine::createGraphicsPipeline(void) {

	auto vertShaderCode = game::readFile("shaders/vert.spv");
	auto fragShaderCode = game::readFile("shaders/frag.spv");

	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;

	vertShaderModule = createShaderModule(vertShaderCode);
	fragShaderModule = createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo		= {};
	vertShaderStageInfo.sType								= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage								= VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module								= vertShaderModule;
	vertShaderStageInfo.pName								= "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo		= {};
	fragShaderStageInfo.sType								= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage								= VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module								= fragShaderModule;
	fragShaderStageInfo.pName								= "main";

	VkPipelineShaderStageCreateInfo shaderStages[]			= { vertShaderStageInfo, fragShaderStageInfo };

	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);

}
