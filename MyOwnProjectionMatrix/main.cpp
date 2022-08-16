// for OpenGL context and its commands
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Cpp libraries
#include <iostream>

// for data loading and shader compiling
#include "Shader.h"
#include <stb/stb_image.h>

// for transformations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define W 820
#define H 820
#define WINDOW_TITLE "Animating mesh using matrix transformations"

// use winding order to draw correctly cube faces? else use dept testing method
#define USE_CULL_FACE

void resize_framebuffer_cb(GLFWwindow* window, int w, int h);
void process_input(GLFWwindow* window);
const float* my_perspective(float fovY, float aspect, float near, float far);

int main(void)
{
	// configure window and context
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create window
	GLFWwindow* window = glfwCreateWindow(W, H, WINDOW_TITLE, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Window could not be created\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// init glad for call OpenGL functions
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cout << "Could not load GLAD\n";
		return -1;
	}

	// register GLFW events
	glfwSetFramebufferSizeCallback(window, resize_framebuffer_cb);

	// ======================================================================
	// create new shader
	const Shader myShader("Shaders/myShader");

	// ======================================================================
	// tell stb library to flip images in load
	stbi_set_flip_vertically_on_load(true);

	// load texture data using stb_image header library
	int i_w, i_h, nrChannels;
	unsigned char* image_data = stbi_load("wall.jpg", &i_w, &i_h, &nrChannels, 0);

	// load awesomeface image data
	int i2_w, i2_h, nrChannels2;
	unsigned char* image_data2 = stbi_load("awesomeface.png", &i2_w, &i2_h, &nrChannels2, 0);


	// ======================================================================
	// generate first texture
	unsigned int texture;
	glGenTextures(1, &texture);
	// bind for configuration
	glActiveTexture(GL_TEXTURE0); // activate texture unit 0
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	// configure wrap mode in s and t dimensions
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// configure minification and magnification filters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// put imge_data to previously created opengl texture object
	if (image_data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, i_w, i_h, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Error loading texture data\n";
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image_data); // free memory

	// generate second texture
	unsigned int texture2;
	glGenTextures(1, &texture2);
	// activate texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	if (image_data2)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, i2_w, i2_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Error loading texture data\n";

	}

	stbi_image_free(image_data2);
	glBindTexture(GL_TEXTURE_2D, 0);

	// ======================================================================

	// triangle vertex data 
	constexpr float vertex_data[] = {
		// FRONT (Z-POSITIVE)
		// positions (in NDC)				// colors						// texture coordinates
		-0.5f, 0.5f, 0.5f,	/*left-top*/	0.0f, 0.0f, 1.0f,	/*red*/		0.0, 1.0,
		0.5f, 0.5f, 0.5f,	/*right-top*/	0.0f, 0.0f, 1.0f,	/*green*/	1.0, 1.0,
		-0.5f, -0.5f, 0.5f,	/*left-bttm*/	0.0f, 0.0f, 1.0f,	/*blue*/	0.0, 0.0,
		0.5f, -0.5f, 0.5f,	/*right-bttm*/  0.0f, 0.0f, 1.0f,	/*#0ff*/	1.0, 0.0,

		// BACK (Z-NEGATIVE)
		// positions (in NDC)				// colors						// texture coordinates
		-0.5f, 0.5f, -0.5f,	/*left-top*/	0.0f, 0.0f, 0.5f,	/*red*/		0.0, 1.0,
		0.5f, 0.5f, -0.5f,	/*right-top*/	0.0f, 0.0f, 0.5f,	/*green*/	1.0, 1.0,
		-0.5f, -0.5f, -0.5f,/*left-bttm*/	0.0f, 0.0f, 0.5f,	/*blue*/	0.0, 0.0,
		0.5f, -0.5f, -0.5f,	/*right-bttm*/  0.0f, 0.0f, 0.5f,	/*#0ff*/	1.0, 0.0,

		// LEFT (X-NEGATIVE)
		// positions (in NDC)				// colors						// texture coordinates
		-0.5f, 0.5f, -0.5f,	/*left-top*/	0.5f, 0.0f, 0.0f,	/*red*/		0.0, 1.0,
		-0.5f, 0.5f, 0.5f,	/*right-top*/	0.5f, 0.0f, 0.0f,	/*green*/	1.0, 1.0,
		-0.5f, -0.5f, -0.5f, /*left-bttm*/	0.5f, 0.0f, 0.0f,	/*blue*/	0.0, 0.0,
		-0.5f, -0.5f, 0.5f, /*right-bttm*/  0.5f, 0.0f, 0.0f,	/*#0ff*/	1.0, 0.0,

		// RIGHT (X-POSITIVE)
		// positions (in NDC)				// colors						// texture coordinates
		0.5f, 0.5f, -0.5f,	/*left-top*/	1.0f, 0.0f, 0.0f,	/*red*/		0.0, 1.0,
		0.5f, 0.5f, 0.5f,	/*right-top*/	1.0f, 0.0f, 0.0f,	/*green*/	1.0, 1.0,
		0.5f, -0.5f, -0.5f, /*left-bttm*/	1.0f, 0.0f, 0.0f,	/*blue*/	0.0, 0.0,
		0.5f, -0.5f, 0.5f, /*right-bttm*/   1.0f, 0.0f, 0.0f,	/*#0ff*/	1.0, 0.0,

		// TOP (Y-POSITIVE)
		// positions (in NDC)				// colors						// texture coordinates
		-0.5f, 0.5f, -0.5f,	/*left-top*/	0.0f, 1.0f, 0.0f,	/*red*/		0.0, 1.0,
		0.5f, 0.5f, -0.5f,	/*right-top*/	0.0f, 1.0f, 0.0f,	/*green*/	1.0, 1.0,
		-0.5f, 0.5f, 0.5f, /*left-bttm*/	0.0f, 1.0f, 0.0f,	/*blue*/	0.0, 0.0,
		0.5f, 0.5f, 0.5f, /*right-bttm*/    0.0f, 1.0f, 0.0f,	/*#0ff*/	1.0, 0.0,

		// BOTTOM (Y-NEGATIVE)
		// positions (in NDC)				// colors						// texture coordinates
		-0.5f, -0.5f, -0.5f,/*left-top*/	0.0f, 0.5f, 0.0F,	/*red*/		0.0, 1.0,
		-0.5f, -0.5f, 0.5f,	/*right-top*/	0.0f, 0.5f, 0.0f,	/*green*/	1.0, 1.0,
		0.5f, -0.5f, -0.5f, /*left-bttm*/	0.0f, 0.5f, 0.0f,	/*blue*/	0.0, 0.0,
		0.5f, -0.5f, 0.5f, /*right-bttm*/   0.0f, 0.5f, 0.0f,	/*#0ff*/	1.0, 0.0
	};

	// index drawing data for draw a cube
	unsigned int constexpr BACK_OFFSET = 4;
	unsigned int constexpr LEFT_OFFSET = 8;
	unsigned int constexpr RIGHT_OFFSET = 12;
	unsigned int constexpr TOP_OFFSET = 16;
	unsigned int constexpr BOTTOM_OFFSET = 20;

#ifdef USE_CULL_FACE
	unsigned int index_drawing_data[] = {
		// FRONT (Z-POSITIVE) specified in "clock wise" "winding order"
		0, 1, 2, /*first triangle*/
		1, 3, 2, /*second triangle*/

		// BACK (Z-NEGATIVE) specified in "counter-clock wise" "winding order"
		0 + BACK_OFFSET, 2 + BACK_OFFSET, 1 + BACK_OFFSET, /*first triangle*/
		1 + BACK_OFFSET, 2 + BACK_OFFSET, 3 + BACK_OFFSET, /*second triangle*/

		// LEFT (X-NEGATIVE) specified in "clock wise" "winding order"
		0 + LEFT_OFFSET, 1 + LEFT_OFFSET, 2 + LEFT_OFFSET, /*first triangle*/
		1 + LEFT_OFFSET, 3 + LEFT_OFFSET, 2 + LEFT_OFFSET, /*second triangle*/

		// LEFT (X-NEGATIVE) specified in "counter-clock wise" "winding order"
		0 + RIGHT_OFFSET, 2 + RIGHT_OFFSET, 1 + RIGHT_OFFSET, /*first triangle*/
		1 + RIGHT_OFFSET, 2 + RIGHT_OFFSET, 3 + RIGHT_OFFSET, /*second triangle*/

		// TOP (Y-POSITIVE) specified in "counter-clock wise" "winding order"
		0 + TOP_OFFSET, 1 + TOP_OFFSET, 2 + TOP_OFFSET, /*first triangle*/
		1 + TOP_OFFSET, 3 + TOP_OFFSET, 2 + TOP_OFFSET, /*second triangle*/

		// TOP (Y-POSITIVE) specified in "counter-clock wise" "winding order"
		0 + BOTTOM_OFFSET, 1 + BOTTOM_OFFSET, 2 + BOTTOM_OFFSET, /*first triangle*/
		1 + BOTTOM_OFFSET, 3 + BOTTOM_OFFSET, 2 + BOTTOM_OFFSET, /*second triangle*/
	};
#else
	unsigned int index_drawing_data[] = {
		// FRONT (Z-POSITIVE) specified in "clock wise" "winding order"
		0, 1, 2, /*first triangle*/
		1, 3, 2, /*second triangle*/

		// BACK (Z-NEGATIVE) specified in "clock wise" "winding order"
		1 + BACK_OFFSET, 2 + BACK_OFFSET, 0 + BACK_OFFSET, /*first triangle*/
		3 + BACK_OFFSET, 2 + BACK_OFFSET, 1 + BACK_OFFSET, /*second triangle*/

		// LEFT (X-NEGATIVE) specified in "clock wise" "winding order"
		0 + LEFT_OFFSET, 1 + LEFT_OFFSET, 2 + LEFT_OFFSET, /*first triangle*/
		1 + LEFT_OFFSET, 3 + LEFT_OFFSET, 2 + LEFT_OFFSET, /*second triangle*/

		// LEFT (X-NEGATIVE) specified in "clock wise" "winding order"
		1 + RIGHT_OFFSET, 2 + RIGHT_OFFSET, 0 + RIGHT_OFFSET, /*first triangle*/
		 3 + RIGHT_OFFSET, 2 + RIGHT_OFFSET, 1 + RIGHT_OFFSET, /*second triangle*/

		// TOP (Y-POSITIVE) specified in "clock wise" "winding order"
		2 + TOP_OFFSET, 1 + TOP_OFFSET, 0 + TOP_OFFSET, /*first triangle*/
		2 + TOP_OFFSET, 3 + TOP_OFFSET, 1 + TOP_OFFSET, /*second triangle*/

		// TOP (Y-POSITIVE) specified in "clock wise" "winding order"
		2 + BOTTOM_OFFSET, 1 + BOTTOM_OFFSET, 0 + BOTTOM_OFFSET, /*first triangle*/
		2 + BOTTOM_OFFSET, 3 + BOTTOM_OFFSET, 1 + BOTTOM_OFFSET, /*second triangle*/
	};
#endif // USE_CULL_FACE

	// generate VAO for store status of subsequent "vertex attribute" calls and element array buffer configs
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO); // bind VAO

	// generate VBO for allocate memory in GPU
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	// bind VBO with GL_ARRAY_BUFFER target
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// copy data from CPU to GPU
	//		use the currently bounded buffer to GL_ARRAY_BUFFER as container
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

	// tell OpenGL how it should interpret the vertex data(per
	// vertex attribute) using glVertexAttribPointer:
	// glVertexAttribPointer(index = [vertex attrib location remember the layout (location = n) keyword in vertex shader], 
	//						size = [is vec2 = 2, vec3 = 3, etc..],
	//						type = [GL_FLOAT, GL_BOOL, etc..], 
	//						normalize = [opengl should normalize the given data?],
	//						stride = [distance in bytes between each "position" ternas in VBO],
	//						start = [whare is the start index of "position"?];

	// indicate which part of vertex data are vertex positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // enable the vertex attribute at location 0

	// indicate which part of vertex data are vertex colors
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1); // enable the vertex attribute at location 1

	// indicate which part of vertex data are texture coordinates
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2); // enable the vertex attribute at location 2

	std::cout << "\n\nGL_ARRAY_BUFFER:\n";
	std::cout << "sizeof(float): " << sizeof(float) << " bytes" << std::endl;
	std::cout << "Num of indices at GL_ARRAY_BUFFER: " << sizeof(vertex_data) / sizeof(float) << std::endl;
	std::cout << "Size reserved for GL_ARRAY_BUFFER: " << sizeof(vertex_data) << " bytes" << std::endl;

	// generate EBO
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	// bind EBO to GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// pass EBO data from CPU to GPU
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_drawing_data), index_drawing_data, GL_STATIC_DRAW);

	std::cout << "\n\nGL_ELEMENT_ARRAY_BUFFER:\n";
	std::cout << "sizeof(unsigned int): " << sizeof(unsigned int) << " bytes" << std::endl;
	std::cout << "Num of indices at GL_ELEMENT_ARRAY_BUFFER: " << sizeof(index_drawing_data) / sizeof(unsigned int) << std::endl;
	std::cout << "Size reserved for GL_ELEMENT_ARRAY_BUFFER: " << sizeof(index_drawing_data) << " bytes" << std::endl;

	std::cout << "\n\nTOTAL BYTES SENT TO GPU: " << sizeof(index_drawing_data) + sizeof(vertex_data) << " bytes" << std::endl;
	// ======================================================================
	// update and draw commands

	// start viewport
	glViewport(0, 0, W, H);

	// define the winding order
	glFrontFace(GL_CW); // defines "winding order" for specify which triangle side is considered the "front" face 
#ifdef USE_CULL_FACE
	glEnable(GL_CULL_FACE); // enable the face culling
	glCullFace(GL_BACK); // cull back faces
#else
	glEnable(GL_DEPTH_TEST);
#endif // USE_CULL_FACE
	/*bind opengl object "texture" to GL_TEXTURE_2D target
	in texture unit 0 (GL_TEXTURE0)*/
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	/*bind opengl object "texture2" to GL_TEXTURE_2D target
	in texture unit 1 (GL_TEXTURE0)*/
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	/*
	Note that finding the uniform location does not require
	you to use the shader program first, but updating a uniform
	does require you to first use the program (by calling
	glUseProgram), because it sets the uniform on the currently
	active shader program.
	*/

	// use shader program and pass uniforms
	myShader.use();	//glUseProgram(shader_program); // use a shader program

	// locate uTime uniform
	const int uTimeLoc = glGetUniformLocation(myShader.getId(), "uTime");

	// specify what texture unit should use the uniform GLSL sampler uTextureA
	const int uTexALoc = glGetUniformLocation(myShader.getId(), "uTextureA");
	glUniform1i(uTexALoc, 0); // use texture unit 0

	// specify what texture unit should use the uniform GLSL sampler uTextureB
	const int uTexBLoc = glGetUniformLocation(myShader.getId(), "uTextureB");
	glUniform1i(uTexBLoc, 1); // use texture unit 1

	// get uniform uModel, uView and uProj location at shader program
	const int uModelLoc = glGetUniformLocation(myShader.getId(), "uModel");
	const int uViewLoc = glGetUniformLocation(myShader.getId(), "uView");
	const int uProjLoc = glGetUniformLocation(myShader.getId(), "uProj");

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.5f, 0.2f, 1.0f); // set the clear color
#ifdef USE_CULL_FACE
		glClear(GL_COLOR_BUFFER_BIT);
#else
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear color buffer bitfield
#endif // USE_CULL_FACE

		process_input(window);

		const auto t = static_cast<float>(glfwGetTime());
		float s = 0.5f + 0.5f * sin(static_cast<float>(glfwGetTime()));

		glUniform1f(uTimeLoc, t);

		// create model matrix
		glm::mat4 model;
		model = glm::rotate(model, t, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, t, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uModelLoc, 1, GL_FALSE, glm::value_ptr(model)); // GLint location,	GLsizei count,	GLboolean transpose, const GLfloat* value

		// create view matrix
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); // note that we're translating the scene in the reverse direction of where we want to move
		glUniformMatrix4fv(uViewLoc, 1, GL_FALSE, glm::value_ptr(view));

		// create projection matrix
		/*glm::mat4 projection;
		projection = glm::perspective(glm::radians(45.0f), static_cast<float>(W / H), 0.1f, 100.0f);
		glUniformMatrix4fv(uProjLoc, 1, GL_FALSE, glm::value_ptr(projection));*/

		const float* myOwnProjectionMatrix = my_perspective(glm::radians(45.0f), static_cast<float>(W / H), 0.1f, 50.0f);
		glUniformMatrix4fv(uProjLoc, 1, false, myOwnProjectionMatrix);
		delete myOwnProjectionMatrix; // Free heap memory

		glBindVertexArray(VAO); // bind object VAO

		// read more at: https://people.eecs.ku.edu/~jrmiller/Courses/672/InClass/3DModeling/glDrawElements.html
		// glDrawArrays(GL_TRIANGLES, 0, 3); // draw triangle
		constexpr int vertices_per_triangle = 3;
		constexpr int num_of_triangles = 12;
		constexpr GLenum mode = GL_TRIANGLES; // Specifies what kind of primitives to render.
		constexpr GLsizei count = vertices_per_triangle * num_of_triangles; // Specifies the number of elements to be rendered.
		constexpr GLenum type = GL_UNSIGNED_INT; // Specifies the type of the values in indices.Must be one of GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, or GL_UNSIGNED_INT.
		const GLvoid* indices = nullptr; // Specifies a pointer to the location where the indices are stored
		// Passing nullptr as the final parameter to glDrawElements tells the vertex fetch processor to use the currently bound element buffer object when extracting per - vertex data for vertex shader executions.
		glDrawElements(mode, count, type, indices); // draw a quad

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}

void resize_framebuffer_cb(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		glfwSetWindowShouldClose(window, true);
	}
}

const float* my_perspective(const float fovY, const float aspect, const float near, const float far)
{
	// OpenGL is column major... so it expects something like { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, transX, transY, transZ, 1 }
	// Formula and theory from here: https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/opengl-perspective-projection-matrix
	//std::unique_ptr<float> projectionMatrix = std::unique_ptr<float>{  };
	const auto projectionMatrix = new float[16];

	// Calculate top and bottom (at near plane)
	const float top = tan(fovY / 2.0f) * near;
	const float bottom = -top;

	// Calculate left and right (at near plane)
	const float left = -top * aspect;
	const float right = top * aspect;
	
	// First column
	projectionMatrix[0] = (2.0f * near) / (right - left);
	projectionMatrix[1] = 0.0f;
	projectionMatrix[2] = 0.0f;
	projectionMatrix[3] = 0.0f;

	// Second column
	projectionMatrix[4] = 0.0f;
	projectionMatrix[5] = (2.0f * near) / (top - bottom);
	projectionMatrix[6] = 0.0f;
	projectionMatrix[7] = 0.0f;
	
	// Second column
	projectionMatrix[8] = (right + left) / (right - left);
	projectionMatrix[9] = (top + bottom) / (top - bottom);
	projectionMatrix[10] = -(far + near) / (far - near);
	projectionMatrix[11] = -1.0f;
	
	// Second column
	projectionMatrix[12] = 0.0f;
	projectionMatrix[13] = 0.0f;
	projectionMatrix[14] = -(2.0f * far * near) / (far - near);
	projectionMatrix[15] = 0.f;

	return projectionMatrix;
}
