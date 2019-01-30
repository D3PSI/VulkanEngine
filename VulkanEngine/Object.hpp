/*
*	File:		Object.hpp
*	
*/
#pragma once
#include <vector>
#include "Vertex.cpp"
class Object {
public:
	Object(void);
	Object(const std::string fileName_);
	void draw(
		
		VkCommandBuffer			commandBuffer_,
		VkDeviceSize*			vertexOffsets_,
		VkDeviceSize			indexOffset_,
		VkIndexType				indexType_

	);
	~Object();
private:
	std::vector< Vertex >					vertices;
	std::vector< uint32_t >					indices;
	VkBuffer								vertexBuffer;
	VkBuffer								indexBuffer;

	void load(void);
	void createVertexBuffer();
	void createIndexBuffer();
	void bindVBO(VkCommandBuffer commandBuffer_, VkDeviceSize* offsets_);
	void bindIBO(

		VkCommandBuffer			commandBuffer_,
		VkDeviceSize			offset_,
		VkIndexType				indexType_

	);

};

