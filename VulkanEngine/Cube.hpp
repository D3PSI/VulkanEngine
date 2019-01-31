#pragma once
#include "Object.hpp"
#include "CubeVertex.cpp"
class Cube :
	public Object {
public:
	Cube();
	void draw(
		
		VkCommandBuffer			commandBuffer_,
		VkDeviceSize*			vertexOffsets_,
		VkDeviceSize			indexOffset_,
		VkIndexType				indexType_
	
	);
	~Cube();
private:
	std::vector< CubeVertex > vertices;
	
	void createVertexBuffer(void);

};

