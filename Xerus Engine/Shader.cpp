#include "stdafx.h"
#include "Shader.h"


xr::Shader::Shader(const char * vertexSource, const char * fragmentSource)
{
	GLuint vertexShader = this->compileShaderSource(vertexSource, GL_VERTEX_SHADER);
	GLuint fragmentShader = this->compileShaderSource(fragmentSource, GL_FRAGMENT_SHADER);

	this->program = this->linkShaderProgram(vertexShader, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

xr::Shader::~Shader()
{
	glDeleteProgram(this->program);
}

void xr::Shader::use()
{
	glUseProgram(this->program);
}

void xr::Shader::bindAttribute(GLuint location, const char * name)
{
	glBindAttribLocation(this->program, location, name);
}

GLuint xr::Shader::getUniformLocation(const char * name)
{
	return glGetUniformLocation(this->program, name);
}

GLuint xr::Shader::compileShaderSource(const char * source, GLenum type)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);

	glCompileShader(shader);
	
	// Check for compile errors
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {
		// Get error log length
		GLint logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

		// Get error log
		char* infoLog = new char[logLength];
		glGetShaderInfoLog(shader, logLength, nullptr, infoLog);

		// Abort
		throw std::runtime_error(infoLog);
	}

	return shader;
}

GLuint xr::Shader::linkShaderProgram(GLuint vertexShader, GLuint fragmentShader)
{
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	// Check for link errors
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);

	if (status == GL_FALSE) {
		// Get error log length
		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

		// Get error log
		char* infoLog = new char[logLength];
		glGetProgramInfoLog(program, logLength, nullptr, infoLog);

		// Abort
		throw std::runtime_error(infoLog);
	}

	return program;
}
