#pragma once

#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct Mesh {
	VertexBuffer<Vertex> vertexbuffer;
	IndexBuffer indexbuffer;
};