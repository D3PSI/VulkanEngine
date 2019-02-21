/*
*	File:		Mesh.hpp
*	
*
*/
#pragma once
#include "Object.hpp"
class Mesh
	: public Object
{
public:
	Mesh(
		
		std::vector< Vertex >		vertices, 
		std::vector< uint32_t >		indices, 
		std::vector< Texture >		textures
	
	);
	void draw(Pipeline* pipeline);
	~Mesh();
private:

};

