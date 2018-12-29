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
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

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

		VkInstance										instance,
		const VkDebugUtilsMessengerCreateInfoEXT*		pCreateInfo,
		const VkAllocationCallbacks*					pAllocator,
		VkDebugUtilsMessengerEXT*						pCallback

	);
	void DestroyDebugUtilsMessengerEXT(

		VkInstance										instance,
		VkDebugUtilsMessengerEXT						callback,
		const VkAllocationCallbacks*					pAllocator

	);
	std::vector< char > readFile(const std::string& filename);
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
	Engine												engine;
	const unsigned int									MAX_FRAMES_IN_FLIGHT				= 2;

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
	
		VkInstance										instance,
		const VkDebugUtilsMessengerCreateInfoEXT*		pCreateInfo,
		const VkAllocationCallbacks*					pAllocator, 
		VkDebugUtilsMessengerEXT*						pCallback

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
		
		VkInstance										instance,
		VkDebugUtilsMessengerEXT						callback,
		const VkAllocationCallbacks*					pAllocator

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
	std::vector< char > readFile(const std::string& filename) {
	
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
		
			logger.log(ERROR_LOG, "Failed to open (shader?) file!");
		
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

			logger.log(START_STOP_LOG, "Starting application...");
		
			engine.run();

			logger.log(START_STOP_LOG, "Stopping application...");

			std::cin.get();

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