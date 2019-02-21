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
#include "Texture.cpp"
#include "Logger.hpp"
#include "Pipeline.hpp"

extern Logger logger;

class Object {
public:
	Object(void);
	Object(

		const std::string		fileName_,
		Pipeline*				pipeline_,
		bool					hasTextures_		= false
	
	);
	virtual void draw(
		
		VkCommandBuffer			commandBuffer_,
		VkDeviceSize*			vertexOffsets_,
		VkDeviceSize			indexOffset_,
		VkIndexType				indexType_,
		uint32_t				descriptorSetIndex_

	);
	void destroy(void);
	virtual ~Object();
protected:
	std::vector< Vertex >					vertices;
	VkBuffer								vertexBuffer;
	VkDeviceMemory							vertexBufferMemory;
	std::vector< uint32_t >					indices;
	VkBuffer								indexBuffer;
	VkDeviceMemory							indexBufferMemory;
	std::vector< Texture >					textures;
	bool									hasTextures;
	Pipeline*								pipeline;

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

