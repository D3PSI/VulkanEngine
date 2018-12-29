#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include "Vertex.cpp"
/*
*	Namespace:		std
*	Purpose:		Hash function for mesh rendering
*
*/
namespace std {

	template<> struct hash< Vertex > {

		size_t operator()(Vertex const& vertex) const {

			return ((hash< glm::vec3 >()(vertex.pos) ^
					(hash< glm::vec3 >()(vertex.color) << 1)) >> 1) ^
					(hash< glm::vec2 >()(vertex.texCoord) << 1);

		}

	};

}