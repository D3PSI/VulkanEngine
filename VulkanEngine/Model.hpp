/*
*	File:		Model.hpp
*	
*
*/
#pragma once
#include "Object.hpp"
class Model
	: public Object
{
public:
	Model(const std::string fileName_, Pipeline* pipeline_);
	~Model();
};

