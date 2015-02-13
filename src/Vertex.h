#ifndef VERTEX_H_
#define VERTEX_H_

#include "glm_header.h"

struct Vertex
{
	vec4 position;
	vec4 color;
};

struct VertexTexCoord
{
	vec4 position;
	vec2 tex_coord;
};

struct VertexNormal
{
	vec4 position;
	vec4 normal;
	vec4 tangent ;
	vec2 tex_coord;
};

struct OpenGLData
{
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	unsigned int m_index_count;
};

#endif //VERTEX_H_