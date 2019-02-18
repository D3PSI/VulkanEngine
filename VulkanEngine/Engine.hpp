/*
*	File:		Engine.hpp
*	
*
*/
#pragma once
#include "VERSION.cpp"
#define NOMINMAX
#include <Windows.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <SDL.h>
#undef main
#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <irrKlang.h>

#include <string>
#include <map>
#include <algorithm>
#include <set>
#include <vector>
#include <optional>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <conio.h>
#include <memory>
#include <thread>

#include "Logger.hpp"
#include "QueueFamilyIndices.cpp"
#include "SwapChainSupportDetails.cpp"
#include "Vertex.cpp"
#include "Hash.cpp"
#include "UniformBufferObject.cpp"
#include "Camera.hpp"
#include "StartWindow.hpp"
#include "ConsoleColor.hpp"
#include "Object.hpp"
#include "Model.hpp"
#include "CubeVertex.cpp"
#include "Pipeline.hpp"
#include "LightingBufferObject.cpp"
#include "Cube.hpp"

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

#define WIDTH 1280
#define HEIGHT 780

extern Logger											logger;

namespace game {

	VkResult CreateDebugUtilsMessengerEXT(

		VkInstance										instance_,
		const VkDebugUtilsMessengerCreateInfoEXT*		pCreateInfo_,
		const VkAllocationCallbacks*					pAllocator_,
		VkDebugUtilsMessengerEXT*						pCallback_

	);
	void DestroyDebugUtilsMessengerEXT(

		VkInstance										instance_,
		VkDebugUtilsMessengerEXT						callback_,
		const VkAllocationCallbacks*					pAllocator_

	);
	std::vector< char > readFile(const std::string& filename_);
	int init();

}

class Engine
{
public:
	Camera												camera;
	VkDevice											device;
	bool												firstMouse						= true;
	double												lastX							= WIDTH / 2;
	double												lastY							= HEIGHT / 2;
	std::mutex											closeStartWindow;
	const std::string									TITLE							= "VULKANENGINE by D3PSI\0";
	const unsigned int									MAX_FRAMES_IN_FLIGHT			= 2;
	float												loadingProgress					= 0.0f;
	double												DELTATIME;
	VkDescriptorPool									descriptorPool;
	std::vector< VkImage >								swapChainImages;

	void run(void); 
	uint32_t findMemoryType(uint32_t typeFilter_, VkMemoryPropertyFlags properties_);
	void createBuffer(

		VkDeviceSize				size_,
		VkBufferUsageFlags			usage_,
		VkMemoryPropertyFlags		properties_,
		VkBuffer&					buffer_,
		VkDeviceMemory&				bufferMemory_

	);
	void copyBuffer(

		VkBuffer		srcBuffer_,
		VkBuffer		dstBuffer_,
		VkDeviceSize	size_

	);
private:
	VkResult											result;
	GLFWwindow*											window;
	StartWindow*										startWindow;
	const std::string									CHALET_PATH						= "res/models/chalet/source/chaletblend.obj";
	const std::string									TEXTURE_PATH					= "res/models/chalet/textures/chalet.jpg";
	GLFWmonitor*										monitor							= nullptr; 
	uint32_t											numThreads;
	const std::vector< const char* >					validationLayers				= {
	
		"VK_LAYER_LUNARG_standard_validation"
	
	};
	const std::vector< const char* >					deviceExtensions				= {
	
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	
	};
	VkInstance											instance;
	VkPhysicalDevice									physicalDevice					= VK_NULL_HANDLE;
	VkSurfaceKHR										surface;
	VkQueue												graphicsQueue;
	VkQueue												presentQueue;
	VkDebugUtilsMessengerEXT							callback;
	VkSwapchainKHR										swapChain;
	VkFormat											swapChainImageFormat;
	VkColorSpaceKHR										swapChainImageColorSpace;
	VkExtent2D											swapChainExtent;
	std::vector< VkImageView >							swapChainImageViews;
	VkRenderPass										renderPass;
	std::vector< VkFramebuffer >						swapChainFramebuffers;
	VkCommandPool										commandPool;
	std::vector< VkCommandBuffer >						commandBuffers;
	std::vector< VkSemaphore >							imageAvailableSemaphores;
	std::vector< VkSemaphore >							renderFinishedSemaphores;
	std::vector< VkFence >								inFlightFences;
	size_t												currentFrame					= 0;
	bool												framebufferResized				= false;
	clock_t												current_ticks, delta_ticks;
	clock_t												fps								= 0;
	uint32_t											mipLevels;
	VkImage												textureImage;
	VkDeviceMemory										textureImageMemory;
	VkImageView											textureImageView;
	VkSampler											textureSampler;
	VkDescriptorPool									lightingDescriptorPool;
	VkImage												depthImage;
	VkDeviceMemory										depthImageMemory;
	VkImageView											depthImageView;
	VkSampleCountFlagBits								msaaSamples						= VK_SAMPLE_COUNT_64_BIT;
	VkImage												colorImage;
	VkDeviceMemory										colorImageMemory;
	VkImageView											colorImageView;
	const float											maxFPS							= 60.0f;
	const float											maxPeriod						= 1.0f / maxFPS; 

	Pipeline											objectPipeline;
	Pipeline											lightingPipeline;

	Object*												chalet;
	Cube*												lightingCube;

	irrklang::ISoundEngine*								audioEngine;
	irrklang::ISound*									bgmusic;
	irrklang::ISound*									effect;

	void initStartWindow(void);
	void initWindow(void);
	void initVulkan(void);
	void createInstance(void);
	void mainLoop(void);
	void cleanup(void);
	bool checkValidationLayerSupport(void);
	std::vector< const char* > getRequiredExtensions(void);

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(

		VkDebugUtilsMessageSeverityFlagBitsEXT			messageSeverity_,
		VkDebugUtilsMessageTypeFlagsEXT					messageType_,
		const VkDebugUtilsMessengerCallbackDataEXT*		pCallbackData_,
		void*											pUserData_

	);
	void setupDebugCallback(void);
	void pickPhysicalDevice(void);
	bool isDeviceSuitable(VkPhysicalDevice device_);
	void createLogicalDevice(void);
	void createSurface(void);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device_);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device_);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device_);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector< VkSurfaceFormatKHR >& availableFormats_);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector< VkPresentModeKHR > availablePresentModes_);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilites_);
	void createSwapChain(void);
	void createImageViews(void);
	VkShaderModule createShaderModule(const std::vector< char >& code_);
	void createPipelines(void);
	void createRenderPass(void);
	void createFramebuffers(void);
	void createCommandPool(void);
	void recordCommandBuffers(void);
	void createSyncObjects(void);
	void renderFrame(void);
	void recreateSwapChain(void);
	void cleanupSwapChain(void);
	static void framebufferResizeCallback(
		
		GLFWwindow*		window_, 
		int				width_, 
		int				eight_
	
	);
	void createDescriptorSetLayout(void);
	void createUniformBuffers(void);
	void updateUniformBuffers(uint32_t currentImage_);
	void createDescriptorPool(void);
	void createDescriptorSets(void);
	void createTextureImage(void);
	void createImage(

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

	);
	VkCommandBuffer beginSingleTimeCommands(void);
	void endSingleTimeCommands(VkCommandBuffer commandBuffer_);
	void transitionImageLayout(
		
		VkImage				image_, 
		VkFormat			format_,
		VkImageLayout		oldLayout_, 
		VkImageLayout		newLayout_,
		uint32_t			mipLevels_
	
	);
	void copyBufferToImage(

		VkBuffer		buffer_,
		VkImage			image_,
		uint32_t		width_,
		uint32_t		height_

	);
	void createTextureImageView(void);
	VkImageView createImageView(

		VkImage					image_,
		VkFormat				format_,
		VkImageAspectFlags		aspectFlags_,
		uint32_t				mipLevels_

	);
	void createTextureSampler(void);
	void createDepthResources(void);
	VkFormat findSupportedFormat(
	
		const std::vector< VkFormat >&			candidates_, 
		VkImageTiling							tiling_, 
		VkFormatFeatureFlags					features_

	);
	VkFormat findDepthFormat(void);
	bool hasStencilComponent(VkFormat format_);
	void loadModels(void);
	static void keyboardInputCallback(
		
		GLFWwindow*			window_, 
		int					key_, 
		int					scancode_,
		int					action_,
		int					mods_
	
	);
	uint32_t getNumThreads(void);
	void generateMipmaps(
	
		VkImage			image_,
		VkFormat		imageFormat_,
		int32_t			texWidth_,
		int32_t			texHeight_,
		uint32_t		mipLevels_
	
	);
	VkSampleCountFlagBits getMaxUsableSampleCount(void);
	void createColorResources(void);
	static void mouseCallback(

		GLFWwindow*			window_,
		double				xPos_,
		double				yPos_

	);
	static void scrollCallback(
	
		GLFWwindow*			window_,
		double				xOffset_,
		double				yOffset_
	
	);
	void createCamera(void);
	void queryKeyboardGLFW(void);
	void init3DAudio(void);

};

extern Engine											engine;