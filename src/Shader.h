#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "Renderer.h"

#define VF_SHADER 0
#define CP_SHADER 1



struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader {
	unsigned int m_Type;
	std::string m_FilePath;
	unsigned int m_RendererID;

	std::unordered_map<std::string, int> m_UniformLocationCache;

public:
	//ctor (vertex shader and fragment shader)
	Shader(unsigned int type, const std::string& filepath)
		:m_Type(type), m_FilePath(filepath), m_RendererID(0) {
		if (m_Type == VF_SHADER) {
			ShaderProgramSource source = ParseShader(filepath);
			m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
		}
		else if(m_Type == CP_SHADER){
			std::string src = readFileIntoString(filepath);
			unsigned int compute = CompileShader(GL_COMPUTE_SHADER, src);
			m_RendererID = glCreateProgram();
			glAttachShader(m_RendererID, compute);
			glLinkProgram(m_RendererID);
			glValidateProgram(m_RendererID);
			glDeleteShader(compute);
		}
		else {
			std::cout << m_Type << ": type unknown, initialize error." << std::endl;
		}
	};
	
	//dtor
	~Shader() {
		glDeleteProgram(m_RendererID);
	};

	unsigned int getID() const {
		return m_RendererID;
	};

	unsigned int GetProgram() {
		return m_RendererID;
	}

	void Bind() const {
		glUseProgram(m_RendererID);
	};

	void Unbind() const {
		glUseProgram(0);
	};

	//set uniforms
	void SetUniform1i(const std::string& name, int value) {
		glUniform1i(GetUniformLocation(name), value);
	};
	void SetUniform1f(const std::string& name, float value) {
		glUniform1f(GetUniformLocation(name), value);
	};
	void SetUniform1b(const std::string& name, bool value) {
		glUniform1i(GetUniformLocation(name), (int)value);
	}
	void SetUniform3f(const std::string& name, float v0, float v1, float v2) {
		glUniform3f(GetUniformLocation(name), v0, v1, v2);
	};
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
		glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
	};
	void SetUniformM4fv(const std::string& name, int count, unsigned char transpose, const float* value) {
		glUniformMatrix4fv(GetUniformLocation(name), count, transpose, value);
	};

private:

	//read file into string
	std::string readFileIntoString(std::string filename)
	{
		std::ifstream ifile(filename.c_str());
		//put into ostringstream object buffer
		std::ostringstream buf;
		char ch;
		while (buf && ifile.get(ch))
			buf.put(ch);
		return buf.str();
	}

	ShaderProgramSource ParseShader(const std::string& filepath) {
		std::ifstream stream(filepath);

		if (stream.fail()) {
			std::cout << "Shader file path: " << filepath << " is invalid!" << std::endl;
		}

		enum class ShaderType {
			NONE = -1, VERTEX = 0, FRAGMENT = 1
		};

		std::string line;
		std::stringstream ss[2]; //0 vertex shader code source
								 //1 fragment shader code source
		ShaderType type = ShaderType::NONE;

		while (getline(stream, line)) {
			if (line.find("#shader") != std::string::npos) {
				if (line.find("vertex") != std::string::npos) {
					// set mode to vertex
					type = ShaderType::VERTEX;
				}
				else if (line.find("fragment") != std::string::npos) {
					// set mode to fragment
					type = ShaderType::FRAGMENT;
				}
			}
			else {
				ss[(int)type] << line << '\n';
			}
		}

		return { ss[0].str(), ss[1].str() }; //use struct to multi return
	};

	unsigned int CompileShader(unsigned int type, const std::string& source) {
		unsigned int id = glCreateShader(type); 
		const char* src = source.c_str();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);
		//Error handling
		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) {
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)alloca(length * sizeof(char));
			//alloca: allow dynamic allocation on stack (no manual release required)
			glGetShaderInfoLog(id, length, &length, message);

			std::cout << "Failed to compile ";
			if (type == GL_VERTEX_SHADER)
				std::cout << "vertex";
			else if (type == GL_FRAGMENT_SHADER)
				std::cout << "fragment";
			else if (type == GL_COMPUTE_SHADER) 
				std::cout << "compute";
			std::cout << " shader!" << std::endl;
			std::cout << message << std::endl;
			glDeleteShader(id);
			return 0;
		}
		return id;
	};

	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
		unsigned int program = glCreateProgram(); //create shader program
		unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
		unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		glValidateProgram(program);
		//finish link, delete
		glDeleteShader(vs);
		glDeleteShader(fs);

		return program;
	};

	unsigned int GetUniformLocation(const std::string& name) {
		if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) { //location stored in hash£¡
			return m_UniformLocationCache[name];
		}
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location == -1) {
			std::cout << "Warning: uniform " << name << " doesn't exist!" << std::endl;
		}
		else {
			m_UniformLocationCache[name] = location;
		}
		return location;
	};
};