/*
*	File:		Object.hpp
*	
*/
#pragma once
#include <vector>
#include <unordered_map>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include "Vertex.cpp"
#include "Logger.hpp"

extern Logger logger;

class Object {
public:
	Object(void);
	Object(const std::string fileName_);
	virtual void draw(
		
		VkCommandBuffer			commandBuffer_,
		VkDeviceSize*			vertexOffsets_,
		VkDeviceSize			indexOffset_,
		VkIndexType				indexType_

	);
	void destroy(void);
	~Object();
protected:
	std::vector< Vertex >					vertices;
	VkBuffer								vertexBuffer;
	VkDeviceMemory							vertexBufferMemory;
	std::vector< uint32_t >					indices;
	VkBuffer								indexBuffer;
	VkDeviceMemory							indexBufferMemory;

	void loadwithtinyobjloader(const std::string fileName_);
	void load(const std::string fileName_);
	virtual void createVertexBuffer(void);
	void createIndexBuffer(void);
	void bindVBO(VkCommandBuffer commandBuffer_, VkDeviceSize* offsets_);
	void bindIBO(

		VkCommandBuffer			commandBuffer_,
		VkDeviceSize			offset_,
		VkIndexType				indexType_

	);

};

