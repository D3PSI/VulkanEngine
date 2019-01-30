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
#include "VERSION.cpp"
#define NOMINMAX
#if !defined GAME_RELEASE && !defined GAME_DEVELOPMENT
	#define GAME_RELEASE
#endif
#if defined GAME_RELEASE && !defined GAME_DEVELOPMENT
	#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
#if !defined GAME_USE_FRAMERATE_CAP_60 && !defined GAME_NO_FRAMERATE_CAP
	#define GAME_NO_FRAMERATE_CAP
#endif
#include <Windows.h>
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
#include <conio.h>

#include "Logger.hpp"
#include "Engine.hpp"

/*
*	Global variables
*
*
*/
Engine												engine;
Logger												logger;

/*
*	Prototypes
*	
*
*/
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

	/*
	*	Function:		VkResult game::CreateDebugUtilsMessengerEXT(
	*
	*						VkInstance										instance_,
	*						const VkDebugUtilsMessengerCreateInfoEXT*		pCreateInfo_,
	*						const VkAllocationCallbacks*					pAllocator_,
	*						VkDebugUtilsMessengerEXT*						pCallback_
	*
	*					)
	*	Purpose:		Sets up the callback_ messenger
	*
	*/
	VkResult CreateDebugUtilsMessengerEXT(
	
		VkInstance										instance_,
		const VkDebugUtilsMessengerCreateInfoEXT*		pCreateInfo_,
		const VkAllocationCallbacks*					pAllocator_, 
		VkDebugUtilsMessengerEXT*						pCallback_

	) {
	
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance_, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
		
			return func(
				
				instance_,
				pCreateInfo_, 
				pAllocator_, 
				pCallback_
			
			);
		
		}
		else {
		
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		
		}

	}

	/*
	*	Function:		void game::DestroyDebugUtilsMessengerEXT(
	*	
	*						VkInstance							instance_,
	*						VkDebugUtilsMessengerEXT			callback_,
	*						const VkAllocationCallbacks*		pAllocator_
	*
	*					)
	*	Purpose:		Destroys the messenger handle
	*
	*/
	void DestroyDebugUtilsMessengerEXT(
		
		VkInstance										instance_,
		VkDebugUtilsMessengerEXT						callback_,
		const VkAllocationCallbacks*					pAllocator_

		) {
	
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance_, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
		
			func(
				
				instance_,
				callback_,
				pAllocator_
			
			);
		
		}

	}

	/*
	*	Function:		static std::vector< char > game::readFile(const std::string& filename_)
	*	Purpose:		Reads (shader) files to binary handled by std::vector< char >
	*
	*/
	std::vector< char > readFile(const std::string& filename_) {
	
		std::ifstream file(filename_, std::ios::ate | std::ios::binary);
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

#ifdef GAME_DEVELOPMENT
			logger.showConsoleWindow();

			std::cout << "\n\nPress any key to exit...";

			_getch();
#endif
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