#include "Camera.h"

Camera::Camera(unsigned int Width, unsigned int Height, GLFWwindow* Window)
{
	CPosition = glm::vec3 (0.0f, 0.0f, -3.0f);
	CDirection = glm::vec3 (0.0f, 0.0f, 0.0f);
	FoV = 65.f;
	Projection = glm::perspective (FoV, (float)Width / (float)Height, 0.1f, 1000.f);
	View = glm::lookAt (
			CPosition,
			CDirection,
			glm::vec3 (0.0f, 1.0f, 0.0f));	
	CameraSpeed = 500.f;
	MaxAngle = 0.89f;
	
	MouseSpeed = 0.003f;		
	glfwGetCursorPos (Window, &MouseX, &MouseY);
}

void Camera::Tick (float dt, GLFWwindow* Window)
{
	LastX = MouseX; LastY = MouseY;
	glfwGetCursorPos (Window, &MouseX, &MouseY);
	
	MouseDelta.x += MouseSpeed * (LastX - MouseX);
	MouseDelta.y += MouseSpeed * (LastY - MouseY);
	
	CDirection = glm::vec3 (
				cos (MouseDelta.y) * sin (MouseDelta.x),
				sin (MouseDelta.y),
				cos (MouseDelta.y) * cos (MouseDelta.x)
						  );
						  
	//if (CDirection.y > MaxAngle)
	//if (CDirection.y < -MaxAngle)
	//std::cout << CDirection.z << std::endl;
						  
	CRight = glm::vec3 (
			sin (MouseDelta.x - 3.14f/2.0f),
			0,
			cos (MouseDelta.x - 3.14/2.0f)
					  );
					  
	CUp = glm::cross (CRight, CDirection);
	
	if (glfwGetKey (Window, GLFW_KEY_W))
		CPosition += CDirection * dt * CameraSpeed;
	if (glfwGetKey (Window, GLFW_KEY_S))
		CPosition -= CDirection * dt * CameraSpeed;
		
	if (glfwGetKey (Window, GLFW_KEY_D))
		CPosition += CRight * dt * CameraSpeed;
	if (glfwGetKey (Window, GLFW_KEY_A))
		CPosition -= CRight * dt * CameraSpeed;
		
	View = glm::lookAt (
						CPosition,
						CPosition + CDirection,
						CUp
					   );
	View = glm::rotate (View, 270.f, glm::vec3 (1, 0, 0));
}

glm::mat4 Camera::GetMVP (glm::mat4 model)
{
	return Projection * View * model;
}

glm::mat4 Camera::GetProjection ()
{
	return Projection;
}

glm::mat4 Camera::GetView()
{
	return View;
}

void Camera::SetFoV(float fov)
{
	FoV = fov;
}

float Camera::GetFoV()
{
	return FoV;
}

void Camera::SetCameraSpeed (float speed)
{
	CameraSpeed = speed;
}

float Camera::GetCameraSpeed ()
{
	return CameraSpeed;
}

glm::vec3 Camera::GetCameraPosition()
{
	return CPosition;
}