#pragma once

namespace xr {
	class Shader
	{
		// Handle to the OpenGL shader
		GLuint program;

	public:

		Shader(const char* vertexSource, const char* fragmentSource);
		~Shader();


		// Use this shader
		void use();


		// Bind a attribute name to a location
		void bindAttribute(GLuint location, const char* name);


		// Get the uniform's location from it's name
		GLuint getUniformLocation(const char* name);

	private:

		// Create a new shader from source
		GLuint compileShaderSource(const char* source, GLenum type);

		// Create a new shader program from shaders
		GLuint linkShaderProgram(GLuint vertexShader, GLuint fragmentShader);

	};
}
