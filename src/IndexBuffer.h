#pragma once
#include "Renderer.h"


class IndexBuffer {
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
public:
	//ctor
	IndexBuffer(const unsigned int* data, unsigned int count) 
		: m_Count(count) {
		//ASSERT(sizeof(unsigned int) == sizeof(GLuint));
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
	};
	//dtor
	~IndexBuffer() {
		glDeleteBuffers(1, &m_RendererID);
	};

	void Bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	};

	void Unbind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	};

	inline unsigned int GetCount() const { return m_Count; }
};