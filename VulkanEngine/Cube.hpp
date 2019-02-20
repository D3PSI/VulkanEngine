#pragma once
#include "Object.hpp"
#include "CubeVertex.cpp"
#include "Pipeline.hpp"

class Cube :
	public Object {
public:
	Cube(Pipeline* pipeline_);
	void draw(
		
		VkCommandBuffer			commandBuffer_,
		VkDeviceSize*			vertexOffsets_,
		VkDeviceSize			indexOffset_,
		VkIndexType				indexType_,
		uint32_t				descriptorSetIndex_
	
	);
	~Cube();
private:
	std::vector< CubeVertex > vertices;

	void createVertexBuffer(void);

};

