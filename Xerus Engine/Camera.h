#pragma once


namespace xr {
	class Camera
	{
	protected:

		// Projection matrix
		glm::mat4 projection;

		// View matrix
		glm::mat4 view;


		// Position of the camera
		glm::vec3 position;

		// The direction the camera is facing
		glm::vec3 direction;

		// The up-direction
		glm::vec3 up;

		// Create a new camera from raw parts
		Camera(glm::mat4 projection, glm::vec3 position, glm::vec3 direction, glm::vec3 up = {0, 1, 0});

	public:
		

		// Set the camera's position
		void setPosition(glm::vec3 position);
		void setPosition(glm::vec2 position) { this->setPosition(glm::vec3(position, 0)); }

		// Set the camera's direction
		void setDirection(glm::vec3 direction);

		// Return the combined transformation matrix
		glm::mat4 getTransform() const;


		// Converts screen coordinates [-1, 1] to world space
		glm::vec3 screenToWorld(glm::vec2 screen);


	private:

		// Update the view matrix
		void updateView();
	};


	class OrthographicCamera : public Camera
	{
		
	public:

		// Standard 2D
		OrthographicCamera(float width, float height);

		// Custom
		OrthographicCamera(float left, float right, float top, float bottom);

		// Update the projection
		void setProjection(float left, float right, float top, float bottom);
		void setProjection(float width, float height) { this->setProjection(0, width, 0, height); }
	};
}

