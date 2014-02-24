// Camera class.
#include "Engine.h"

class Camera
{
	private:
	glm::mat4 Projection, View; // Two matricies for controlling the camera.
	
	float FoV; // Field of View
	float CameraSpeed; // Speed that the camera moves at.
	float MouseSpeed; // Speed of the mouse, which turns the camera.
	
	glm::vec3 CPosition, CDirection, CUp, CRight; // Various directions for the camera to work.
	
	double MouseX, MouseY, LastX, LastY; // Mouse movement.
	glm::vec2 MouseDelta; // Mouse movement.
	
	float MaxAngle; // Maximum verticle angle.
	
	public:
	Camera(unsigned int Width, unsigned int Height, GLFWwindow* Window); // Init.
	void Tick (float DeltaTime, GLFWwindow* win);
	glm::mat4 GetMVP(glm::mat4 m); // Get the final MVP (Projection x View x Model).
	glm::mat4 GetProjection();
	glm::mat4 GetView();
	
	void SetFoV(float);
	float GetFoV();
	
	void SetCameraSpeed(float);
	float GetCameraSpeed();

	glm::vec3 GetCameraPosition();
};