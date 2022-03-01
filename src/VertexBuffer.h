#pragma once
#include "Renderer.h"


class VertexBuffer {
private:
	unsigned int m_RendererID;

public:
	//ctor
	VertexBuffer(const void* data, unsigned int size) {
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID); 
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); 
	};
	VertexBuffer(const void* positions, const void* normals, const void* texCoords,
				 unsigned int size_p, unsigned int size_n, unsigned int size_c) {
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size_p, &positions);
		glBufferSubData(GL_ARRAY_BUFFER, size_p, size_n, &normals);
		glBufferSubData(GL_ARRAY_BUFFER, size_p + size_n, size_c, &texCoords);
	}
	//dtor
	~VertexBuffer() {
		glDeleteBuffers(1, &m_RendererID);
	};

	void Bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	};
	void Unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	};
};