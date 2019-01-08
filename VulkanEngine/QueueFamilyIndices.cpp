#pragma once
#include "VERSION.cpp"
#include <optional>

struct QueueFamilyIndices {

	std::optional< uint32_t > graphicsFamily;
	std::optional< uint32_t > presentFamily;

	bool isComplete() {

		return graphicsFamily.has_value() && presentFamily.has_value();

	}

};