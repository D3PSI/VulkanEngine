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

}

/*
*	Function:		~Camera()
*	Purpose:		Default destructor
*
*/
Camera::~Camera() {



}
