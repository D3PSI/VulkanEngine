/*
*	File:		Engine.hpp
*	
*
*/
#pragma once
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <string>

#include "Logger.hpp"

class Engine
{
public:
	void run(void);
private:
	VkResult				result;
	Logger					logger;
	GLFWwindow*				window;
	const unsigned int		width		= 1280;
	const unsigned int		height		= 780;
	const std::string		title		= "VULKAN by D3PSI";
	GLFWmonitor*			monitor		= nullptr;
	VkInstance				instance;

	void initWindow(void);
	void initVulkan(void);
	void createInstance(void);
	void mainLoop(void);
	void cleanup(void);
};

