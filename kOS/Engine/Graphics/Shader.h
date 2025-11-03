#pragma once
#ifndef SHADER_H
#define SHADER_H
/*****************************************************************
FILENAME:   Shader.h
AUTHOR(S):  Gabe (100%)
BRIEF:      Manages to creation of shaders from file path
			Able to use helper functions to access shader properties easily

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
******************************************************************/

#include <glad/glad.h>
#include "Config/pch.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>
#include "Debugging/Logging.h"
class Shader {

public:
	unsigned int ID{ 0 };
	std::unordered_map<std::string, GLint> uniformLocationCache;
	GLint GetLocation(const std::string& ss) {
		auto it = uniformLocationCache.find(ss);
		if (it == uniformLocationCache.end()) {
			uniformLocationCache[ss] = glGetUniformLocation(ID, ss.c_str());
			return uniformLocationCache[ss];;
		}
		return it->second;;
	}

	/************************************************************************/
	/*!
	\brief
	Constructs a `Shader` object by loading and compiling vertex and fragment shaders from the specified file paths.

	\param const char* vertexPath
	Path to the vertex shader file.
	\param const char* fragmentPath
	Path to the fragment shader file.

	\return
	NIL
	*/
	/************************************************************************/

	Shader(const char* vertexSourceCode, const char* fragmentSourceCode) {

		//Leave this here for now for run time shaders
		//std::string vertexCode, fragmentCode;
		//std::ifstream vShaderFile, fShaderFile;
		//vShaderFile.open(vertexPath);
		//fShaderFile.open(fragmentPath);
		//if (!vShaderFile.is_open() || !fShaderFile.is_open()) { std::cout << "Error opening shader file"; return; }

		////Create string streams
		//std::stringstream vStringStream, fStringStream;
		//vStringStream << vShaderFile.rdbuf();
		//fStringStream << fShaderFile.rdbuf();
		//vShaderFile.close();
		//fShaderFile.close();

		//vertexCode = vStringStream.str();
		//fragmentCode = fStringStream.str();

		//const char* vertexSourceCode{ vertexCode.c_str() };
		//const char* fragmentSourceCode{ fragmentCode.c_str() };

		// compile shaders
		int success;
		unsigned int vertex, fragment;

		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertexSourceCode, NULL);
		glCompileShader(vertex);
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

		if (!success) {
			std::cout << "Vertex Shader did not compile" << '\n';
			int log_len;
			glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &log_len);
			if (log_len > 0) {
				char* log = new char[log_len];
				GLsizei written_log_len;
				glGetShaderInfoLog(vertex, log_len, &written_log_len, log);
				std::cout << "ERROR LOGG" << std::string{ log };
				delete[] log;
			}
			return;
		}

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragmentSourceCode, NULL);
		glCompileShader(fragment);
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

		if (!success) {
			std::cout << "Fragment shader did not compile";
			return;
		}

		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);

		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			char infoLog[512];
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
			return;
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);
		LOGGING_INFO("Created shader");
	}
	Shader(const char* vertexSourceCode, const char* fragmentSourceCode, const char* geometrySourceCode) {

		// compile shaders
		int success;
		unsigned int vertex, fragment, geometry;

		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertexSourceCode, NULL);
		glCompileShader(vertex);
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

		if (!success) {
			std::cout << "Shader did not compile Vertex" << '\n';
			int log_len;
			glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &log_len);
			if (log_len > 0) {
				char* log = new char[log_len];
				GLsizei written_log_len;
				glGetShaderInfoLog(vertex, log_len, &written_log_len, log);
				std::cout << "ERROR LOGG" << std::string{ log };
				delete[] log;
			}
			return;
		}

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragmentSourceCode, NULL);
		glCompileShader(fragment);
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

		if (!success) {
			std::cout << "Fragment shader did not compile";
			return;
		}

		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &geometrySourceCode, NULL);
		glCompileShader(geometry);
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);

		if (!success) {
			std::cout << "Geometry shader did not compile";
			return;;
		}

		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glAttachShader(ID, geometry);

		glLinkProgram(ID);

		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			char infoLog[512];
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
			return;
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);
		glDeleteShader(geometry);
		std::cout << "Created shader with vfg\n";
	}

	Shader(Shader const& shad) {
		this->ID = shad.ID;
		this->uniformLocationCache = shad.uniformLocationCache;
	}
	/************************************************************************/
	/*!
	\brief
	Deletes the shader program.

	\return
	NIL
	*/
	/************************************************************************/
	void DeleteShader() {
		//std::cout << "DELETE SHADER";
		glDeleteProgram(ID);
	}
	/************************************************************************/
	/*!
	\brief
	Activates the shader program for use in the rendering pipeline.

	\return
	NIL
	*/
	/************************************************************************/
	void Use() {
		glUseProgram(ID);
	}

	void Disuse() {
		glUseProgram(0);
	}

	/************************************************************************/
	/*!
	\brief
	Sets a boolean uniform variable in the shader.

	\param const std::string& name
	The name of the uniform variable in the shader.
	\param bool value
	The boolean value to set.

	\return
	NIL
	*/
	/************************************************************************/
	void SetBool(const std::string& name, bool value) const
	{
		GLint location = glGetUniformLocation(ID, name.c_str());
		glUniform1i(location, static_cast<int>(value));
	};
	/************************************************************************/
	/*!
	\brief
	Sets a transformation matrix uniform in the shader.

	\param const std::string& name
	The name of the uniform variable in the shader.
	\param const glm::mat4& trans
	The transformation matrix to set.

	\return
	NIL
	*/
	/************************************************************************/
	void SetTrans(const std::string& name, const glm::mat4& trans) {
		glUniformMatrix4fv(GetLocation(name), 1, GL_FALSE, glm::value_ptr(trans));
	}

	void SetVec2(const std::string& name, const glm::vec2& vec2) {
		glUniform2f(GetLocation(name), vec2.x, vec2.y);
	}

	void SetVec3(const std::string& name, const glm::vec3& vec3) {
		glUniform3f(GetLocation(name), vec3.x, vec3.y, vec3.z);
	}

	/************************************************************************/
	/*!
	\brief
	Sets a 4D vector uniform in the shader.

	\param const std::string& name
	The name of the uniform variable in the shader.
	\param const glm::vec4& vec4
	The 4D vector to set.

	\return
	NIL
	*/
	/************************************************************************/
	void SetVec4(const std::string& name, const glm::vec4& vec4) {
		glUniform4f(GetLocation(name), vec4.r, vec4.g, vec4.b, vec4.a);
	}
	/************************************************************************/
	/*!
	\brief
	Sets an integer uniform in the shader.

	\param const std::string& name
	The name of the uniform variable in the shader.
	\param int value
	The integer value to set.

	\return
	NIL
	*/
	/************************************************************************/
	void SetInt(const std::string& name, int value) {
		glUniform1i(GetLocation(name), value);
	}
	/************************************************************************/
	/*!
	\brief
	Sets a float uniform in the shader.

	\param const std::string& name
	The name of the uniform variable in the shader.
	\param float value
	The float value to set.

	\return
	NIL
	*/
	/************************************************************************/
	void SetFloat(const std::string& name, float value) {
		glUniform1f(GetLocation(name), value);
	}

	void SetMat4(const std::string& name, const glm::mat4& value) {
		glUniformMatrix4fv(GetLocation(name), 1, GL_FALSE, glm::value_ptr(value));
	}

	void SetMat4Array(const std::string& name, const glm::mat4& value, size_t arraySize) {
		glUniformMatrix4fv(GetLocation(name), static_cast<GLsizei>(arraySize), GL_FALSE, glm::value_ptr(value));
	}

	void SetMat3(const std::string& name, const glm::mat3& value) {
		glUniformMatrix3fv(GetLocation(name), 1, GL_FALSE, glm::value_ptr(value));
	}

	void SetIntArray(const std::string& name, int* values, int count) {
		glUniform1iv(GetLocation(name), count, values);
	}
};
#endif // ! SHADER_H
