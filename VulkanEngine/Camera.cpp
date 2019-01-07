/*
*	File:		Camera.cpp
*	
*
*/
#include "Camera.hpp"


/*
*	Function:		Camera(
*
*						glm::vec3		position_ = glm::vec3(
*
*							0.0f,
*							0.0f,
*							0.0f
*							
*						),
*						glm::vec3		up_ = glm::vec3(
*						
*							0.0f,
*							1.0f,
*							0.0f
*
*						),
*						float			yaw_ = YAW,
*						float			pitch_ = PITCH
*	
*					)
*	Purpose:		Constructor using vectors
*
*/
Camera::Camera(
	
	glm::vec3		position_,
	glm::vec3		up_,
	float			yaw_,
	float			pitch_

) : front(glm::vec3(

	0.0f,
	0.0f,
   -1.0f

)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {

	position		= position_;
	worldUp			= up_;
	yaw				= yaw_;
	pitch			= pitch_;
	updateCameraVectors();

}

/*
*	Function:		Camera(
*
*						float		posX_,
*						float		posY_,
*						float		posZ_,
*						float		upX_,
*						float		upY_,
*						float		upZ_,
*						float		yaw_,
*						float		pitch_
*		
*		)
*	Purpose:		Constructor using scalar values
*
*/
Camera::Camera(
	
	float		posX_,
	float		posY_,
	float		posZ_,
	float		upX_,
	float		upY_,
	float		upZ_,
	float		yaw_,
	float		pitch_

) {

	position		= glm::vec3(
		
		posX_,
		posY_,
		posZ_
	
	);

	worldUp			= glm::vec3(
	
		upX_,
		upY_,
		upZ_
	
	);

	yaw				= yaw_;
	pitch			= pitch_;
	updateCameraVectors();

}

/*
*	Function:		glm::mat4 getViewMatrix()
*	Purpose:		Returns the view matrix
*
*/
glm::mat4 Camera::getViewMatrix(void) {

	return glm::lookAt(
	
		position,
		position + front,
		up
	
	);

}

/*
*	Function:		void processKeyboard(CameraMovement direction_, float deltaTime_)
*	Purpose:		Processes movement input from keyboard
*
*/
void Camera::processKeyboard(CameraMovement direction_, float deltaTime_) {

	float velocity = movementSpeed * deltaTime_;

	switch (direction_) {
	
		case FORWARD:
			position += front * velocity;
			break;
		case BACKWARD:
			position -= front * velocity;
			break;
		case LEFT:
			position -= right * velocity;
			break;
		case RIGHT:
			position += right * velocity;
			break;
		default:
			break;

	}

}

/*
*	Function:		void processMouseMovement(
*	
*						float			xOffset_, 
*						float			yOffset_,
*						VkBool32		constrainPitch
*
*					)
*	Purpose:		
*
*/
void Camera::processMouseMovement(
	
	float			xOffset_, 
	float			yOffset_,
	VkBool32		constrainPitch

) {

	xOffset_		*= mouseSensitivity;
	yOffset_		*= mouseSensitivity;

	yaw				+= xOffset_;
	pitch			+= yOffset_;

	if (constrainPitch) {
	
		if (pitch > 89.0f) {
		
			pitch = 89.0f;
		
		}
		else if (pitch < -89.0f) {
		
			pitch = -89.0f;
		
		}
	
		updateCameraVectors();

	}

}

/*
*	Function:		void processMouseScroll(float yOffset_)
*	Purpose:		Processes mouse scroll events
*
*/
void Camera::processMouseScroll(float yOffset_) {

	if (zoom >= 1.0f && zoom <= 45.0f) {
	
		zoom -= yOffset_;

	}
	else if (zoom <= 1.0f) {
	
		zoom = 1.0f;
	
	}
	else if (zoom >= 45.0f) {
	
		zoom = 45.0f;
	
	}

	updateCameraVectors();

}

/*
*	Function:		~Camera()
*	Purpose:		Default destructor
*
*/
Camera::~Camera() {



}

/*
*	Function:		void updateCameraVectors()
*	Purpose:		Updates camera vectors (LOLZ :D)
*
*/
void Camera::updateCameraVectors(void) {

	glm::vec3 front_;
	front_.x		= cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front_.y		= sin(glm::radians(pitch));
	front_.z		= sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front			= glm::normalize(front_);
	right			= glm::normalize(glm::cross(front, worldUp));
	up				= glm::normalize(glm::cross(right, front));

}
