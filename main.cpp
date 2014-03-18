#include "Engine.h"
#include "Camera.h"


// https://developer.valvesoftware.com/wiki/Source_BSP_File_Format
// https://gist.github.com/aylaylay/9d910fa144b56ca7d0dc

// messsssyyyyyy
// minor todo: add comments to main

int main()
{
	bsp map ("de_dust2.bsp");
	Engine gl;
	
	gl.openWindow ("BSP");

	//glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	glfwSetInputMode (gl.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Camera PlayerCamera (1280, 720, gl.window);
	map.LoadMapDetails(PlayerCamera.GetCameraPosition());

	GLuint vao;
	glGenVertexArrays (1, &vao);
	glBindVertexArray (vao);

	GLuint vbo;
	glGenBuffers (1, &vbo);

		glBindBuffer (GL_ARRAY_BUFFER, vbo);
		glBufferData (GL_ARRAY_BUFFER, (sizeof(float) * 3) * map.vertices.size(), &map.vertices[0], GL_STATIC_DRAW);

	GLuint normalVBO;
	glGenBuffers (1, &normalVBO);

		glBindBuffer (GL_ARRAY_BUFFER, normalVBO);
		glBufferData (GL_ARRAY_BUFFER, (sizeof (float) * 3) * map.mapNormals.size(), &map.mapNormals[0], GL_STATIC_DRAW);
		
	GLuint ebo;
	glGenBuffers (1, &ebo);

		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (GLuint) * map.indices.size(), &map.indices[0], GL_STATIC_DRAW);

	GLuint shader = gl.CreateShader ("basic_shader.vert", "basic_shader.frag");
	glUseProgram (shader);

	glBindBuffer (GL_ARRAY_BUFFER, vbo);

		GLint posAttrib = glGetAttribLocation (shader, "position");
		glEnableVertexAttribArray (posAttrib);
		glVertexAttribPointer (posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer (GL_ARRAY_BUFFER, normalVBO);
		
		GLint nPosAttrib = glGetAttribLocation (shader, "nColor");
		glEnableVertexAttribArray (nPosAttrib);
		glVertexAttribPointer (nPosAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
		
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
		
		GLint view = glGetUniformLocation (shader, "view");
		glUniformMatrix4fv (view, 1, GL_FALSE, glm::value_ptr (PlayerCamera.GetView()));

		GLint projection = glGetUniformLocation (shader, "projection");
		glUniformMatrix4fv (projection, 1, GL_FALSE, glm::value_ptr (PlayerCamera.GetProjection()));

		glm::mat4 Model (1.0f);
		Model = glm::rotate (Model, 90.0f, glm::vec3 (1, 0, 1));
		GLint model = glGetUniformLocation (shader, "model");
		glUniformMatrix4fv (model, 1, GL_FALSE, glm::value_ptr (Model));
	
	float LastTime = glfwGetTime();

	while (!glfwWindowShouldClose (gl.window))
	{
		float DeltaTime = glfwGetTime() - LastTime;
		LastTime = glfwGetTime();

		PlayerCamera.Tick (DeltaTime, gl.window);
		glUniformMatrix4fv (view, 1, GL_FALSE, glm::value_ptr (PlayerCamera.GetView()));
		glUniformMatrix4fv (projection, 1, GL_FALSE, glm::value_ptr (PlayerCamera.GetProjection()));
		glUniformMatrix4fv (model, 1, GL_FALSE, glm::value_ptr (glm::mat4 (1.0f)));

		if (glfwGetKey (gl.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose (gl.window, GL_TRUE);

		glClearColor (0.8f , 0.8f , 0.8f , 1.0f);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffer.

		glDrawElements (GL_TRIANGLES, map.indices.size(), GL_UNSIGNED_INT, 0);
		
		glfwSwapBuffers (gl.window);
		glfwPollEvents ();
	}

	glfwTerminate();
	
	return 0;
}