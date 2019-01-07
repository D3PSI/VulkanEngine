/*
*	File:		Camera.hpp
*	
*
*/
#pragma once
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum CameraMovement {

	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT

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
	~Camera();
private:

};

