#ifndef VERTEX_TYPES_H
#define VERTEX_TYPES_H

#include "graphics.h"
#include <cstddef>

struct Vertex2D {
	float x;
	float y;

	Vertex2D(float x, float y) : x(x), y(y) {

	}

	static std::vector<VertexAttributeData> get_vertex_attributes() {
		VertexAttributeData position_attribute {};
		position_attribute.index = 0;
		position_attribute.length = 2;
		position_attribute.normalized = false;
		position_attribute.type = GL_FLOAT;
		position_attribute.stride = get_size();
		position_attribute.offset = (void*)offsetof(Vertex2D, x);
		return {position_attribute};
	};

	static size_t get_size() {
		return sizeof(Vertex2D);
	};
};

struct Vertex3D {
	float x;
	float y;
	float z;

	Vertex3D(float x, float y, float z) : x(x), y(y), z(z) {

	}

	static std::vector<VertexAttributeData> get_vertex_attributes() {
		VertexAttributeData position_attribute {};
		position_attribute.index = 0;
		position_attribute.length = 3;
		position_attribute.normalized = false;
		position_attribute.type = GL_FLOAT;
		position_attribute.stride = get_size();
		position_attribute.offset = (void*)offsetof(Vertex3D, x);
		return {position_attribute};
	};

	static size_t get_size() {
		return sizeof(Vertex3D);
	};
};
#endif
