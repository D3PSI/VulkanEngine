/*
*	File:		Object.cpp
*
*/
#include "Object.hpp"
#include <stb_image.h>
#include <tiny_obj_loader.h>
#include "Engine.hpp"

/*
*	Function:		Object()
*	Purpose:		Default constructor
*
*/
Object::Object(void) {



}

/*
*	Function:		Object(const std::string fileName_)
*	Purpose:		Constructor
*	
*/
Object::Object(const std::string fileName_) {

#if defined GAME_USE_TINY_OBJ
	loadwithtinyobjloader(fileName_);
#elif !defined GAME_USE_TINY_OBJ
	load(fileName_);
#endif
	createVertexBuffer();
	createIndexBuffer();

}

/*
*	Function:		virtual void draw(
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
*	Function:		virtual ~Object()
*	Purpose:		Default destructor
*
*/
Object::~Object() {

	

}

/*
*	Function:		void loadwithtinyobjloader(const std::string fileName_)
*	Purpose:		Loads vertex data into array using the tiny_obj_loader library
*
*/
void Object::loadwithtinyobjloader(const std::string fileName_) {

	tinyobj::attrib_t						attrib;
	std::vector< tinyobj::shape_t >			shapes;
	std::vector< tinyobj::material_t >		materials;
	std::string								warn, err;

	if (!tinyobj::LoadObj(

		&attrib,
		&shapes,
		&materials,
		&warn,
		&err,
		fileName_.c_str()

	)) {

		logger.log(ERROR_LOG, warn + err);

	}

	std::unordered_map< Vertex, uint32_t > uniqueVertices = {};

	for (const auto& shape : shapes) {

		for (const auto& index : shape.mesh.indices) {

			Vertex vertex = {};

			vertex.pos = {

				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]

			};

			vertex.texCoord = {

				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]

			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			vertex.normal = {
			
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			
			};
			
			if (uniqueVertices.count(vertex) == 0) {

				uniqueVertices[vertex] = static_cast< uint32_t >(vertices.size());
				vertices.push_back(vertex);

			}

			indices.push_back(uniqueVertices[vertex]);

		}

	}

}

/*
*	Function:		void load(const std::string fileName_)
*	Purpose:		Loads a 3D-model from a file using ASSIMP and the Mesh class
*
*/
void Object::load(const std::string fileName_) {



}

/*
*	Function:		virtual void createVertexBuffer()
*	Purpose:		Creates a valid VkBuffer handle with correct layout from the vertices vector
*
*/
void Object::createVertexBuffer() {

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
*	Function:		void createIndexBuffer()
*	Purpose:		Creates a valid VkBuffer handle with correct layout from the vertices vector
*
*/
void Object::createIndexBuffer() {

	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
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
		indices.data(),
		(size_t)bufferSize

	);

	vkUnmapMemory(engine.device, stagingBufferMemory);

	engine.createBuffer(

		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		indexBuffer,
		indexBufferMemory

	);

	engine.copyBuffer(

		stagingBuffer,
		indexBuffer,
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
*	Function:		void destroy()
*	Purpose:		Destroys all allocated resources per object
*
*/
void Object::destroy() {

	vkDestroyBuffer(

		engine.device,
		vertexBuffer,
		nullptr

	);
	vkFreeMemory(

		engine.device,
		vertexBufferMemory,
		nullptr

	);

	vkDestroyBuffer(

		engine.device,
		indexBuffer,
		nullptr

	);
	vkFreeMemory(

		engine.device,
		indexBufferMemory,
		nullptr

	);

}
