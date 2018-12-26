/*
*	File:		Engine.hpp
*	
*
*/
#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <map>
#include <algorithm>
#include <set>
#include <vector>
#include <optional>
#include <iostream>
#include <chrono>

#include "Logger.hpp"
#include "QueueFamilyIndices.cpp"
#include "SwapChainSupportDetails.cpp"
#include "Vertex.cpp"
#include "UniformBufferObject.cpp"

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

extern Logger									logger;

namespace game {

	extern const unsigned int MAX_FRAMES_IN_FLIGHT;

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
	std::vector< char > readFile(const std::string& filename);
	int init();

}

const std::vector<Vertex> vertices = {

	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	{{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}}

};

const std::vector<uint16_t> indices = {

	0, 1, 2, 2, 3, 0

};

class Engine
{
public:
	void run(void);
private:
	VkResult									result;
	GLFWwindow*									window;
	const unsigned int							width							= 1280;
	const unsigned int							height							= 780;
	const std::string							title							= "VULKAN by D3PSI";
	GLFWmonitor*								monitor							= nullptr; 
	const std::vector< const char* >			validationLayers				= {
	
		"VK_LAYER_LUNARG_standard_validation"
	
	};
	const std::vector< const char* >			deviceExtensions				= {
	
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	
	};
	VkInstance									instance;
	VkPhysicalDevice							physicalDevice					= VK_NULL_HANDLE;
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
	VkRenderPass								renderPass;
	VkDescriptorSetLayout						descriptorSetLayout;
	VkDescriptorPool							descriptorPool;
	std::vector< VkDescriptorSet >				descriptorSets;
	VkPipelineLayout							pipelineLayout;
	VkPipeline									graphicsPipeline;
	std::vector< VkFramebuffer >				swapChainFramebuffers;
	VkCommandPool								commandPool;
	std::vector< VkCommandBuffer >				commandBuffers;
	std::vector< VkSemaphore >					imageAvailableSemaphores;
	std::vector< VkSemaphore >					renderFinishedSemaphores;
	std::vector< VkFence >						inFlightFences;
	size_t										currentFrame					= 0;
	bool										framebufferResized				= false;
	VkBuffer									vertexBuffer;
	VkDeviceMemory								vertexBufferMemory;
	VkBuffer									indexBuffer;
	VkDeviceMemory								indexBufferMemory;
	std::vector< VkBuffer >						uniformBuffers;
	std::vector< VkDeviceMemory >				uniformBuffersMemory;
	clock_t										current_ticks, delta_ticks;
	clock_t										fps								= 0;
	VkImage										textureImage;
	VkDeviceMemory								textureImageMemory;
	VkImageView									textureImageView;
	VkSampler									textureSampler;

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
	void createRenderPass(void);
	void createFramebuffers(void);
	void createCommandPool(void);
	void createCommandBuffers(void);
	void createSyncObjects(void);
	void renderFrame(void);
	void recreateSwapChain(void);
	void cleanupSwapChain(void);
	static void framebufferResizeCallback(
		
		GLFWwindow*		window, 
		int				width, 
		int				eight
	
	);
	void createVertexBuffer(void);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void createBuffer(
		
		VkDeviceSize				size, 
		VkBufferUsageFlags			usage, 
		VkMemoryPropertyFlags		properties,
		VkBuffer&					buffer, 
		VkDeviceMemory&				bufferMemory
	
	);
	void copyBuffer(
	
		VkBuffer		srcBuffer,
		VkBuffer		dstBuffer,
		VkDeviceSize	size
	
	);
	void createIndexBuffer(void);
	void createDescriptorSetLayout(void);
	void createUniformBuffers(void);
	void updateUniformBuffer(uint32_t currentImage);
	void createDescriptorPool(void);
	void createDescriptorSets(void);
	void createTextureImage(void);
	void createImage(

		uint32_t					width,
		uint32_t					height,
		VkFormat					format,
		VkImageTiling				tiling,
		VkImageUsageFlags			usage,
		VkMemoryPropertyFlags		properties,
		VkImage&					image,
		VkDeviceMemory&				imageMemory

	);
	VkCommandBuffer beginSingleTimeCommands(void);
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	void transitionImageLayout(
		
		VkImage				image, 
		VkFormat			format,
		VkImageLayout		oldLayout, 
		VkImageLayout		newLayout
	
	);
	void copyBufferToImage(

		VkBuffer		buffer,
		VkImage			image,
		uint32_t		width,
		uint32_t		height

	);
	void createTextureImageView(void);
	VkImageView createImageView(VkImage image, VkFormat format);
	void createTextureSampler(void);

};

