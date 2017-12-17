#pragma once
#include "Shader.h"
#include "Vertex.h"
#include "Buffer.h"
#include "Texture.h"

#include "Mesh.h"

namespace xr {
	
	enum RendererCommand {
		// Draws the next n indices of current mesh {n}
		DRAW_INDICES,

		// Switches to the next camera matrix
		NEXT_CAMERA,

		// Switches to the next texture
		NEXT_TEXTURE,

		// Switches to another mesh {index}
		SWITCH_MESH
	};

	struct RendererSequence {
		// Sequence of commands
		std::vector<RendererCommand> commands;

		// Parameters of the commands
		std::vector<int> parameters;

		// List of Meshes
		std::vector<Mesh> meshes;

		// Vertex and index data
		//std::vector<Vertex> vertices;
		//std::vector<GLuint> indices;

		// Camera matrices
		std::vector<glm::mat4> cameraMatrices;

		// Textures
		std::vector<Texture> textures;

		// Clear all data from the sequence
		void clear() {
			commands.clear();
			parameters.clear();
			meshes.clear();
			cameraMatrices.clear();
			textures.clear();
		}
	};


	class Renderer
	{
		// Main shader
		Shader shader;

		// Vertex buffer
		VertexBuffer vertexBuffer;

		// Uniform locations
		struct UniformLocations {
			GLuint cameraMatrix;
			GLuint texture0;
		} uniformLocations;

	public:

		// Create a new renderer
		Renderer();


		// Clear color and depth buffer
		void clear(float r, float g, float b, float a);


		// Submit a sequence to the renderer
		void submit(const RendererSequence& sequence);
	};
}

