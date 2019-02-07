/*
*	File:	Engine.cpp
*
*
*
*/
#include "Engine.hpp"
#include <stb_image.h>
#include <tiny_obj_loader.h>
#include "CubeVertex.cpp"

/*
*	Function:		void run()
*	Purpose:		Initializes the application
*
*/
void Engine::run() {

	logger.log(EVENT_LOG, "Initializing GLFW-window...");
	initWindow();
	engine.loadingProgress += 0.1f;
	logger.log(EVENT_LOG, "Successfully initialized GLFW-window!");
	logger.log(EVENT_LOG, "Initializing Vulkan...");
	initVulkan();
	engine.loadingProgress += 0.1f;
	logger.log(EVENT_LOG, "Successfully initialized VULKAN!");
	logger.log(EVENT_LOG, "Entering main game loop...");
	mainLoop();	
	logger.log(EVENT_LOG, "Cleaning up...");
	cleanup();
	logger.log(EVENT_LOG, "Successfully cleaned up!");

}

/*
*	Function:		void initStartWindow()
*	Purpose:		Creates another GLFW context that will be used as a splash / startup screen
*	
*/
void Engine::initStartWindow(void) {

	startWindow = new StartWindow();

	logger.log(EVENT_LOG, "Starting thread...");
	std::thread t0([=] () {

		startWindow->loop();
		logger.log(EVENT_LOG, "Stopping thread...");

	});
	t0.detach();

}

/*
*	Function:		void initWindow()
*	Purpose:		Initializes GLFW and its window_
*
*/
void Engine::initWindow() {
	
	initStartWindow();
#if defined GAME_RELEASE

	logger.hideConsoleWindow();

#endif

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
	glfwWindowHint(GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

#if !defined GAME_WINDOW_MODE_WINDOWED && !defined GAME_WINDOW_MODE_FULLSCREEN && !defined GAME_WINDOW_MODE_BORDERLESS
	#define GAME_WINDOW_MODE_UNDEFINED
#endif

	// Create a fullscreen window
	// !!! IMPORTANT !!!
	// TEARING ISSUES
#ifdef GAME_WINDOW_MODE_FULLSCREEN
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	

	window = glfwCreateWindow(
		
		mode->width,
		mode->height, 
		engine.TITLE.c_str(),
		monitor,
		nullptr
	
	);
#endif
#if defined GAME_WINDOW_MODE_WINDOWED
	// Create a windowed window
	window = glfwCreateWindow(

		WIDTH,
		HEIGHT,
		engine.TITLE.c_str(),
		nullptr,
		nullptr

	);

	glfwSetWindowPos(

		window,
		mode->width / 2 - WIDTH / 2,
		mode->height / 2 - HEIGHT / 2

	);
#endif
#if defined GAME_WINDOW_MODE_BORDERLESS || defined GAME_WINDOW_MODE_UNDEFINED
	// Create a borderless fullscreen window 
	// !!! IMPORTANT !!!
	// TEARING ISSUES
	window = glfwCreateWindow(

		mode->width,
		mode->height,
		engine.TITLE.c_str(),
		monitor,
		nullptr

	);
#endif

	glfwMakeContextCurrent(window);

	loadingProgress += 0.1f;

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetKeyCallback(window, keyboardInputCallback);

	glfwSetInputMode(
	
		window,
		GLFW_CURSOR,
		GLFW_CURSOR_DISABLED
	
	);

	GLFWimage windowIcon[1];
	windowIcon[0].pixels = stbi_load(
		
		"res/icon/infinity.jpg",
		&windowIcon[0].width,
		&windowIcon[0].height,
		0,
		STBI_rgb_alpha
	
	);

	glfwSetWindowIcon(
		
		window, 
		1,
		windowIcon

	);

	stbi_image_free(windowIcon[0].pixels);

	engine.loadingProgress += 0.1f;

}

/*
*	Function:		void initVulkan()
*	Purpose:		Initializes Vulkan
*
*/
void Engine::initVulkan() {

	numThreads = getNumThreads();

	std::cout << green << "std::thread::hardware_concurrency()" << white << ":		" << yellow << numThreads << white << std::endl;
	
	createCamera();

	engine.loadingProgress += 0.1f;

	init3DAudio();

	engine.loadingProgress += 0.1f;

	createInstance();
	setupDebugCallback();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createDescriptorSetLayout();
	createDescriptorPool();
	createCommandPool();
	createColorResources();
	createDepthResources();
	createFramebuffers();
	createTextureImage();
	createTextureImageView();
	createTextureSampler(); 

	engine.loadingProgress += 0.1f;
	//std::this_thread::sleep_for(std::chrono::seconds(5));		// JUST TO SHOW LOADING SCREEN A LITTLE BIT LONGER!!!
	
	loadModels();

	engine.loadingProgress += 0.1f;

	createUniformBuffers();
	createPipelines();
	createDescriptorSets();
	createCommandBuffers();
	createSyncObjects();

	glfwShowWindow(window); 
	glfwFocusWindow(window);

	engine.closeStartWindow.lock();
	startWindow->closeVar = true;
	engine.closeStartWindow.unlock();

	delete startWindow;

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
	appInfo.pApplicationName		= engine.TITLE.c_str();
	appInfo.applicationVersion		= VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName				= engine.TITLE.c_str();
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

	double lastTime		= glfwGetTime();
	double pastTime		= 0;
	float nbFrames		= 0;
	float maxfps		= 0;

	std::thread t0([=]() {

		bgmusic = audioEngine->play3D(
			
			"res/sounds/bgmusic2.wav",
			irrklang::vec3df(0, 0, 0),
			true, 
			false, 
			true
		
		);

		if (bgmusic) {
		
			bgmusic->setMinDistance(1.0f);
			bgmusic->setVolume(1.0f);
		
		}

	});
	t0.detach();

	while (!glfwWindowShouldClose(window)) {

		double currentTime		= glfwGetTime();
		double deltaTime		= currentTime - pastTime;
		DELTATIME				= deltaTime;
#if defined GAME_USE_FRAMERATE_CAP_60
		if (deltaTime >= maxPeriod) {
#endif
			pastTime = currentTime;
		
			nbFrames++;
			float seconds = 10.0f;

			if (currentTime - lastTime >= 1.0 && nbFrames > maxfps) {
		
				maxfps = nbFrames;
		
			}

			if (currentTime - lastTime >= seconds) {

				std::string fps				= "Average FPS (last " + std::to_string(seconds) + " seconds):	%f\t";
				std::string frametime		= "Average Frametime (last " + std::to_string(seconds) + " seconds):	%f ms\t";
				std::string maxFPS			= "Max FPS:	%f\n";

				printf(fps.c_str(), double(nbFrames / seconds));
				printf(frametime.c_str(), double((1000.0 * seconds) / nbFrames));
				printf(maxFPS.c_str(), double(maxfps / seconds));
				nbFrames	 = 0;
				lastTime	+= seconds;

			}

			audioEngine->setListenerPosition(
				
				irrklang::vec3df(
					
					camera.position.x, 
					camera.position.y, 
					camera.position.z
				
				),
				irrklang::vec3df(
					
					-camera.front.x,
					-camera.front.y,
					-camera.front.z
				
				)
			
			);

			glfwPollEvents();
			queryKeyboardGLFW();
			renderFrame();
#if defined GAME_USE_FRAMERATE_CAP_60
		}
#endif

	}

	vkDeviceWaitIdle(device);

}

/*
*	Function:		void cleanup()
*	Purpose:		Handles all destruction and cleaning up tasks
*
*/
void Engine::cleanup() {

	//effect->drop();
	bgmusic->drop();
	audioEngine->drop();

	cleanupSwapChain();

	vkDestroySampler(
	
		device,
		textureSampler,
		nullptr

	);

	vkDestroyImageView(
	
		device,
		textureImageView,
		nullptr

	);

	vkDestroyImage(
		
		device,
		textureImage, 
		nullptr
	
	);
	vkFreeMemory(
		
		device,
		textureImageMemory,
		nullptr
	
	);

	/*vkDestroyDescriptorSetLayout(
	
		device,
		objectDescriptorSetLayout,
		nullptr
	
	);

	vkDestroyDescriptorSetLayout(

		device,
		lightingDescriptorSetLayout,
		nullptr

	);*/

	renderedCube->destroy();
	lightingCube->destroy();

	delete renderedCube;
	delete lightingCube;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

		vkDestroySemaphore(
			
			device, 
			renderFinishedSemaphores[i],
			nullptr
		
		);
		vkDestroySemaphore(
			
			device,
			imageAvailableSemaphores[i],
			nullptr
		
		);
		vkDestroyFence(
			
			device,
			inFlightFences[i],
			nullptr
		
		);

	}

	vkDestroyCommandPool(
	
		device,
		commandPool,
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
*						VkDebugUtilsMessageSeverityFlagBitsEXT			messageSeverity_,
*						VkDebugUtilsMessageTypeFlagsEXT					messageType_,
*						const VkDebugUtilsMessengerCallbackDataEXT*		pCallbackData_,
*						void*											pUserData_
*							
*						)
*	Purpose:		Callback function for VULKAN validation layers
*
*/
VKAPI_ATTR VkBool32 VKAPI_CALL Engine::debugCallback(

	VkDebugUtilsMessageSeverityFlagBitsEXT			messageSeverity_,
	VkDebugUtilsMessageTypeFlagsEXT					messageType_,
	const VkDebugUtilsMessengerCallbackDataEXT*		pCallbackData_,
	void*											pUserData_

	) {

	std::string msg = pCallbackData_->pMessage;

	std::cout << red << "Validation layer: " << msg << white << std::endl;

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

		logger.log(ERROR_LOG, "Failed to find GPU's with Vulkan support!");

	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(
		
		instance, 
		&deviceCount, 
		devices.data()
	
	);

	for (const auto& device : devices) {

		if (isDeviceSuitable(device)) {

			physicalDevice		= device;
			msaaSamples			= getMaxUsableSampleCount();
			break;

		}

	}

	if (physicalDevice == VK_NULL_HANDLE) {

		logger.log(ERROR_LOG, "Failed to find a suitable GPU!");

	}

}

/*
*	Function:		bool isDeviceSuitable(VkPhysicalDevice device_)
*	Purpose:		Checks whether the device_ supports VULKAN by giving each device_ a score and picking the highest one
*
*/
bool Engine::isDeviceSuitable(VkPhysicalDevice device_) {

	QueueFamilyIndices indices = findQueueFamilies(device_);

	bool extensionsSupported = checkDeviceExtensionSupport(device_);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
	
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device_);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device_, &supportedFeatures);

	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;

}

/*
*	Function:		void createLogicalDevice()
*	Purpose:		Creates the logical device_ from previously selected physical device_
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
	deviceFeatures.samplerAnisotropy		= VK_TRUE;
	deviceFeatures.sampleRateShading		= VK_TRUE;

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
	*	Function:		QueueFamilyIndices game::findQueueFamilies(VkPhysicalDevice device_)
	*	Purpose:		Finds the indices for the correct queue families for the physical device_
	*
	*/
QueueFamilyIndices Engine::findQueueFamilies(VkPhysicalDevice device_) {

	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(

		device_,
		&queueFamilyCount,
		nullptr

	);

	std::vector< VkQueueFamilyProperties > queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(

		device_,
		&queueFamilyCount,
		queueFamilies.data()

	);

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(

			device_,
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
*	Function:		bool checkDeviceExtensionSupport(VkPhysicalDevice device_)
*	Purpose:		Check whether the target device_ has support for swapchains
*
*/
bool Engine::checkDeviceExtensionSupport(VkPhysicalDevice device_) {

	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(
		
		device_,
		nullptr,
		&extensionCount,
		nullptr
	
	);

	std::vector< VkExtensionProperties > availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(
		
		device_,
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
*	Function:		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device_)
*	Purpose:		Querys the system for swapchain support
*
*/
SwapChainSupportDetails Engine::querySwapChainSupport(VkPhysicalDevice device_) {
	
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
	
		device_,
		surface,
		&details.capabilities
	
	);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(
		
		device_,
		surface,
		&formatCount, 
		nullptr
	
	);

	if (formatCount != 0) {
	
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(

			device_,
			surface,
			&formatCount,
			details.formats.data()
		
		);

	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(
	
		device_,
		surface,
		&presentModeCount,
		nullptr

	);

	if (presentModeCount != 0) {
	
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(
		
			device_,
			surface,
			&presentModeCount,
			details.presentModes.data()
		
		);
	
	}

	return details;

}

/*
*	Function:		VkSurfaceFormatKHR chooseSwapSurfaceFormat(cosnt std::vector< VkSurfaceFormatKHR >& availableFormats_)
*	Purpose:		Finds the optimal surface format_ for the specified logical device_
*
*/
VkSurfaceFormatKHR Engine::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats_) {

	if (availableFormats_.size() == 1 && availableFormats_[0].format == VK_FORMAT_UNDEFINED) {
	
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	
	}

	for (const auto& availableFormat : availableFormats_) {
	
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
		
			return availableFormat;
		
		}
	
	}

	return availableFormats_[0];
}

/*
*	Function:		VkPresentModeKHR chooseSwapPresentMode(const std::vector< VkPresentModeKHR > availablePresentModes_)
*	Purpose:		Enumerates best present mode for swapchain
*
*/
VkPresentModeKHR Engine::chooseSwapPresentMode(const std::vector< VkPresentModeKHR > availablePresentModes_) {
	
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_MAILBOX_KHR;

	std::string mode = "VK_PRESENT_MODE_MAILBOX_KHR";

	for (const auto& availablePresentMode : availablePresentModes_) {
	
		if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR) {
		
			mode = "VK_PRESENT_MODE_FIFO_KHR";
			bestMode = availablePresentMode;
		
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
		
			mode = "VK_PRESENT_MODE_IMMEDIATE_KHR";
			bestMode = availablePresentMode;
		
		}
	
	}

	logger.log(EVENT_LOG, "Swapchain presentation mode:	" + mode);

	return bestMode;

}

/*
*	Function:		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities_)
*	Purpose:		Enumerates best image_ size_ for swapchain
*
*/
VkExtent2D Engine::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities_) {

	if (capabilities_.currentExtent.width != std::numeric_limits< uint32_t >::max()) {
	
		return capabilities_.currentExtent;
	
	}
	else {

		int width, height;
		glfwGetFramebufferSize(
		
			window,
			&width,
			&height
		
		);
	
		VkExtent2D actualExtent = {
			
			static_cast< uint32_t >(width), 
			static_cast< uint32_t >(height)
		
		};
		actualExtent.width = std::max(
			
			capabilities_.minImageExtent.width, 
			std::min(capabilities_.maxImageExtent.width, actualExtent.width)
		
		);
		actualExtent.height = std::max(
		
			capabilities_.minImageExtent.width,
			std::min(capabilities_.maxImageExtent.width, actualExtent.height)

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

	VkSwapchainCreateInfoKHR createInfo			= {};
	createInfo.sType							= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface							= surface;
	createInfo.minImageCount					= imageCount;
	createInfo.imageFormat						= surfaceFormat.format;
	createInfo.imageColorSpace					= surfaceFormat.colorSpace;
	createInfo.imageExtent						= extent;
	createInfo.imageArrayLayers					= 1;
	createInfo.imageUsage						= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices					= findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[]				= {indices.graphicsFamily.value(), indices.presentFamily.value()};

	if (indices.graphicsFamily != indices.presentFamily) {
	
		createInfo.imageSharingMode				= VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount		= 2;
		createInfo.pQueueFamilyIndices			= queueFamilyIndices;

	}
	else {
	
		createInfo.imageSharingMode				= VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount		= 0;
		createInfo.pQueueFamilyIndices			= nullptr;
	
	}

	createInfo.preTransform						= swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha					= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode						= presentMode;
	createInfo.clipped							= VK_TRUE;
	createInfo.oldSwapchain						= VK_NULL_HANDLE;

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
	
		swapChainImageViews[i] = createImageView(
			
			swapChainImages[i], 
			swapChainImageFormat,
			VK_IMAGE_ASPECT_COLOR_BIT,
			1
		
		);

	}

	

}

/*
*	Function:		VkShaderModule createShaderModule(const std::vector< char >& code_)
*	Purpose:		Creates a shader module from a given byte array of SPIR-V compiled GLSL-shading code
*
*/
VkShaderModule Engine::createShaderModule(const std::vector< char >& code_) {

	VkShaderModuleCreateInfo createInfo			= {};
	createInfo.sType							= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize							= code_.size();
	createInfo.pCode							= reinterpret_cast< const uint32_t* >(code_.data());
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
*	Function:		void createPipelines()
*	Purpose:		Generates the graphics pipeline for rendering
*
*/
void Engine::createPipelines(void) {

	VkVertexInputBindingDescription objectBindingDescription						= CubeVertex::getBindingDescription();
	std::array< VkVertexInputAttributeDescription, 1 > objectAttributeDescriptions  = CubeVertex::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo							= {};
	vertexInputInfo.sType															= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount									= 1;
	vertexInputInfo.vertexAttributeDescriptionCount									= static_cast< uint32_t >(objectAttributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions										= &objectBindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions									= objectAttributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly							= {};
	inputAssembly.sType																= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology															= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable											= VK_FALSE;

	VkViewport viewport																= {};
	viewport.x																		= 0.0f;
	viewport.y																		= 0.0f;
	viewport.width																	= (float)swapChainExtent.width;
	viewport.height																	= (float)swapChainExtent.height;
	viewport.minDepth																= 0.0f;
	viewport.maxDepth																= 1.0f;

	VkRect2D scissor																= {};
	scissor.offset																	= {0, 0};
	scissor.extent																	= swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState									= {};
	viewportState.sType																= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount														= 1;
	viewportState.pViewports														= &viewport;
	viewportState.scissorCount														= 1;
	viewportState.pScissors															= &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer								= {};
	rasterizer.sType																= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable														= VK_FALSE;
	rasterizer.rasterizerDiscardEnable												= VK_FALSE;
	rasterizer.polygonMode															= VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth															= 1.0f;
	rasterizer.cullMode																= VK_CULL_MODE_NONE;
	rasterizer.frontFace															= VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable														= VK_FALSE;
	rasterizer.depthBiasConstantFactor												= 0.0f;
	rasterizer.depthBiasClamp														= 0.0f;
	rasterizer.depthBiasSlopeFactor													= 0.0f;

	VkPipelineMultisampleStateCreateInfo multisampling								= {};
	multisampling.sType																= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable												= VK_TRUE;
	multisampling.rasterizationSamples												= msaaSamples;
	multisampling.minSampleShading													= 0.2f;
	multisampling.pSampleMask														= nullptr;
	multisampling.alphaToCoverageEnable												= VK_FALSE;
	multisampling.alphaToOneEnable													= VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment						= {};
	colorBlendAttachment.colorWriteMask												= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable												= VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor										= VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor										= VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp												= VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor										= VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor										= VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp												= VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending								= {};
	colorBlending.sType																= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable														= VK_FALSE;
	colorBlending.logicOp															= VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount													= 1;
	colorBlending.pAttachments														= &colorBlendAttachment;
	colorBlending.blendConstants[0]													= 0.0f;
	colorBlending.blendConstants[1]													= 0.0f;
	colorBlending.blendConstants[2]													= 0.0f;
	colorBlending.blendConstants[3]													= 0.0f;

	VkPipelineDepthStencilStateCreateInfo depthStencil								= {};
	depthStencil.sType																= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable													= VK_TRUE;
	depthStencil.depthWriteEnable													= VK_TRUE;
	depthStencil.depthCompareOp														= VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable												= VK_FALSE;
	depthStencil.minDepthBounds														= 0.0f;
	depthStencil.maxDepthBounds														= 1.0f;
	depthStencil.stencilTestEnable													= VK_FALSE;
	depthStencil.front																= {};
	depthStencil.back																= {};

	VkDescriptorSetLayoutBinding uboLayoutBinding									= {};
	uboLayoutBinding.binding														= 0;
	uboLayoutBinding.descriptorCount												= 1;
	uboLayoutBinding.descriptorType													= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers												= nullptr;
	uboLayoutBinding.stageFlags														= VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding								= {};
	samplerLayoutBinding.binding													= 1;
	samplerLayoutBinding.descriptorCount											= 1;
	samplerLayoutBinding.descriptorType												= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers											= nullptr;
	samplerLayoutBinding.stageFlags													= VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding colorBinding										= {};
	colorBinding.binding															= 1;
	colorBinding.descriptorCount													= 1;
	colorBinding.descriptorType														= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	colorBinding.pImmutableSamplers													= nullptr;
	colorBinding.stageFlags															= VK_SHADER_STAGE_FRAGMENT_BIT;

	std::vector< VkDescriptorSetLayoutBinding > bindings							= { uboLayoutBinding, colorBinding };

	objectPipeline = Pipeline(
		
		"shaders/objectShaders/vert.spv", 
		"shaders/objectShaders/frag.spv",
		&vertexInputInfo,
		&inputAssembly,
		&viewportState,
		&rasterizer,
		&multisampling,
		&depthStencil,
		&colorBlending,
		nullptr,
		renderPass,
		0,
		VK_NULL_HANDLE,
		-1,
		&bindings,
		descriptorPool,
		true

	);

	objectPipeline.descriptorSetWrites([=] () {

		for (size_t i = 0; i < engine.swapChainImages.size(); i++) {

			VkDescriptorBufferInfo bufferInfo							= {};
			bufferInfo.buffer											= objectPipeline.uniformBuffers[i];
			bufferInfo.offset											= 0;
			bufferInfo.range											= sizeof(UniformBufferObject);

			VkDescriptorImageInfo imageInfo								= {};
			imageInfo.imageLayout										= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView											= textureImageView;
			imageInfo.sampler											= textureSampler;

			VkDescriptorBufferInfo lightingBufferInfo					= {};
			lightingBufferInfo.buffer									= objectPipeline.lightingBuffers[i];
			lightingBufferInfo.offset									= 0;
			lightingBufferInfo.range									= sizeof(LightingBufferObject);

			std::array< VkWriteDescriptorSet, 2 > descriptorWrites		= {};
			descriptorWrites[0].sType									= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet									= objectPipeline.descriptorSets[i];
			descriptorWrites[0].dstBinding								= 0;
			descriptorWrites[0].dstArrayElement							= 0;
			descriptorWrites[0].descriptorType							= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount							= 1;
			descriptorWrites[0].pBufferInfo								= &bufferInfo;
			descriptorWrites[1].sType									= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet									= objectPipeline.descriptorSets[i];
			descriptorWrites[1].dstBinding								= 1;
			descriptorWrites[1].dstArrayElement							= 0;
			descriptorWrites[1].descriptorType							= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[1].descriptorCount							= 1;
			descriptorWrites[1].pBufferInfo								= &lightingBufferInfo;
			/*descriptorWrites[1].sType									= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet									= objectPipeline.descriptorSets[i];
			descriptorWrites[1].dstBinding								= 1;
			descriptorWrites[1].dstArrayElement							= 0;
			descriptorWrites[1].descriptorType							= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount							= 1;
			descriptorWrites[1].pImageInfo								= &imageInfo;*/

			vkUpdateDescriptorSets(

				device,
				static_cast< uint32_t >(descriptorWrites.size()),
				descriptorWrites.data(),
				0,
				nullptr

			);

		}

	});

	VkVertexInputBindingDescription lightingBindingDescription								= CubeVertex::getBindingDescription();
	std::array< VkVertexInputAttributeDescription, 1 > lightingAttributeDescriptions		= CubeVertex::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount											= 1;
	vertexInputInfo.vertexAttributeDescriptionCount											= static_cast< uint32_t >(lightingAttributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions												= &lightingBindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions											= lightingAttributeDescriptions.data();

	rasterizer.cullMode																		= VK_CULL_MODE_NONE;

	std::vector< VkDescriptorSetLayoutBinding > lightingBindings							= { uboLayoutBinding };

	lightingPipeline = Pipeline(
		
		"shaders/lightingShaders/vert.spv",
		"shaders/lightingShaders/frag.spv",
		&vertexInputInfo,
		&inputAssembly,
		&viewportState,
		&rasterizer,
		&multisampling,
		&depthStencil,
		&colorBlending,
		nullptr,
		renderPass,
		0,
		VK_NULL_HANDLE,
		-1,
		&lightingBindings,
		lightingDescriptorPool,
		false

	);

	lightingPipeline.descriptorSetWrites([=] () {

		for (size_t i = 0; i < engine.swapChainImages.size(); i++) {

			VkDescriptorBufferInfo bufferInfo											= {};
			bufferInfo.buffer															= lightingPipeline.uniformBuffers[i];
			bufferInfo.offset															= 0;
			bufferInfo.range															= sizeof(UniformBufferObject);

			std::array< VkWriteDescriptorSet, 1> descriptorWrites						= {};
			descriptorWrites[0].sType													= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet													= lightingPipeline.descriptorSets[i];
			descriptorWrites[0].dstBinding												= 0;
			descriptorWrites[0].dstArrayElement											= 0;
			descriptorWrites[0].descriptorType											= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount											= 1;
			descriptorWrites[0].pBufferInfo												= &bufferInfo;

			vkUpdateDescriptorSets(

				device,
				static_cast< uint32_t >(descriptorWrites.size()),
				descriptorWrites.data(),
				0,
				nullptr

			);

		}

	});

	vkDestroyShaderModule(

		device,
		objectPipeline.getVertShaderModule().getModule(),
		nullptr

	);
	vkDestroyShaderModule(

		device,
		objectPipeline.getFragShaderModule().getModule(),
		nullptr

	);

	vkDestroyShaderModule(

		device,
		lightingPipeline.getVertShaderModule().getModule(),
		nullptr

	);
	vkDestroyShaderModule(

		device,
		lightingPipeline.getFragShaderModule().getModule(),
		nullptr

	);

}

/*
*	Function:		void createRenderPass()
*	Purpose:		Generates a render pass
*
*/
void Engine::createRenderPass(void) {

	VkAttachmentDescription colorAttachment						= {};
	colorAttachment.format										= swapChainImageFormat;
	colorAttachment.samples										= msaaSamples;
	colorAttachment.loadOp										= VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp										= VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp								= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp								= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout								= VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout									= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	
	VkAttachmentDescription depthAttachment						= {};
	depthAttachment.format										= findDepthFormat();
	depthAttachment.samples										= msaaSamples;
	depthAttachment.loadOp										= VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp										= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp								= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp								= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout								= VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout									= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription colorAttachmentResolve				= {};
	colorAttachmentResolve.format								= swapChainImageFormat;
	colorAttachmentResolve.samples								= VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentResolve.loadOp								= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.storeOp								= VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve.stencilLoadOp						= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.stencilStoreOp						= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve.initialLayout						= VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve.finalLayout							= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef					= {};
	colorAttachmentRef.attachment								= 0;
	colorAttachmentRef.layout									= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef					= {};
	depthAttachmentRef.attachment								= 1;
	depthAttachmentRef.layout									= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentResolveRef				= {};
	colorAttachmentResolveRef.attachment						= 2;
	colorAttachmentResolveRef.layout							= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass								= {};
	subpass.pipelineBindPoint									= VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount								= 1;
	subpass.pColorAttachments									= &colorAttachmentRef;
	subpass.pDepthStencilAttachment								= &depthAttachmentRef;
	subpass.pResolveAttachments									= &colorAttachmentResolveRef;
	
	VkSubpassDependency dependency								= {};
	dependency.srcSubpass										= VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass										= 0;
	dependency.srcStageMask										= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask									= 0;
	dependency.dstStageMask										= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask									= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array< VkAttachmentDescription, 3 > attachments		= {colorAttachment, depthAttachment, colorAttachmentResolve};
	VkRenderPassCreateInfo renderPassInfo						= {};
	renderPassInfo.sType										= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount								= static_cast< uint32_t >(attachments.size());
	renderPassInfo.pAttachments									= attachments.data();
	renderPassInfo.subpassCount									= 1;
	renderPassInfo.pSubpasses									= &subpass;
	renderPassInfo.dependencyCount								= 1;
	renderPassInfo.pDependencies								= &dependency;

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
	
		std::array< VkImageView, 3 > attachments = {

			colorImageView,
			depthImageView,
			swapChainImageViews[i]

		};

		VkFramebufferCreateInfo framebufferInfo					= {};
		framebufferInfo.sType									= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass								= renderPass;
		framebufferInfo.attachmentCount							= static_cast< uint32_t >(attachments.size());
		framebufferInfo.pAttachments							= attachments.data();
		framebufferInfo.width									= swapChainExtent.width;
		framebufferInfo.height									= swapChainExtent.height;
		framebufferInfo.layers									= 1;

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

		std::array< VkClearValue, 2 > clearValues		= {};
		clearValues[0].color							= {0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0f};
		clearValues[1].depthStencil						= {1.0f, 0};
		renderPassBeginInfo.clearValueCount				= static_cast< uint32_t >(clearValues.size());
		renderPassBeginInfo.pClearValues				= clearValues.data();

		VkCommandBufferBeginInfo beginInfo				= {};
		beginInfo.sType									= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkBeginCommandBuffer(
			
			commandBuffers[i],
			&beginInfo
		
		);

		vkCmdBeginRenderPass(
			
			commandBuffers[i],
			&renderPassBeginInfo,
			VK_SUBPASS_CONTENTS_INLINE

		);

			objectPipeline.bind(commandBuffers[i], &objectPipeline.descriptorSets[i]);

				VkDeviceSize offsets[] = {0};

				renderedCube->draw(

					commandBuffers[i],
					offsets,
					0,
					VK_INDEX_TYPE_UINT32

				);

			lightingPipeline.bind(commandBuffers[i], &lightingPipeline.descriptorSets[i]);
			
				lightingCube->draw(
					
					commandBuffers[i],
					offsets,
					0,
					VK_INDEX_TYPE_UINT32

				);

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
void Engine::createSyncObjects(void) {

	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo		= {};
	semaphoreInfo.sType						= VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo				= {};
	fenceInfo.sType							= VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags							= VK_FENCE_CREATE_SIGNALED_BIT;


	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

		if (vkCreateSemaphore(

			device,
			&semaphoreInfo,
			nullptr,
			&imageAvailableSemaphores[i]

		) != VK_SUCCESS || vkCreateSemaphore(

			device,
			&semaphoreInfo,
			nullptr,
			&renderFinishedSemaphores[i]

		) != VK_SUCCESS ||vkCreateFence(
		
			device,
			&fenceInfo,
			nullptr,
			&inFlightFences[i]
		
		) != VK_SUCCESS) {

			logger.log(ERROR_LOG, "Failed to create semaphores!");

		}

	}

}

/*
*	Function:		void renderFrame()
*	Purpose:		Renders a frame to the screen
*
*/
void Engine::renderFrame(void) {

	vkWaitForFences(
	
		device,
		1,
		&inFlightFences[currentFrame],
		VK_TRUE,
		std::numeric_limits< uint64_t >::max()
	
	);

	uint32_t imageIndex;
	result = vkAcquireNextImageKHR(
	
		device,
		swapChain,
		std::numeric_limits< uint64_t >::max(),
		imageAvailableSemaphores[currentFrame],
		VK_NULL_HANDLE,
		&imageIndex
	
	);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
	
		recreateSwapChain();
		return;
	
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
	
		logger.log(ERROR_LOG, "Failed to acquire swap chain image!");
	
	}

	updateUniformBuffers(imageIndex);

	VkSubmitInfo submitInfo				= {};
	submitInfo.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[]		= {imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[]	= {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount		= 1;
	submitInfo.pWaitSemaphores			= waitSemaphores;
	submitInfo.pWaitDstStageMask		= waitStages;
	submitInfo.commandBufferCount		= 1;
	submitInfo.pCommandBuffers			= &commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[]		= {renderFinishedSemaphores[currentFrame]};
	submitInfo.signalSemaphoreCount		= 1;
	submitInfo.pSignalSemaphores		= signalSemaphores;

	vkResetFences(

		device,
		1,
		&inFlightFences[currentFrame]

	);

	if (vkQueueSubmit(
	
		graphicsQueue,
		1,
		&submitInfo,
		inFlightFences[currentFrame]
	
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

	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
	
		framebufferResized = false;
		recreateSwapChain();
	
	}
	else if (result != VK_SUCCESS) {
	
		logger.log(ERROR_LOG, "Failed to present swap chain image!");
	
	}

	vkQueueWaitIdle(presentQueue);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

}

/*
*	Function:		void recreateSwapChain()
*	Purpose:		Recreates the swapchain on window_ resize events and more
*
*/
void Engine::recreateSwapChain(void) {

	int width = 0, height = 0;
	while (width == 0 || height == 0) {
	
		glfwGetFramebufferSize(
		
			window,
			&width,
			&height
		
		);
	
		glfwWaitEvents();

	}

	vkDeviceWaitIdle(device);

	cleanupSwapChain();

	createSwapChain();
	createImageViews();
	createRenderPass();
	createDescriptorPool();
	createPipelines();
	createColorResources();
	createDepthResources();
	createFramebuffers();
	createCommandBuffers();

}

/*
*	Function:		void cleanupSwapChain()
*	Purpose:		Cleans objects, that are needed for swapchain recreation
*
*/
void Engine::cleanupSwapChain(void) {

	objectPipeline.destroy();

	lightingPipeline.destroy();

	vkDestroyDescriptorPool(

		device,
		descriptorPool,
		nullptr

	);

	vkDestroyDescriptorPool(

		device,
		lightingDescriptorPool,
		nullptr

	);

	vkDestroyImageView(
	
		device,
		colorImageView,
		nullptr	
	);

	vkDestroyImage(
	
		device,
		colorImage,
		nullptr
	
	);

	vkFreeMemory(
	
		device,
		colorImageMemory,
		nullptr
	
	);

	vkDestroyImageView(
		
		device,
		depthImageView,
		nullptr
	
	);

	vkDestroyImage(
	
		device,
		depthImage,
		nullptr
	
	);

	vkFreeMemory(
	
		device,
		depthImageMemory,
		nullptr

	);

	for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {

		vkDestroyFramebuffer(
			
			device,
			swapChainFramebuffers[i], 
			nullptr
		
		);

	}

	vkFreeCommandBuffers(
		
		device,
		commandPool,
		static_cast< uint32_t >(commandBuffers.size()),
		commandBuffers.data()
	
	);

	vkDestroyRenderPass(
		
		device,
		renderPass,
		nullptr
	
	);

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {

		vkDestroyImageView(
			
			device, 
			swapChainImageViews[i], 
			nullptr
		
		);

	}

	vkDestroySwapchainKHR(
		
		device,
		swapChain,
		nullptr
	
	);

}

/*
*	Function:		static void framebufferResizeCallback(
*
*						GLFWwindow*		window_, 
*						int				width_,
*						int				height_
*	
*					)
*	Purpose:		GLFW-callback function on window_ resize event
*
*/
void Engine::framebufferResizeCallback(
	
	GLFWwindow*		window_,
	int				width_,
	int				height_

) {

	auto app = reinterpret_cast< Engine* >(glfwGetWindowUserPointer(window_));
	app->framebufferResized = true;

	std::string log = "Framebuffer resized to: " + std::to_string(width_) + " / " + std::to_string(height_);

	logger.log(EVENT_LOG, log);

}

/*
*	Function:		uint32_t findMemoryType(uint32_t typeFilter_, VkMemoryPropertyFlags properties_)
*	Purpose:		Find memory type to use for vertex data
*
*/
uint32_t Engine::findMemoryType(uint32_t typeFilter_, VkMemoryPropertyFlags properties_) {

	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
	
		if ((typeFilter_ & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties_) == properties_) {
		
			return i;
		
		}
	
	}

	logger.log(ERROR_LOG, "Failed to find suitable memory type!");

	return -1;

}

/*
*	Function:		void createBuffer(
*			
*						VkDeviceSize				size_,
*						VkBufferUsageFlags			usage_,
*						VkMemoryPropertyFlags		properties_,
*						VkBuffer&					buffer_, 
*						VkDeviceMemory&				bufferMemory_
*
*					)
*	Purpose:		Creates a valid VkBuffer handle
*
*/
void Engine::createBuffer(
	
	VkDeviceSize				size_,
	VkBufferUsageFlags			usage_,
	VkMemoryPropertyFlags		properties_,
	VkBuffer&					buffer_, 
	VkDeviceMemory&				bufferMemory_

) {

	VkBufferCreateInfo bufferInfo		= {};
	bufferInfo.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size						= size_;
	bufferInfo.usage					= usage_;
	bufferInfo.sharingMode				= VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(
		
		device,
		&bufferInfo,
		nullptr, 
		&buffer_
	
	) != VK_SUCCESS) {

		logger.log(ERROR_LOG, "Failed to create buffer!");

	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(
		
		device,
		buffer_,
		&memRequirements
	
	);

	VkMemoryAllocateInfo allocInfo		= {};
	allocInfo.sType						= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize			= memRequirements.size;
	allocInfo.memoryTypeIndex			= findMemoryType(memRequirements.memoryTypeBits, properties_);

	if (vkAllocateMemory(
		
		device,
		&allocInfo,
		nullptr,
		&bufferMemory_
	
	) != VK_SUCCESS) {

		logger.log(ERROR_LOG, "Failed to allocate buffer memory!");

	}

	vkBindBufferMemory(
		
		device,
		buffer_,
		bufferMemory_,
		0
	
	);

}

/*
*	Function:		void copyBuffer(
*	
*						VkBuffer srcBuffer_
*						VkBuffer dstBuffer_
*						VkDeviceSize size_
*					)
*	Purpose:		Copies a source buffer_ to a destination buffer_
*
*/
void Engine::copyBuffer(

	VkBuffer		srcBuffer_,
	VkBuffer		dstBuffer_,
	VkDeviceSize	size_

) {

	VkCommandBuffer commandBuffer		= beginSingleTimeCommands();

	VkBufferCopy copyRegion				= {};
	copyRegion.size						= size_;
	vkCmdCopyBuffer(
		
		commandBuffer, 
		srcBuffer_, 
		dstBuffer_,
		1, 
		&copyRegion
	
	);

	endSingleTimeCommands(commandBuffer);

}

/*
*	Function:		void createDescriptorSetLayout()
*	Purpose:		Creates the descriptor set for uniform buffers
*
*/
void Engine::createDescriptorSetLayout(void) {



}

/*
*	Function:		void createUniformBuffers()
*	Purpose:		Creates uniform buffers for each frame and puts them into an array
*
*/
void Engine::createUniformBuffers(void) {

	/*VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(swapChainImages.size());
	uniformBuffersMemory.resize(swapChainImages.size());

	lightUniformBuffers.resize(swapChainImages.size());
	lightUniformBuffersMemory.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++) {
	
		createBuffer(
			
			bufferSize,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniformBuffers[i],
			uniformBuffersMemory[i]
		
		);
		createBuffer(
		
			bufferSize,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			lightUniformBuffers[i],
			lightUniformBuffersMemory[i]
		
		);
	
	}*/

}

/*
*	Function:		void updateUniformBuffers(uint32_t currentImage_)
*	Purpose:		Updates uniform buffers (transformation matrices) every frame
*
*/
void Engine::updateUniformBuffers(uint32_t currentImage_) {
	
	static auto startTime								= std::chrono::high_resolution_clock::now();
	auto currentTime									= std::chrono::high_resolution_clock::now();
	float time											= std::chrono::duration< float, std::chrono::seconds::period >(currentTime - startTime).count();
	
	objectPipeline.ubo.model							= glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	objectPipeline.ubo.model							= glm::rotate(objectPipeline.ubo.model, time * glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	objectPipeline.ubo.view								= camera.getViewMatrix();
	objectPipeline.ubo.proj								= glm::perspective(glm::radians(camera.zoom), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 100.0f);
	objectPipeline.ubo.proj[1][1]						*= -1;

	objectPipeline.updateUBOs(currentImage_);

	objectPipeline.lbo.lightColor						= glm::vec3(1.0f, 1.0f, 1.0f);
	objectPipeline.lbo.objectColor						= glm::vec3(23.0f / 255.0f, 166.0f / 255.0f, 255.0f / 255.0f);		// R, G, B

	objectPipeline.updateLBOs(currentImage_);
	
	lightingPipeline.ubo.model							= glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	lightingPipeline.ubo.model							= glm::translate(lightingPipeline.ubo.model, glm::vec3(10.0));
	lightingPipeline.ubo.model							= glm::rotate(lightingPipeline.ubo.model, time * glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	lightingPipeline.ubo.view							= camera.getViewMatrix();
	lightingPipeline.ubo.proj							= glm::perspective(glm::radians(camera.zoom), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 100.0f);
	lightingPipeline.ubo.proj[1][1]						*= -1;

	lightingPipeline.updateUBOs(currentImage_);

}

/*
*	Function:		void createDescriptorPool()
*	Purpose:		Creates the descriptor pool for descriptor set creation
*
*/
void Engine::createDescriptorPool(void) {

	std::array< VkDescriptorPoolSize, 2 > poolSizes			= {};
	poolSizes[0].type										= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount							= static_cast< uint32_t >(swapChainImages.size());
	poolSizes[1].type										= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[1].descriptorCount							= static_cast< uint32_t >(swapChainImages.size());

	VkDescriptorPoolCreateInfo poolInfo						= {};
	poolInfo.sType											= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount									= static_cast< uint32_t >(poolSizes.size());
	poolInfo.pPoolSizes										= poolSizes.data();
	poolInfo.maxSets										= static_cast< uint32_t >(swapChainImages.size());

	if (vkCreateDescriptorPool(
	
		device,
		&poolInfo,
		nullptr,
		&descriptorPool
	
	) != VK_SUCCESS) {
	
		logger.log(ERROR_LOG, "Failed to create descriptor pool!");
	
	}

	std::array< VkDescriptorPoolSize, 1 > lightingPoolSizes					= {};
	lightingPoolSizes[0].type												= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	lightingPoolSizes[0].descriptorCount									= static_cast< uint32_t >(swapChainImages.size());

	VkDescriptorPoolCreateInfo lightingPoolInfo								= {};
	lightingPoolInfo.sType													= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	lightingPoolInfo.poolSizeCount											= static_cast< uint32_t >(lightingPoolSizes.size());
	lightingPoolInfo.pPoolSizes												= lightingPoolSizes.data();
	lightingPoolInfo.maxSets												= static_cast< uint32_t >(swapChainImages.size());

	if (vkCreateDescriptorPool(

		device,
		&lightingPoolInfo,
		nullptr,
		&lightingDescriptorPool

	) != VK_SUCCESS) {

		logger.log(ERROR_LOG, "Failed to create descriptor pool!");

	}

}

/*
*	Function:		void createDescriptorSets()
*	Purpose:		Finally creates the descriptor sets (for each image_ in the swapchain)
*	
*/
void Engine::createDescriptorSets(void) {

	/*std::vector< VkDescriptorSetLayout > layouts(swapChainImages.size(), objectDescriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo		= {};
	allocInfo.sType								= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool					= descriptorPool;
	allocInfo.descriptorSetCount				= static_cast< uint32_t >(swapChainImages.size());
	allocInfo.pSetLayouts						= layouts.data();

	objectDescriptorSets.resize(swapChainImages.size());

	if (vkAllocateDescriptorSets(
	
		device,
		&allocInfo,
		objectDescriptorSets.data()

	) != VK_SUCCESS) {
	
		logger.log(ERROR_LOG, "Failed to allocate descriptor sets!");
	
	}

	for (size_t i = 0; i < swapChainImages.size(); i++) {
	
		VkDescriptorBufferInfo bufferInfo									= {};
		bufferInfo.buffer													= uniformBuffers[i];
		bufferInfo.offset													= 0;
		bufferInfo.range													= sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo										= {};
		imageInfo.imageLayout												= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView													= textureImageView;
		imageInfo.sampler													= textureSampler;
		
		std::array< VkWriteDescriptorSet, 2> descriptorWrites				= {};
		descriptorWrites[0].sType											= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet											= objectDescriptorSets[i];
		descriptorWrites[0].dstBinding										= 0;
		descriptorWrites[0].dstArrayElement									= 0;
		descriptorWrites[0].descriptorType									= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount									= 1;
		descriptorWrites[0].pBufferInfo										= &bufferInfo;
		descriptorWrites[1].sType											= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet											= objectDescriptorSets[i];
		descriptorWrites[1].dstBinding										= 1;
		descriptorWrites[1].dstArrayElement									= 0;
		descriptorWrites[1].descriptorType									= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount									= 1;
		descriptorWrites[1].pImageInfo										= &imageInfo;

		vkUpdateDescriptorSets(
		
			device,
			static_cast< uint32_t >(descriptorWrites.size()),
			descriptorWrites.data(),
			0,
			nullptr
		
		);

	}

	std::vector< VkDescriptorSetLayout > lightingLayouts(swapChainImages.size(), lightingDescriptorSetLayout);
	VkDescriptorSetAllocateInfo lightingAllocInfo		= {};
	lightingAllocInfo.sType										= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	lightingAllocInfo.descriptorPool							= lightingDescriptorPool;
	lightingAllocInfo.descriptorSetCount						= static_cast< uint32_t >(swapChainImages.size());
	lightingAllocInfo.pSetLayouts								= lightingLayouts.data();

	lightingDescriptorSets.resize(swapChainImages.size());

	if (vkAllocateDescriptorSets(

		device,
		&lightingAllocInfo,
		lightingDescriptorSets.data()

	) != VK_SUCCESS) {

		logger.log(ERROR_LOG, "Failed to allocate descriptor sets!");

	}

	for (size_t i = 0; i < swapChainImages.size(); i++) {

		VkDescriptorBufferInfo bufferInfo							= {};
		bufferInfo.buffer											= lightUniformBuffers[i];
		bufferInfo.offset											= 0;
		bufferInfo.range											= sizeof(UniformBufferObject);

		std::array< VkWriteDescriptorSet, 1> descriptorWrites		= {};
		descriptorWrites[0].sType									= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet									= lightingDescriptorSets[i];
		descriptorWrites[0].dstBinding								= 0;
		descriptorWrites[0].dstArrayElement							= 0;
		descriptorWrites[0].descriptorType							= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount							= 1;
		descriptorWrites[0].pBufferInfo								= &bufferInfo;

		vkUpdateDescriptorSets(

			device,
			static_cast< uint32_t >(descriptorWrites.size()),
			descriptorWrites.data(),
			0,
			nullptr

		);

	}*/

}

/*
*	Function:		void createTextureImage()
*	Purpose:		Generates a texture image_
*
*/
void Engine::createTextureImage(void) {

	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(
	
		TEXTURE_PATH.c_str(),
		&texWidth,
		&texHeight,
		&texChannels,
		STBI_rgb_alpha

	);

	mipLevels = static_cast< uint32_t >(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels) {
	
		logger.log(ERROR_LOG, "Failed to load texture image at path " + TEXTURE_PATH + "!");
	
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(

		imageSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory

	);

	void* data;
	vkMapMemory(
	
		device,
		stagingBufferMemory,
		0,
		imageSize,
		0,
		&data

	);

	memcpy(

		data,
		pixels,
		static_cast< size_t >(imageSize)

	);

	vkUnmapMemory(device, stagingBufferMemory);

	stbi_image_free(pixels);

	createImage(

		texWidth,
		texHeight,
		mipLevels,
		VK_SAMPLE_COUNT_1_BIT,
		VK_FORMAT_R8G8B8A8_UNORM, 
		VK_IMAGE_TILING_OPTIMAL, 
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		textureImage, 
		textureImageMemory
	
	);

	transitionImageLayout(
		
		textureImage,
		VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		mipLevels
	
	);
	
	copyBufferToImage(
		
		stagingBuffer,
		textureImage,
		static_cast< uint32_t >(texWidth), 
		static_cast< uint32_t >(texHeight)
	
	);

	vkDestroyBuffer(
		
		device, 
		stagingBuffer, 
		nullptr
	
	);
	vkFreeMemory(
		
		device, 
		stagingBufferMemory, 
		nullptr
	
	);

	generateMipmaps(
	
		textureImage,
		VK_FORMAT_R8G8B8A8_UNORM,
		texWidth,
		texHeight,
		mipLevels
	
	);

}

/*
*	Function:		void createImage(
*	
*						uint32_t					width_,
*						uint32_t					height_,
*						uint32_t					mipLevels_, 
*						VkSampleCountFlagBits		numSamples_,
*						VkFormat					format_,
*						VkImageTiling				tiling_, 
*						VkImageUsageFlags			usage_, 
*						VkMemoryPropertyFlags		properties_,
*						VkImage&					image_, 
*						VkDeviceMemory&				imageMemory_
*
*					)
*	Purpose:		Creates a texture image_ handle
*
*/
void Engine::createImage(

	uint32_t					width_,
	uint32_t					height_,
	uint32_t					mipLevels_,
	VkSampleCountFlagBits		numSamples_,
	VkFormat					format_,
	VkImageTiling				tiling_, 
	VkImageUsageFlags			usage_, 
	VkMemoryPropertyFlags		properties_,
	VkImage&					image_, 
	VkDeviceMemory&				imageMemory_

) {

	VkImageCreateInfo imageInfo		= {};
	imageInfo.sType					= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType				= VK_IMAGE_TYPE_2D;
	imageInfo.extent.width			= width_;
	imageInfo.extent.height			= height_;
	imageInfo.extent.depth			= 1;
	imageInfo.mipLevels				= mipLevels_;
	imageInfo.arrayLayers			= 1;
	imageInfo.format				= format_;
	imageInfo.tiling				= tiling_;
	imageInfo.initialLayout			= VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage					= usage_;
	imageInfo.samples				= numSamples_;
	imageInfo.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(
		
		device, 
		&imageInfo,
		nullptr, 
		&image_
	
	) != VK_SUCCESS) {

		logger.log(ERROR_LOG, "Failed to create image!");

	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(
		
		device, 
		image_,
		&memRequirements
	
	);

	VkMemoryAllocateInfo allocInfo		= {};
	allocInfo.sType						= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize			= memRequirements.size;
	allocInfo.memoryTypeIndex			= findMemoryType(memRequirements.memoryTypeBits, properties_);

	if (vkAllocateMemory(
		
		device, 
		&allocInfo, 
		nullptr,
		&imageMemory_
	
	) != VK_SUCCESS) {

		logger.log(ERROR_LOG, "Failed to allocate image memory!");

	}

	vkBindImageMemory(
		
		device,
		image_, 
		imageMemory_, 
		0
	
	);

}

/*
*	Function:		VkCommandBuffer beginSingleTimeCommands()
*	Purpose:		Creates a one time use command buffer_
*
*/
VkCommandBuffer Engine::beginSingleTimeCommands() {

	VkCommandBufferAllocateInfo allocInfo		= {};
	allocInfo.sType								= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level								= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool						= commandPool;
	allocInfo.commandBufferCount				= 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(
		
		device, 
		&allocInfo,
		&commandBuffer
	
	);

	VkCommandBufferBeginInfo beginInfo		= {};
	beginInfo.sType							= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags							= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;

}

/*
*	Function:		void endSingleTimeCommands(VkCommandBuffer commandBuffer_)
*	Purpose:		Ends execution of command buffer_
*	
*/
void Engine::endSingleTimeCommands(VkCommandBuffer commandBuffer_) {

	vkEndCommandBuffer(commandBuffer_);

	VkSubmitInfo submitInfo			= {};
	submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount	= 1;
	submitInfo.pCommandBuffers		= &commandBuffer_;

	vkQueueSubmit(
		
		graphicsQueue,
		1,
		&submitInfo,
		VK_NULL_HANDLE
	
	);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(
		
		device,
		commandPool,
		1, 
		&commandBuffer_
	
	);

}

/*
*	Function:		void transitionImageLayout(
*
*						VkImage				image_,
*						VkFormat			format_, 
*						VkImageLayout		oldLayout_,
*						VkImageLayout		newLayout_,
*						uint32_t			mipLevels_
*
*					)
*	Purpose:		Handles image_ layout transitions
*
*/
void Engine::transitionImageLayout(
	
	VkImage				image_,
	VkFormat			format_,
	VkImageLayout		oldLayout_,
	VkImageLayout		newLayout_,
	uint32_t			mipLevels_

) {

	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier					= {};
	barrier.sType									= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout								= oldLayout_;
	barrier.newLayout								= newLayout_;
	barrier.srcQueueFamilyIndex						= VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex						= VK_QUEUE_FAMILY_IGNORED;
	barrier.image									= image_;
	barrier.subresourceRange.aspectMask				= VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel			= 0;
	barrier.subresourceRange.levelCount				= mipLevels_;
	barrier.subresourceRange.baseArrayLayer			= 0;
	barrier.subresourceRange.layerCount				= 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (newLayout_ == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
	
		barrier.subresourceRange.aspectMask			= VK_IMAGE_ASPECT_DEPTH_BIT;

		if (hasStencilComponent(format_)) {
		
			barrier.subresourceRange.aspectMask	   |= VK_IMAGE_ASPECT_STENCIL_BIT;

		}

	}
	else {
	
		barrier.subresourceRange.aspectMask			= VK_IMAGE_ASPECT_COLOR_BIT;

	}

	if (oldLayout_ == VK_IMAGE_LAYOUT_UNDEFINED && newLayout_ == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {

		barrier.srcAccessMask		= 0;
		barrier.dstAccessMask		= VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage					= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage			= VK_PIPELINE_STAGE_TRANSFER_BIT;

	}
	else if (oldLayout_ == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout_ == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {

		barrier.srcAccessMask		= VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask		= VK_ACCESS_SHADER_READ_BIT;

		sourceStage					= VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage			= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

	}
	else if (oldLayout_ == VK_IMAGE_LAYOUT_UNDEFINED && newLayout_ == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
	
		barrier.srcAccessMask		= 0;
		barrier.dstAccessMask		= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage					= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage			= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

	}
	else if (oldLayout_ == VK_IMAGE_LAYOUT_UNDEFINED && newLayout_ == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
	
		barrier.srcAccessMask		= 0;
		barrier.dstAccessMask		= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage					= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage			= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	
	}
	else {

		throw std::invalid_argument("Unsupported layout transition!");

	}

	vkCmdPipelineBarrier(

		commandBuffer,
		sourceStage, 
		destinationStage,
		0,
		0,
		nullptr,
		0,
		nullptr,
		1,
		&barrier

	);

	endSingleTimeCommands(commandBuffer);

}

/*
*	Function:		copyBufferToImage(
*	
*						VkBuffer		buffer_,
*						VkImage			image_,
*						uint32_t		width_, 
*						uint32_t		height_
*
*					)
*	Purpose:		Copies image_ info from buffer_ into image_
*
*/
void Engine::copyBufferToImage(
	
	VkBuffer		buffer_,
	VkImage			image_,
	uint32_t		width_, 
	uint32_t		height_

) {

	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferImageCopy region					= {};
	region.bufferOffset							= 0;
	region.bufferRowLength						= 0;
	region.bufferImageHeight					= 0;
	region.imageSubresource.aspectMask			= VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel			= 0;
	region.imageSubresource.baseArrayLayer		= 0;
	region.imageSubresource.layerCount			= 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {

		width_,
		height_,
		1

	};

	vkCmdCopyBufferToImage(

		commandBuffer,
		buffer_,
		image_,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	
	);

	endSingleTimeCommands(commandBuffer);

}

/*
*	Function:		void createTextureImageView()
*	Purpose:		Creates image_ view for texture
*
*/
void Engine::createTextureImageView(void) {

	textureImageView = createImageView(
		
		textureImage,
		VK_FORMAT_R8G8B8A8_UNORM, 
		VK_IMAGE_ASPECT_COLOR_BIT,
		mipLevels
	
	);

}

/*
*	Function:		VkImageView Engine::createImageView(
*	
*						VkImage					image_,
*						VkFormat				format_, 
*						VkImageAspectFlags		aspectFlags_,
*						uint32_t				mipLevels_
*
*					)
*	Purpose:		Creates a VkImageView
*
*/
VkImageView Engine::createImageView(

	VkImage					image_,
	VkFormat				format_,
	VkImageAspectFlags		aspectFlags_,
	uint32_t				mipLevels_

) {

	VkImageViewCreateInfo viewInfo					= {};
	viewInfo.sType									= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image									= image_;
	viewInfo.viewType								= VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format									= format_;
	viewInfo.subresourceRange.aspectMask			= aspectFlags_;
	viewInfo.subresourceRange.baseMipLevel			= 0;
	viewInfo.subresourceRange.levelCount			= mipLevels_;
	viewInfo.subresourceRange.baseArrayLayer		= 0;
	viewInfo.subresourceRange.layerCount			= 1;

	VkImageView imageView;
	if (vkCreateImageView(
		
		device,
		&viewInfo, 
		nullptr,
		&imageView
	
	) != VK_SUCCESS) {

		logger.log(ERROR_LOG, "Failed to create texture image view!");

	}

	return imageView;

}

/*
*	Function:		void createTextureSampler()
*	Purpose:		Creates a texture sampler
*
*/
void Engine::createTextureSampler(void) {

	VkSamplerCreateInfo samplerInfo			= {};
	samplerInfo.sType						= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter					= VK_FILTER_LINEAR;
	samplerInfo.minFilter					= VK_FILTER_LINEAR;
	samplerInfo.addressModeU				= VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV				= VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW				= VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable			= VK_TRUE;
	samplerInfo.maxAnisotropy				= 16;
	samplerInfo.borderColor					= VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates		= VK_FALSE;
	samplerInfo.compareEnable				= VK_FALSE;
	samplerInfo.compareOp					= VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode					= VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias					= 0.0f;
	samplerInfo.minLod						= 0.0f;
	samplerInfo.maxLod						= static_cast< float >(mipLevels);

	if (vkCreateSampler(
	
		device,
		&samplerInfo,
		nullptr,
		&textureSampler

	) != VK_SUCCESS) {
	
		logger.log(ERROR_LOG, "Failed to create texture sampler!");
	
	}

}

/*
*	Function:		void createDepthResources()
*	Purpose:		Creates depth buffer_
*	
*/
void Engine::createDepthResources(void) {

	VkFormat depthFormat = findDepthFormat();

	createImage(
		
		swapChainExtent.width, 
		swapChainExtent.height,
		1,
		msaaSamples,
		depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		depthImage, 
		depthImageMemory
	
	);

	depthImageView = createImageView(
		
		depthImage,
		depthFormat,
		VK_IMAGE_ASPECT_DEPTH_BIT,
		1

	);

	transitionImageLayout(
	
		depthImage, 
		depthFormat,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		1

	);

}

/*
*	Function:		VkFormat findSupportedFormat(
*
*						const std::vector< VkFormat >&			candidates_,
*						VkImageTiling							tiling_,
*						VkFormatFeatureFlags					features_
*
*					)
*	Purpose:		Finds device-supported formats
*
*/
VkFormat Engine::findSupportedFormat(

	const std::vector< VkFormat >&			candidates_,
	VkImageTiling							tiling_,
	VkFormatFeatureFlags					features_

) {

	for (VkFormat format : candidates_) {
	
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(

			physicalDevice,
			format,
			&props

		);

		if (tiling_ == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features_) == features_) {

			return format;

		}
		else if (tiling_ == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features_) == features_) {

			return format;

		}

		logger.log(ERROR_LOG, "Failed to find supported format!");
	
		return VK_FORMAT_UNDEFINED;

	}

	return VK_FORMAT_UNDEFINED;

}

/*
*	Function:		VkFormat findDepthFormat()
*	Purpose:		Finds the best format for depth buffering
*
*/
VkFormat Engine::findDepthFormat() {

	return findSupportedFormat(
	
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	
	);
}

/*
*	Function:		bool hasStencilComponent(VkFormat format_)
*	Purpose:		Checks whether a format_ has a stencil component
*
*/
bool Engine::hasStencilComponent(VkFormat format_) {

	return format_ == VK_FORMAT_D32_SFLOAT_S8_UINT || format_ == VK_FORMAT_D24_UNORM_S8_UINT;

}

/*
*	Function:		void loadModels()
*	Purpose:		Loads vertex data from file
*
*/
void Engine::loadModels(void) {

	renderedCube		= new Cube();
	lightingCube		= new Cube();

}

/*
*	Function:		void keyboardInputCallback(
*		
*						GLFWwindow*			window_, 
*						int					key_, 
*						int					scancode_,
*						int					action_,
*						int					mods_
*	
*					)
*	Purpose:		Keyboard input callback for GLFW
*	
*/
void Engine::keyboardInputCallback(

	GLFWwindow*			window_,
	int					key_,
	int					scancode_,
	int					action_,
	int					mods_

) {

	static unsigned int consoleCount = 0;

	if (action_ == GLFW_PRESS) {

		switch (key_) {

			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window_, GLFW_TRUE);
				break;
			case GLFW_KEY_LEFT_CONTROL:
				engine.camera.disableInput();
				glfwSetInputMode(

					engine.window,
					GLFW_CURSOR,
					GLFW_CURSOR_NORMAL

				);
				break;
#ifdef GAME_DEVELOPMENT
			case GLFW_KEY_C:
				if (consoleCount == 0) {
				
					logger.showConsoleWindow();
					consoleCount++;
				
				}
				else if (consoleCount == 1) {
				
					logger.hideConsoleWindow();
					consoleCount = 0;
				
				}
				break;
#endif
			default:
				break;

		}
	
	}
	else if (action_ == GLFW_RELEASE) {
	
		switch(key_) {
		
			case GLFW_KEY_LEFT_CONTROL:
				engine.camera.enableInput();
				glfwSetInputMode(

					engine.window,
					GLFW_CURSOR,
					GLFW_CURSOR_DISABLED

				);
				break;
			default:
				break;

		}
	
	}

}

/*
*	Function:		uint32_t getNumThreads()
*	Purpose:		Returns the maximum number of simultaneosly detachable threads
*
*/
uint32_t Engine::getNumThreads(void){

	return std::thread::hardware_concurrency();

}

/*
*	Function:		void generateMipmaps(
*	
*						VkImage			image_,
*						VkFormat		imageFormat_,
*						int32_t			texWidth_,
*						int32_t			texHeight_,
*						uint32_t		mipLevels_
*	
*					)
*	Purpose:		Generates MipMaps of a VkImage
*
*/
void Engine::generateMipmaps(

	VkImage			image_,
	VkFormat		imageFormat_,
	int32_t			texWidth_,
	int32_t			texHeight_,
	uint32_t		mipLevels_

) {

	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(
		
		physicalDevice, 
		imageFormat_,
		&formatProperties
	
	);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {

		logger.log(ERROR_LOG, "Texture image format does not support linear blitting!");

	}

	VkCommandBuffer commandBuffer					= beginSingleTimeCommands();

	VkImageMemoryBarrier barrier					= {};
	barrier.sType									= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image									= image_;
	barrier.srcQueueFamilyIndex						= VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex						= VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask				= VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer			= 0;
	barrier.subresourceRange.layerCount				= 1;
	barrier.subresourceRange.levelCount				= 1;

	int32_t mipWidth								= texWidth_;
	int32_t mipHeight								= texHeight_;

	for (uint32_t i = 1; i < mipLevels_; i++) {

		barrier.subresourceRange.baseMipLevel		= i - 1;
		barrier.oldLayout							= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout							= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask						= VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask						= VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(
			
			commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0, 
			nullptr,
			0,
			nullptr,
			1, 
			&barrier
		
		);

		VkImageBlit blit						= {};
		blit.srcOffsets[0]						= { 0, 0, 0 };
		blit.srcOffsets[1]						= { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask			= VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel			= i - 1;
		blit.srcSubresource.baseArrayLayer		= 0;
		blit.srcSubresource.layerCount			= 1;
		blit.dstOffsets[0]						= { 0, 0, 0 };
		blit.dstOffsets[1]						= { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask			= VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel			= i;
		blit.dstSubresource.baseArrayLayer		= 0;
		blit.dstSubresource.layerCount			= 1;

		vkCmdBlitImage(
			
			commandBuffer,
			image_,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			image_,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, 
			&blit,
			VK_FILTER_LINEAR
		
		);

		barrier.oldLayout						= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout						= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask					= VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask					= VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(
			
			commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, 
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0,
			0, 
			nullptr,
			0, 
			nullptr,
			1, 
			&barrier
		
		);

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;

	}

	barrier.subresourceRange.baseMipLevel		= mipLevels_ - 1;
	barrier.oldLayout							= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout							= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask						= VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask						= VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(
		
		commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, 
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		0, 
		nullptr,
		0, 
		nullptr,
		1, 
		&barrier
	
	);

	endSingleTimeCommands(commandBuffer);

}

/*
*	Function:		void getMaxUsableSampleCount()
*	Purpose:		Fetches maximum multisampling sample count
*
*/
VkSampleCountFlagBits Engine::getMaxUsableSampleCount(void) {

	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

	VkSampleCountFlags counts = std::min(physicalDeviceProperties.limits.framebufferColorSampleCounts, physicalDeviceProperties.limits.framebufferDepthSampleCounts);

	if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
	if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
	if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
	if (counts & VK_SAMPLE_COUNT_8_BIT)  { return VK_SAMPLE_COUNT_8_BIT;  }
	if (counts & VK_SAMPLE_COUNT_4_BIT)  { return VK_SAMPLE_COUNT_4_BIT;  }
	if (counts & VK_SAMPLE_COUNT_2_BIT)  { return VK_SAMPLE_COUNT_2_BIT;  }

	return VK_SAMPLE_COUNT_1_BIT;

}

/*
*	Function:		void createColorResources()
*	Purpose:		Creates a multisampled color buffer
*
*/
void Engine::createColorResources(void) {

	VkFormat colorFormat = swapChainImageFormat;

	createImage(
	
		swapChainExtent.width,
		swapChainExtent.height,
		1,
		msaaSamples,
		colorFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		colorImage,
		colorImageMemory
	
	);

	colorImageView = createImageView(
	
		colorImage,
		colorFormat,
		VK_IMAGE_ASPECT_COLOR_BIT, 
		1

	);

	transitionImageLayout(
	
		colorImage,
		colorFormat,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		1

	);

}

/*
*	Function:		void mouseCallback(
*	
*						GLFWwindow*			window_,
*						double				xPos_,
*						double				yPos_
*
*					)
*	Purpose:		Mouse movement callback function for GLFW
*
*/
void Engine::mouseCallback(
	
	GLFWwindow*			window_,
	double				xPos_,
	double				yPos_

) {

	if (engine.firstMouse) {
	
		engine.lastX			= xPos_;
		engine.lastY			= yPos_;
		engine.firstMouse		= false;
	
	}

	double xOffset			= xPos_ - engine.lastX;
	double yOffset			= -(yPos_ - engine.lastY);
	engine.lastX			= xPos_;
	engine.lastY			= yPos_;

	engine.camera.processMouseMovement(static_cast< float >(xOffset), static_cast< float >(yOffset));

}

/*
*	Function:		void scrollCallback(
*
*						GLFWwindow*			window_,
*						double				xOffset_,
*						double				yOffset_
*
*					)
*	Purpose:		Scroll callback function for GLFW
*
*/
void Engine::scrollCallback(
	
	GLFWwindow*			window_,
	double				xOffset_,
	double				yOffset_

) {

	engine.camera.processMouseScroll(static_cast< float >(yOffset_));

}

/*
*	Function:		void createCamera()
*	Purpose:		Creates the camera object
*
*/
void Engine::createCamera(void) {
	
	camera = Camera(

		glm::vec3(
			
			0.0f,
			0.0f, 
			3.0f
		
		),
		glm::vec3(

			0.0f,
			1.0f,
			0.0f
		
		),
		-90.0f,
		0.0f
	);

}

/*
*	Function:		void queryKeyboardGLFW()
*	Purpose:		Checks each frame whether movement key are pressed
*
*/
void Engine::queryKeyboardGLFW(void) {

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {

		camera.processKeyboard(FORWARD, static_cast< float >(DELTATIME));

	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {

		camera.processKeyboard(BACKWARD, static_cast< float >(DELTATIME));

	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {

		camera.processKeyboard(LEFT, static_cast< float >(DELTATIME));

	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {

		camera.processKeyboard(RIGHT, static_cast< float >(DELTATIME));

	}

}

/*
*	Function:		void init3DAudio()
*	Purpose:		Initializes the 3D audio library (irrKlang)
*
*/
void Engine::init3DAudio(void) {

	audioEngine = irrklang::createIrrKlangDevice();
	if (!audioEngine) {
	
		logger.log(ERROR_LOG, "Failed to find audio device!");
	
	}

}