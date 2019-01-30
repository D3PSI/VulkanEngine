/*
*	File:		Object.cpp
*
*/
#include "Object.hpp"

/*
*	Function:		Object()
*	Purpose:		Default constructor
*
*/
Object::Object() {



}

/*
*	Function:		Object(const std::string fileName_)
*	Purpose:		Constructor
*	
*/
Object::Object(const std::string fileName_) {

	

}

/*
*	Function:		void draw(
*	
*						VkCommandBuffer			commandBuffer_, 
*						VkDeviceSize*			vertexOffsets_,
*						VkDeviceSize			indexOffset_,
*						VkIndexType				indexType_
*
*					)
*	Purpose:		Binds appropriate buffers and draws to the command buffer
*
*/
void Object::draw(
	
	VkCommandBuffer			commandBuffer_, 
	VkDeviceSize*			vertexOffsets_,
	VkDeviceSize			indexOffset_,
	VkIndexType				indexType_

) {

	bindVBO(commandBuffer_, vertexOffsets_);
	bindIBO(
		
		commandBuffer_,
		indexOffset_, 
		indexType_
	
	);
	
	vkCmdDrawIndexed(
	
		commandBuffer_,
		static_cast< uint32_t >(indices.size()),
		1,
		0,
		0,
		0

	);

}

/*
*	Function:		void bindVBO(VkCommandBuffer commandBuffer_, VkDeviceSize* offsets_)
*	Purpose:		Binds the vertex buffer to the command buffer
*
*/
void Object::bindVBO(VkCommandBuffer commandBuffer_, VkDeviceSize* offsets_) {

	vkCmdBindVertexBuffers(
	
		commandBuffer_,
		0,
		1,
		&vertexBuffer,
		offsets_
	
	);

}

/*
*	Function:		void bindIBO(
*
*						VkCommandBuffer			commandBuffer_,
*						VkDeviceSize			offset_, 
*						VkIndexType				indexType_
*
*					)
*	Purpose:		Binds the index buffer to the command buffer
*
*/
void Object::bindIBO(
	
	VkCommandBuffer			commandBuffer_,
	VkDeviceSize			offset_, 
	VkIndexType				indexType_

) {

	vkCmdBindIndexBuffer(
	
		commandBuffer_,
		indexBuffer,
		offset_,
		indexType_
	
	);

}

/*
*	Function:		~Object()
*	Purpose:		Default destructor
*
*/
Object::~Object() {

		

}

/*
*	Function:		void load()
*	Purpose:		Loads vertex data into array using ASSIMP
*
*/
void Object::load(void) {

	// TODO

}

/*
*	Function:		void createVertexBuffer()
*	Purpose:		Creates a valid VkBuffer handle with correct layout from the vertices vector
*
*/
void Object::createVertexBuffer() {

	// TODO

}

/*
*	Function:		void createIndexBuffer()
*	Purpose:		Creates a valid VkBuffer handle with correct layout from the vertices vector
*
*/
void Object::createIndexBuffer() {

	// TODO

}
