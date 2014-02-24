#ifndef engine
#define engine
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "include/GL/glew.h"
#include "include/GLFW/glfw3.h"
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "bsp.h"


class Engine
{
private:

public:
	GLFWwindow* window;
	void openWindow (const char*);

	GLuint CreateShader (const char* VertexFP, const char* FragmentFP);
};

#endif