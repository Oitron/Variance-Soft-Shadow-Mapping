#pragma once
#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"


class VertexBufferLayout;

class VertexArray {
private:
	unsigned int m_RendererID;

public:
	//ctor
	VertexArray() {
		glGenVertexArrays(1, &m_RendererID);
	};
	//dtor
	~VertexArray() {
		glDeleteVertexArrays(1, &m_RendererID);
	};

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) {
		Bind(); //bind vertexArray;
		vb.Bind();
		const auto& elements = layout.GetElements();
		unsigned int offset = 0;
		for (unsigned int i = 0; i < elements.size(); ++i) {
			const auto& element = elements[i];
			glEnableVertexAttribArray(i);
			//glVertexAttribPointer  location -> vertex shader location
			glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset);
			offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
		}
	};

	unsigned int GetID() const {
		return m_RendererID;
	};

	void Bind() const {
		glBindVertexArray(m_RendererID);
	};
	void Unbind() const {
		glBindVertexArray(0);
	};

};