/**
***		Just another VULKAN game engine, made by:
***		
***			D3PSI
***			d3psigames@gmail.com
***		
***		
***		View LICENSE.md for license information!
***		
***		
***		
**/		

/*
*	File:		Main.cpp
*	
*
*/
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <optional>
#include <vector>
#include <cstring>

#include "Logger.hpp"
#include "Engine.hpp"

/*
*	Global variables
*
*
*/
Logger			logger;

/*
*	Prototypes
*	
*
*/
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

/*
*	Namespace:		game
*	Purpose:		Prevent global naming conflicts
*
*/
namespace game {

	/*
	*	"Global" Variables (in namespace)
	*	
	*
	*/
	Engine			engine;

	/*
	*	Function:		VkResult game::CreateDebugUtilsMessengerEXT(
	*
	*						VkInstance instance,
	*						const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	*						const VkAllocationCallbacks* pAllocator,
	*						VkDebugUtilsMessengerEXT* pCallback
	*
	*					)
	*	Purpose:		Sets up the callback messenger
	*
	*/
	VkResult CreateDebugUtilsMessengerEXT(
	
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator, 
		VkDebugUtilsMessengerEXT* pCallback

	) {
	
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
		
			return func(
				
				instance,
				pCreateInfo, 
				pAllocator, 
				pCallback
			
			);
		
		}
		else {
		
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		
		}

	}

	/*
	*	Function:		void DestroyDebugUtilsMessengerEXT(
	*	
	*						VkInstance instance,
	*						VkDebugUtilsMessengerEXT callback,
	*						const VkAllocationCallbacks* pAllocator
	*
	*					)
	*	Purpose:		Destroys the messenger handle
	*
	*/
	void DestroyDebugUtilsMessengerEXT(
		
		VkInstance instance,
		VkDebugUtilsMessengerEXT callback,
		const VkAllocationCallbacks* pAllocator

		) {
	
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
		
			func(
				
				instance,
				callback,
				pAllocator
			
			);
		
		}

	}

	/*
	*	Function:		QueueFamilyIndices game::findQueueFamilies(VkPhysicalDevice device)
	*	Purpose:		Finds the indices for the correct queue families for the physical device
	*
	*/
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
	
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
		
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			
				indices.graphicsFamily = i;
			
			}
		
			if (indices.isComplete()) {
			
				break;
			
			}

			i++;

		}

		return indices;
	
	}

	/*
	*	Function:		int game::init()
	*	Purpose:		Handles main initialization of everything
	*
	*/
	int init() {
	
		try {
		
			engine.run();

			return EXIT_SUCCESS;
		
		} 
		catch (const std::exception& e) {
		
			logger.log(ERROR_LOG, e.what());
			return EXIT_FAILURE;
		
		}
	
	}

}

/*
*	Function:		int main()
*	Purpose:		Entry point
*
*/
int main() {

	return game::init();

}