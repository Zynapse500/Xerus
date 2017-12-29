#pragma once


namespace xr {
	class Camera
	{
	protected:

		// Projection matrix
		glt::mat4f projection;

		// View matrix
		glt::mat4f view;


		// Position of the camera
		glt::vec3f position;

		// The direction the camera is facing
		glt::vec3f direction;

		// The up-direction
		glt::vec3f up;

		// Create a new camera from raw parts
		Camera(glt::mat4f projection, glt::vec3f position, glt::vec3f direction, glt::vec3f up = {0, 1, 0});

	public:
		

		// Set the camera's position
		void setPosition(glt::vec3f position);
		void setPosition(glt::vec2f position) { this->setPosition(glt::vec3f(position, 0)); }

		// Get the camera's position
		glt::vec3f getPosition();


		// Set the camera's direction
		void setDirection(glt::vec3f direction);

		// Return the combined transformation matrix
		glt::mat4f getTransform() const;


		// Converts screen coordinates [-1, 1] to world space
		glt::vec3f screenToWorld(glt::vec2f screen);


	private:

		// Update the view matrix
		void updateView();
	};


	class OrthographicCamera : public Camera
	{
		
	public:

		// Default [-1, 1]
		OrthographicCamera();

		// Standard 2D
		OrthographicCamera(float width, float height);

		// Custom
		OrthographicCamera(float left, float right, float top, float bottom);

		// Update the projection
		void setProjection(float left, float right, float top, float bottom);
		void setProjection(float width, float height) { this->setProjection(0, width, 0, height); }
	};
}

