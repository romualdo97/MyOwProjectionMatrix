#pragma once
#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <glm/gtc/type_ptr.hpp> // for transformations
#include <string>
#include <iostream>

class Shader
{
public:
	// Constructor: Specify vertex and fragment shader path
	// Shader("MyFrag.frag", "myVert.vert");
	Shader(char const *vertexPath, char const *fragmentPath);

	// Constructor: Specify just a shader name (no shader file extension)
	// this constructor assumes the vertex and fragment shader are named equally
	// with its respective different file extensions, e.g: Shader("MyShader"); assumes
	// two files; MyShader.frag, MyShader.vert
	Shader(GLchar const *shaderName);

	// Copy-Constructor
	Shader(const Shader& other);

	// Move constructor
	Shader(Shader&& other) noexcept;

	// Copy assignment operator
	Shader& operator=(const Shader& other);

	// Move assignment operator
	Shader& operator=(Shader&& other) noexcept;

	// Destructor
	~Shader();

	// Use shader program
	void use() const;

	// Utility uniform functions
	void setMatrix(std::string const& name, glm::mat4x4& value) const;
	void setBool(std::string const &name, bool value) const;
	void setInt(std::string const &name, int value) const;
	void setFloat(std::string const &name, float value) const;
	void setFloatArray(std::string const& name, GLsizei arraySize, const float* firstItem) const;
	void setVec3(std::string const& name, glm::vec3& value) const;
	void setVec3(std::string const& name, float x, float y, float z) const;
	void setVec3Array(std::string const& name, GLsizei arraySize, glm::vec3 firstItem) const;
	unsigned int getId() const;

private:
	static unsigned int initShader(char const* vertex_src, char const* fragment_src);
	static unsigned int compileShader(char const *vertex_src, char const *fragment_src);
	static void checkCompileError(unsigned int shader, unsigned int stage, unsigned int const status);
	static char* readShaderFile(char const* shader_file_src);

	unsigned int id;
	const char* vertexSrcFilePath;
	const char* fragmentSrcFilePath;
};

