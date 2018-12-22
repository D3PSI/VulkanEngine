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

	logger.log(EVENT_LOG, "Initializing GLFW-window...");
	initWindow();
	logger.log(EVENT_LOG, "Successfully initialized GLFW-window!");
	logger.log(EVENT_LOG, "Initializing Vulkan...");
	initVulkan();
	logger.log(EVENT_LOG, "Successfully initialized VULKAN!");
	logger.log(EVENT_LOG, "Entering main game loop...");
	mainLoop();	
	logger.log(EVENT_LOG, "Cleaning up...");
	cleanup();
	logger.log(EVENT_LOG, "Successfully cleaned up!");

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
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandPool();
	createCommandBuffers();
	createSemaphores();

}

/*
*	Function:		VkResult createInstance()
*	Purpose:		Creates the VkInstance
*
*/
void Engine::createInstance() {

	if (enableValidationLayers && !checkValidationLayerSupport()) {
	
		logger.log(ERROR_LOG, "Validation layers requested, but not available!");
	
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
		renderFrame();

	}

	vkDeviceWaitIdle(device);

}

/*
*	Function:		void cleanup()
*	Purpose:		Handles all destruction and cleaning up tasks
*
*/
void Engine::cleanup() {

	vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);

	vkDestroyCommandPool(
	
		device,
		commandPool,
		nullptr
	
	);

	for (auto framebuffer : swapChainFramebuffers) {

		vkDestroyFramebuffer(
			
			device, 
			framebuffer, 
			nullptr
		
		);

	}

	vkDestroyPipeline(
	
		device,
		graphicsPipeline,
		nullptr
	
	);

	vkDestroyPipelineLayout(
	
		device,
		pipelineLayout,
		nullptr

	);

	vkDestroyRenderPass(
	
		device,
		renderPass,
		nullptr
	
	);

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

	VkPipelineShaderStageCreateInfo vertShaderStageInfo				= {};
	vertShaderStageInfo.sType										= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage										= VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module										= vertShaderModule;
	vertShaderStageInfo.pName										= "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo				= {};
	fragShaderStageInfo.sType										= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage										= VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module										= fragShaderModule;
	fragShaderStageInfo.pName										= "main";

	VkPipelineShaderStageCreateInfo shaderStages[]					= { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo			= {};
	vertexInputInfo.sType											= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount					= 0;
	vertexInputInfo.pVertexBindingDescriptions						= nullptr;
	vertexInputInfo.vertexAttributeDescriptionCount					= 0;
	vertexInputInfo.pVertexAttributeDescriptions					= nullptr;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly			= {};
	inputAssembly.sType												= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology											= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable							= VK_FALSE;

	VkViewport viewport												= {};
	viewport.x														= 0.0f;
	viewport.y														= 0.0f;
	viewport.width													= (float)swapChainExtent.width;
	viewport.height													= (float)swapChainExtent.height;
	viewport.minDepth												= 0.0f;
	viewport.maxDepth												= 1.0f;

	VkRect2D scissor												= {};
	scissor.offset													= {0, 0};
	scissor.extent													= swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState					= {};
	viewportState.sType												= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount										= 1;
	viewportState.pViewports										= &viewport;
	viewportState.scissorCount										= 1;
	viewportState.pScissors											= &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer				= {};
	rasterizer.sType												= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable										= VK_FALSE;
	rasterizer.rasterizerDiscardEnable								= VK_FALSE;
	rasterizer.polygonMode											= VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth											= 1.0f;
	rasterizer.cullMode												= VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace											= VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable										= VK_FALSE;
	rasterizer.depthBiasConstantFactor								= 0.0f;
	rasterizer.depthBiasClamp										= 0.0f;
	rasterizer.depthBiasSlopeFactor									= 0.0f;

	VkPipelineMultisampleStateCreateInfo multisampling				= {};
	multisampling.sType												= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable								= VK_FALSE;
	multisampling.rasterizationSamples								= VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading									= 1.0f;
	multisampling.pSampleMask										= nullptr;
	multisampling.alphaToCoverageEnable								= VK_FALSE;
	multisampling.alphaToOneEnable									= VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment		= {};
	colorBlendAttachment.colorWriteMask								= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable								= VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor						= VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor						= VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp								= VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor						= VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor						= VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp								= VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending				= {};
	colorBlending.sType												= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable										= VK_FALSE;
	colorBlending.logicOp											= VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount									= 1;
	colorBlending.pAttachments										= &colorBlendAttachment;
	colorBlending.blendConstants[0]									= 0.0f;
	colorBlending.blendConstants[1]									= 0.0f;
	colorBlending.blendConstants[2]									= 0.0f;
	colorBlending.blendConstants[3]									= 0.0f;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo					= {};
	pipelineLayoutInfo.sType										= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount								= 0;
	pipelineLayoutInfo.pSetLayouts									= nullptr;
	pipelineLayoutInfo.pushConstantRangeCount						= 0;
	pipelineLayoutInfo.pPushConstantRanges							= nullptr;

	if (vkCreatePipelineLayout(
	
		device,
		&pipelineLayoutInfo,
		nullptr,
		&pipelineLayout
	
	) != VK_SUCCESS) {
	
		logger.log(ERROR_LOG, "Failed to create pipeline layout!");
	
	}

	VkGraphicsPipelineCreateInfo pipelineInfo						= {};
	pipelineInfo.sType												= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount											= 2;
	pipelineInfo.pStages											= shaderStages;
	pipelineInfo.pVertexInputState									= &vertexInputInfo;
	pipelineInfo.pInputAssemblyState								= &inputAssembly;
	pipelineInfo.pViewportState										= &viewportState;
	pipelineInfo.pRasterizationState								= &rasterizer;
	pipelineInfo.pMultisampleState									= &multisampling;
	pipelineInfo.pDepthStencilState									= nullptr;
	pipelineInfo.pColorBlendState									= &colorBlending;
	pipelineInfo.pDynamicState										= nullptr;
	pipelineInfo.layout												= pipelineLayout;
	pipelineInfo.renderPass											= renderPass;
	pipelineInfo.subpass											= 0;
	pipelineInfo.basePipelineHandle									= VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex									= -1;

	if (vkCreateGraphicsPipelines(
	
		device,
		VK_NULL_HANDLE,
		1,
		&pipelineInfo,
		nullptr,
		&graphicsPipeline
	
	) != VK_SUCCESS) {
	
		logger.log(ERROR_LOG, "Failed to create graphics pipeline!");
	
	}

	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);

}

/*
*	Function:		void createRenderPass()
*	Purpose:		Generates a render pass
*
*/
void Engine::createRenderPass(void) {

	VkAttachmentDescription colorAttachment		= {};
	colorAttachment.format						= swapChainImageFormat;
	colorAttachment.samples						= VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp						= VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp						= VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp				= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp				= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout				= VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout					= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	
	VkAttachmentReference colorAttachmentRef	= {};
	colorAttachmentRef.attachment				= 0;
	colorAttachmentRef.layout					= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass				= {};
	subpass.pipelineBindPoint					= VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount				= 1;
	subpass.pColorAttachments					= &colorAttachmentRef;
	
	VkSubpassDependency dependency				= {};
	dependency.srcSubpass						= VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass						= 0;
	dependency.srcStageMask						= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask					= 0;
	dependency.dstStageMask						= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask					= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo		= {};
	renderPassInfo.sType						= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount				= 1;
	renderPassInfo.pAttachments					= &colorAttachment;
	renderPassInfo.subpassCount					= 1;
	renderPassInfo.pSubpasses					= &subpass;
	renderPassInfo.dependencyCount				= 1;
	renderPassInfo.pDependencies				= &dependency;

	if (vkCreateRenderPass(
		
		device, 
		&renderPassInfo,
		nullptr,
		&renderPass
	
	) != VK_SUCCESS) {
	
		logger.log(ERROR_LOG, "Failed to create render pass!");
	
	}

}

/*
*	Function:		void createFramebuffers()
*	Purpose:		Sets up and creates all needed framebuffers
*	
*/
void Engine::createFramebuffers(void) {

	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
	
		VkImageView attachments[] = {
		
			swapChainImageViews[i]
		
		};

		VkFramebufferCreateInfo framebufferInfo				= {};
		framebufferInfo.sType								= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass							= renderPass;
		framebufferInfo.attachmentCount						= 1;
		framebufferInfo.pAttachments						= attachments;
		framebufferInfo.width								= swapChainExtent.width;
		framebufferInfo.height								= swapChainExtent.height;
		framebufferInfo.layers								= 1;

		if (vkCreateFramebuffer(
		
			device,
			&framebufferInfo,
			nullptr,
			&swapChainFramebuffers[i]

		) != VK_SUCCESS) {
		
			logger.log(ERROR_LOG, "Failed to create framebuffer!");
		
		}

	}

}

/*
*	Function:		void createCommandPool()
*	Purpose:		Creates the command pool
*
*/
void Engine::createCommandPool(void) {

	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo poolInfo	= {};
	poolInfo.sType						= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex			= queueFamilyIndices.graphicsFamily.value();
	poolInfo.flags						= 0;

	if (vkCreateCommandPool(
	
		device,
		&poolInfo,
		nullptr,
		&commandPool
	
	) != VK_SUCCESS) {
	
		logger.log(ERROR_LOG, "Failed to create command pool!");
	
	}

}

/*
*	Function:		void createCommandBuffers()
*	Purpose:		Creates command buffers
*
*/
void Engine::createCommandBuffers(void) {

	commandBuffers.resize(swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo		= {};
	allocInfo.sType								= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool						= commandPool;
	allocInfo.level								= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount				= (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(
	
		device,
		&allocInfo,
		commandBuffers.data()
	
	) != VK_SUCCESS) {
	
		logger.log(ERROR_LOG, "Failed to allocate command buffers!");
	
	}

	for (size_t i = 0; i < commandBuffers.size(); i++) {
	
		VkRenderPassBeginInfo renderPassBeginInfo		= {};
		renderPassBeginInfo.sType						= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass					= renderPass;
		renderPassBeginInfo.framebuffer					= swapChainFramebuffers[i];
		renderPassBeginInfo.renderArea.offset			= {0, 0};
		renderPassBeginInfo.renderArea.extent			= swapChainExtent;

		VkClearValue clearColor							= {0.0f, 0.0f, 0.0f, 1.0f};
		renderPassBeginInfo.clearValueCount				= 1;
		renderPassBeginInfo.pClearValues				= &clearColor;

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkBeginCommandBuffer(
			
			commandBuffers[i],
			&beginInfo
		
		);

		vkCmdBeginRenderPass(
			
			commandBuffers[i],
			&renderPassBeginInfo,
			VK_SUBPASS_CONTENTS_INLINE

			);

				vkCmdBindPipeline(
		
					commandBuffers[i],
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					graphicsPipeline
		
				);

				// RENDERING CALCULATIONS HERE \\

				vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
		
			logger.log(ERROR_LOG, "Failed to record command buffer!");
		
		}
	}

}

/*
*	Function:		void createSemaphores()
*	Purpose:		Creates the semaphores to safely compute everything
*
*/
void Engine::createSemaphores(void) {

	VkSemaphoreCreateInfo semaphoreInfo		= {};
	semaphoreInfo.sType						= VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	
	if (vkCreateSemaphore(
		
		device,
		&semaphoreInfo,
		nullptr,
		&imageAvailableSemaphore
	
	) != VK_SUCCESS || vkCreateSemaphore(
	
		device, 
		&semaphoreInfo,
		nullptr,
		&renderFinishedSemaphore
	
	) != VK_SUCCESS) {
	
		logger.log(ERROR_LOG, "Failed to create semaphores!");
	
	}

}

/*
*	Function:		void renderFrame()
*	Purpose:		Renders a frame to the screen
*
*/
void Engine::renderFrame(void) {

	uint32_t imageIndex;
	vkAcquireNextImageKHR(
	
		device,
		swapChain,
		std::numeric_limits< uint64_t >::max(),
		imageAvailableSemaphore,
		VK_NULL_HANDLE,
		&imageIndex
	
	);

	VkSubmitInfo submitInfo				= {};
	submitInfo.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[]		= {imageAvailableSemaphore};
	VkPipelineStageFlags waitStages[]	= {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount		= 1;
	submitInfo.pWaitSemaphores			= waitSemaphores;
	submitInfo.pWaitDstStageMask		= waitStages;
	submitInfo.commandBufferCount		= 1;
	submitInfo.pCommandBuffers			= &commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[]		= {renderFinishedSemaphore};
	submitInfo.signalSemaphoreCount		= 1;
	submitInfo.pSignalSemaphores		= signalSemaphores;

	if (vkQueueSubmit(
	
		graphicsQueue,
		1,
		&submitInfo,
		VK_NULL_HANDLE
	
	) != VK_SUCCESS) {
	
		logger.log(ERROR_LOG, "Failed to submit draw buffer command!");
	
	}

	VkPresentInfoKHR presentInfo		= {};
	presentInfo.sType					= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount		= 1;
	presentInfo.pWaitSemaphores			= signalSemaphores;

	VkSwapchainKHR swapChains[]			= {swapChain};
	presentInfo.swapchainCount			= 1;
	presentInfo.pSwapchains				= swapChains;
	presentInfo.pImageIndices			= &imageIndex;
	presentInfo.pResults				= nullptr;

	vkQueuePresentKHR(presentQueue, &presentInfo);

	vkQueueWaitIdle(presentQueue);

}
