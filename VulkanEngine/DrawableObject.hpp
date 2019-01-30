/*
*	File:		DrawableObject.hpp
*	
*/
#pragma once
#include <vector>
#include "Vertex.cpp"
class DrawableObject {
public:
	DrawableObject();
	~DrawableObject();
private:
	std::vector< Vertex >					vertices;
	std::vector< uint32_t >					indices;
};

