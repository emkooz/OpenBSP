#include "Engine.h"

void Engine::openWindow (const char* name)
{
	glfwInit();
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint (GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow (1280, 720, name, nullptr, nullptr);
	glfwMakeContextCurrent (window);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LESS);

	glEnable (GL_PRIMITIVE_RESTART); // experimental
	glPrimitiveRestartIndex (999988887777);
}

GLuint Engine::CreateShader(const char* VertexFP, const char* FragmentFP)
{
	// Read the shaders.
	std::ifstream ifs(VertexFP);
	std::stringstream vss;
	vss << ifs.rdbuf();
	std::string vstring = vss.str();
	ifs.close();
	
	std::cout << "\nCompiling: " << VertexFP << std::flush;
	// Compile.
	GLuint VertexShader = glCreateShader (GL_VERTEX_SHADER);
	const char * VSPointer = vstring.c_str();
	glShaderSource (VertexShader, 1, &VSPointer, NULL);
	glCompileShader (VertexShader);
	
	// Check for errors.
	GLint status;
	glGetShaderiv (VertexShader, GL_COMPILE_STATUS, &status);
	char buf[512];
	glGetShaderInfoLog (VertexShader, 512, NULL, buf);
	std::cout << std::endl << buf;

	ifs.open(FragmentFP);
	std::stringstream fss;
	fss << ifs.rdbuf();
	std::string fstring = fss.str();
	ifs.close();
	
	std::cout << "\nCompiling: " << FragmentFP << std::flush;
	GLuint FragmentShader = glCreateShader (GL_FRAGMENT_SHADER);
	const char * FSPointer = fstring.c_str();
	glShaderSource (FragmentShader, 1, &FSPointer, NULL);
	glCompileShader (FragmentShader);
	
	glGetShaderiv (FragmentShader, GL_COMPILE_STATUS, &status);
	glGetShaderInfoLog (FragmentShader, 512, NULL, buf);
	std::cout << std::endl << buf;
	
	// Create the shader program.
	GLuint ShaderProgram = glCreateProgram();
	glAttachShader (ShaderProgram, VertexShader);
	glAttachShader (ShaderProgram, FragmentShader);
	glBindFragDataLocation (ShaderProgram, 0, "color"); // Does NOT work with GLSL 1.2
	
	std::cout << "Linking..." << std:: endl;
	glLinkProgram (ShaderProgram);
	
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &status);
    glGetProgramInfoLog(ShaderProgram, 512, NULL, buf);
    fprintf(stdout, "%s\n", buf);
        
        if (status == GL_FALSE)
        {
			std::cout << "Shaders were unable to link!\n";
			return 0;
		}
	
	glDeleteShader( FragmentShader );
	glDeleteShader( VertexShader );
	
	return ShaderProgram;
};