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
	static std::vector< char > readFile(const std::string& filename);
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
	*	Function:		void game::DestroyDebugUtilsMessengerEXT(
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
	*	Function:		static std::vector< char > game::readFile(const std::string& filename)
	*	Purpose:		Reads (shader) files to binary handled by std::vector< char >
	*
	*/
	static std::vector< char > readFile(const std::string& filename) {
	
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
		
			logger.log(ERROR_LOG, "Failed to open shader file!");
			throw std::runtime_error("Failed to open shader file!");
		
		}
	
		size_t fileSize = (size_t)file.tellg();
		std::vector< char > buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;

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