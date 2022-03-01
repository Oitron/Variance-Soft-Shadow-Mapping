#pragma once


#include <GL/glew.h>
#include <iostream>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"


#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__));


void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
};

bool GLLogCall(const char* function, const char* file, int line) {
	while (GLenum error = glGetError()) { // error = 0 (GL_NO_ERROR)
		std::cout << "[OpenGL error] (" << error << "): " << function <<
			" " << file << ": " << line << std::endl;
		return false;
	}
	return true;
};






class Renderer {
public:
	void Clear() const {
		glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	};
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {
		shader.Bind();
		va.Bind();
		ib.Bind();
		GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount()/*count not size*/, GL_UNSIGNED_INT, nullptr));
	};
	void DrawNoIB(const VertexArray& va, const Shader& shader, const unsigned int numVertex) {
		shader.Bind();
		va.Bind();
		glDrawArrays(GL_TRIANGLES, 0, numVertex);
	}
};