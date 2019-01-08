/*
*	File:		Camera.hpp
*	
*
*/
#pragma once
#include "VERSION.cpp"
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum CameraMovement {

	FORWARD			= 0,
	BACKWARD		= 1,
	LEFT			= 2,
	RIGHT			= 3

};

const float YAW					= -90.0f;
const float PITCH				= 0.0f;
const float SPEED				= 2.5f;
const float SENSITIVITY			= 0.1f;
const float ZOOM				= 45.0f;

class Camera
{
public:
	glm::vec3					position;
	glm::vec3					front;
	glm::vec3					up;
	glm::vec3					right;
	glm::vec3					worldUp;
	float						yaw;
	float						pitch;
	float						movementSpeed;
	float						mouseSensitivity;
	float						zoom;

	Camera(
	
		glm::vec3		position_		= glm::vec3(

			0.0f,
			0.0f,
			0.0f
		
		),
		glm::vec3		up_				= glm::vec3(
		
			0.0f,
			1.0f,
			0.0f
		
		),
		float			yaw_			= YAW,
		float			pitch_			= PITCH

	);
	Camera(
	
		float		posX_,
		float		posY_,
		float		posZ_,
		float		upX_,
		float		upY_,
		float		upZ_,
		float		yaw_,
		float		pitch_
	
	);
	glm::mat4 getViewMatrix(void);
	void processKeyboard(CameraMovement direction_, float deltaTime_);
	void processMouseMovement(
		
		float			xOffset_,
		float			yOffset_,
		VkBool32		constrainPitch = VK_TRUE
	
	);
	void processMouseScroll(float yOffset_);
	void enableInput(void);
	void disableInput(void);
	~Camera();
private:
	bool inputEnabled;
	void updateCameraVectors(void);
};

