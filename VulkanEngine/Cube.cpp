#include "Cube.hpp"
#include "Engine.hpp"

const CubeVertex vert[] = {

	glm::vec3(-0.5f, -0.5f, -0.5f),  glm::vec3(0.0f,  0.0f, -1.0f),
	glm::vec3( 0.5f, -0.5f, -0.5f),  glm::vec3(0.0f,  0.0f, -1.0f),
	glm::vec3( 0.5f,  0.5f, -0.5f),  glm::vec3(0.0f,  0.0f, -1.0f),
	glm::vec3( 0.5f,  0.5f, -0.5f),  glm::vec3(0.0f,  0.0f, -1.0f),
	glm::vec3(-0.5f,  0.5f, -0.5f),  glm::vec3(0.0f,  0.0f, -1.0f),
	glm::vec3(-0.5f, -0.5f, -0.5f),  glm::vec3(0.0f,  0.0f, -1.0f),

	glm::vec3(-0.5f, -0.5f,  0.5f),  glm::vec3(0.0f,  0.0f,  1.0f),
	glm::vec3( 0.5f, -0.5f,  0.5f),  glm::vec3(0.0f,  0.0f,  1.0f),
	glm::vec3( 0.5f,  0.5f,  0.5f),  glm::vec3(0.0f,  0.0f,  1.0f),
	glm::vec3( 0.5f,  0.5f,  0.5f),  glm::vec3(0.0f,  0.0f,  1.0f),
	glm::vec3(-0.5f,  0.5f,  0.5f),  glm::vec3(0.0f,  0.0f,  1.0f),
	glm::vec3(-0.5f, -0.5f,  0.5f),  glm::vec3(0.0f,  0.0f,  1.0f),

	glm::vec3(-0.5f,  0.5f,  0.5f), -glm::vec3(1.0f,  0.0f,  0.0f),
	glm::vec3(-0.5f,  0.5f, -0.5f), -glm::vec3(1.0f,  0.0f,  0.0f),
	glm::vec3(-0.5f, -0.5f, -0.5f), -glm::vec3(1.0f,  0.0f,  0.0f),
	glm::vec3(-0.5f, -0.5f, -0.5f), -glm::vec3(1.0f,  0.0f,  0.0f),
	glm::vec3(-0.5f, -0.5f,  0.5f), -glm::vec3(1.0f,  0.0f,  0.0f),
	glm::vec3(-0.5f,  0.5f,  0.5f), -glm::vec3(1.0f,  0.0f,  0.0f),

	glm::vec3( 0.5f,  0.5f,  0.5f),  glm::vec3(1.0f,  0.0f,  0.0f),
	glm::vec3( 0.5f,  0.5f, -0.5f),  glm::vec3(1.0f,  0.0f,  0.0f),
	glm::vec3( 0.5f, -0.5f, -0.5f),  glm::vec3(1.0f,  0.0f,  0.0f),
	glm::vec3( 0.5f, -0.5f, -0.5f),  glm::vec3(1.0f,  0.0f,  0.0f),
	glm::vec3( 0.5f, -0.5f,  0.5f),  glm::vec3(1.0f,  0.0f,  0.0f),
	glm::vec3( 0.5f,  0.5f,  0.5f),  glm::vec3(1.0f,  0.0f,  0.0f),

	glm::vec3(-0.5f, -0.5f, -0.5f),  glm::vec3(0.0f, -1.0f,  0.0f),
	glm::vec3( 0.5f, -0.5f, -0.5f),  glm::vec3(0.0f, -1.0f,  0.0f),
	glm::vec3( 0.5f, -0.5f,  0.5f),  glm::vec3(0.0f, -1.0f,  0.0f),
	glm::vec3( 0.5f, -0.5f,  0.5f),  glm::vec3(0.0f, -1.0f,  0.0f),
	glm::vec3(-0.5f, -0.5f,  0.5f),  glm::vec3(0.0f, -1.0f,  0.0f),
	glm::vec3(-0.5f, -0.5f, -0.5f),  glm::vec3(0.0f, -1.0f,  0.0f),

	glm::vec3(-0.5f,  0.5f, -0.5f),  glm::vec3(0.0f,  1.0f,  0.0f),
	glm::vec3( 0.5f,  0.5f, -0.5f),  glm::vec3(0.0f,  1.0f,  0.0f),
	glm::vec3( 0.5f,  0.5f,  0.5f),  glm::vec3(0.0f,  1.0f,  0.0f),
	glm::vec3( 0.5f,  0.5f,  0.5f),  glm::vec3(0.0f,  1.0f,  0.0f),
	glm::vec3(-0.5f,  0.5f,  0.5f),  glm::vec3(0.0f,  1.0f,  0.0f),
	glm::vec3(-0.5f,  0.5f, -0.5f),  glm::vec3(0.0f,  1.0f,  0.0f)

};

/*
*	Function:		Cube()
*	Purpose:		Default constructor
*
*/
Cube::Cube() {

	vertices = std::vector(vert, vert + sizeof(vert) / sizeof(vert[0]));

	createVertexBuffer();

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
*	Purpose:		Overrides inherited draw function to draw without an index buffer	
*
*/
void Cube::draw(

	VkCommandBuffer			commandBuffer_,
	VkDeviceSize*			vertexOffsets_,
	VkDeviceSize			indexOffset_,
	VkIndexType				indexType_

) {

	bindVBO(commandBuffer_, vertexOffsets_);

	vkCmdDraw(

		commandBuffer_,
		static_cast< uint32_t >(vertices.size()),
		1,
		0,
		0

	);

}

/*
*	Function:		void createVertexBuffer()
*	Purpose:		Creates a valid VkBuffer handle with correct layout from the vertices vector
*
*/
void Cube::createVertexBuffer() {

	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer					stagingBuffer;
	VkDeviceMemory				stagingBufferMemory;

	engine.createBuffer(

		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory

	);

	void* data;
	vkMapMemory(

		engine.device,
		stagingBufferMemory,
		0,
		bufferSize,
		0,
		&data

	);
	memcpy(

		data,
		vertices.data(),
		(size_t)bufferSize

	);
	vkUnmapMemory(engine.device, stagingBufferMemory);

	engine.createBuffer(

		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		vertexBuffer,
		vertexBufferMemory

	);

	engine.copyBuffer(

		stagingBuffer,
		vertexBuffer,
		bufferSize

	);

	vkDestroyBuffer(

		engine.device,
		stagingBuffer,
		nullptr

	);
	vkFreeMemory(

		engine.device,
		stagingBufferMemory,
		nullptr

	);

}

/*
*	Function:		~Cube()
*	Purpose:		Default destructor
*
*/
Cube::~Cube() {



}
