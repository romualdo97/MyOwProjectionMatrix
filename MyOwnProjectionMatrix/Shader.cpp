
#include "Shader.h"
#include <fstream>
#include <sstream>

// for transformations
#include <glm\gtc\type_ptr.hpp>

Shader::Shader(GLchar const *vertexPath, GLchar const *fragmentPath) : vertexSrcFilePath(vertexPath), fragmentSrcFilePath(fragmentPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::cout << "Shader instanced will find source files at:\n\t";
	std::cout << vertexPath << " and " << fragmentPath << "\n";

	id = initShader(vertexPath, fragmentPath);
}

Shader::Shader(GLchar const *shaderName)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::stringstream ssVertexPath, ssFragmentPath;
	ssVertexPath << shaderName << ".vert";
	ssFragmentPath << shaderName  << ".frag";
	const std::string vertexPath = ssVertexPath.str();
	const std::string fragmentPath = ssFragmentPath.str();

	std::cout << "Shader instanced will find source files at:\n\t";
	char const *vertexPath_cstr = vertexPath.c_str();
	char const *fragmentPath_cstr = fragmentPath.c_str();	
	std::cout << vertexPath_cstr << " and " << fragmentPath_cstr << "\n";

	id = initShader(vertexPath_cstr, fragmentPath_cstr);
	vertexSrcFilePath = vertexPath_cstr;
	fragmentSrcFilePath = fragmentPath_cstr;
}

Shader::Shader(const Shader& other)
{
	id = initShader(other.vertexSrcFilePath, other.fragmentSrcFilePath);
	vertexSrcFilePath = other.vertexSrcFilePath;
	fragmentSrcFilePath = other.fragmentSrcFilePath;
}

Shader& Shader::operator=(const Shader& other)
{
	if (&other == this)
	{
		return *this;
	}

	id = initShader(other.vertexSrcFilePath, other.fragmentSrcFilePath);
	vertexSrcFilePath = other.vertexSrcFilePath;
	fragmentSrcFilePath = other.fragmentSrcFilePath;
	return *this;
}

Shader::Shader(Shader&& other) noexcept
{
	id = other.id;
	vertexSrcFilePath = other.vertexSrcFilePath;
	fragmentSrcFilePath = other.fragmentSrcFilePath;

	other.id = 0;
	other.vertexSrcFilePath = nullptr;
	other.fragmentSrcFilePath = nullptr;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
	if (&other == this)
	{
		return *this;
	}

	id = other.id;
	vertexSrcFilePath = other.vertexSrcFilePath;
	fragmentSrcFilePath = other.fragmentSrcFilePath;

	other.id = 0;
	other.vertexSrcFilePath = nullptr;
	other.fragmentSrcFilePath = nullptr;

	return *this;
}

unsigned Shader::initShader(char const* vertexPath_cstr, char const* fragmentPath_cstr)
{
	// convert from string to c-like string
	char const* vertexCode_cstr = readShaderFile(vertexPath_cstr);
	char const* fragmentCode_cstr = readShaderFile(fragmentPath_cstr);

	// =====================================================
	// 2. compile shader
	const unsigned int shaderId = compileShader(vertexCode_cstr, fragmentCode_cstr);

	// =====================================================
	// 3. Clear memory
	delete vertexCode_cstr;
	delete fragmentCode_cstr;

	return shaderId;
}


unsigned int Shader::compileShader(char const *vertex_src, char const *fragment_src)
{
	// compile vertex shader
	const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertex_src, NULL);
	glCompileShader(vertexShader);
	checkCompileError(vertexShader, GL_VERTEX_SHADER, GL_COMPILE_STATUS);

	// compile fragment shader
	const unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragment_src, NULL);
	glCompileShader(fragmentShader);
	checkCompileError(fragmentShader, GL_FRAGMENT_SHADER, GL_COMPILE_STATUS);

	// link shader program
	const unsigned int shaderId = glCreateProgram();
	glAttachShader(shaderId, vertexShader);
	glAttachShader(shaderId, fragmentShader);
	glLinkProgram(shaderId);
	checkCompileError(shaderId, NULL, GL_LINK_STATUS);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderId;
}

void Shader::checkCompileError(unsigned int shader, unsigned int stage, unsigned int const status)
{
	char infoLog[512];
	int success;

	if (status == GL_COMPILE_STATUS && stage == GL_FRAGMENT_SHADER)
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
			std::cout << "ERROR WHILE COMPILING FRAGMENT SHADER\n";
			std::cout << infoLog << std::endl;
		}
	}

	if (status == GL_COMPILE_STATUS && stage == GL_VERTEX_SHADER)
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
			std::cout << "ERROR WHILE COMPILING VERTEX SHADER\n";
			std::cout << infoLog << std::endl;
		}
	}

	if (status == GL_LINK_STATUS)
	{
		glGetShaderiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
			std::cout << "ERROR WHILE LINKING SHADER PROGRAM\n";
			std::cout << infoLog << std::endl;
		}
	}
}

char* Shader::readShaderFile(char const* shader_file_src)
{
	std::ifstream file;
	std::string shaderFileData;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		std::stringstream ssFile;
		file.open(shader_file_src);
		ssFile << file.rdbuf();
		file.close();
		shaderFileData = ssFile.str();
	}
	catch (const std::ifstream::failure& e)
	{
		std::cout << "ERROR TRYING TO READ SHADER FILES\n";
	}

	// Convert to c-string and store on heap
	char* source = new char[shaderFileData.size() + 1];
	for (auto current = shaderFileData.cbegin(); current < shaderFileData.cend(); ++current)
	{
		source[current - shaderFileData.cbegin()] = *current;
	}
	source[shaderFileData.size()] = NULL;
	return source;
}

void Shader::use() const
{
	glUseProgram(id);
}

void Shader::setMatrix(std::string const& name, glm::mat4x4& value) const
{
	const GLint uniformLoc = glGetUniformLocation(id, name.c_str());
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setBool(std::string const &name, bool value) const
{
	const GLint uniformLoc = glGetUniformLocation(id, name.c_str());
	glUniform1i(uniformLoc, value);
}

void Shader::setInt(std::string const &name, int value) const
{
	const GLint uniformLoc = glGetUniformLocation(id, name.c_str());
	glUniform1i(uniformLoc, value);
}

void Shader::setFloat(std::string const &name, float value) const
{
	const GLint uniformLoc = glGetUniformLocation(id, name.c_str());
	glUniform1f(uniformLoc, value);
}

void Shader::setFloatArray(std::string const& name, GLsizei arraySize, const float* firstItem) const
{
	const GLint uniformLoc = glGetUniformLocation(id, name.c_str());
	glUniform3fv(uniformLoc, arraySize, firstItem);
}

void Shader::setVec3(std::string const& name, glm::vec3& value) const
{
	const GLint uniformLoc = glGetUniformLocation(id, name.c_str());
	glUniform3fv(uniformLoc, 1, &(value.x));
}

void Shader::setVec3(std::string const& name, float x, float y, float z) const
{
	const GLint uniformLoc = glGetUniformLocation(id, name.c_str());
	glUniform3f(uniformLoc, x, y, z);
}

void Shader::setVec3Array(std::string const& name, GLsizei arraySize, glm::vec3 firstItem) const
{
	const GLint uniformLoc = glGetUniformLocation(id, name.c_str());
	glUniform3fv(uniformLoc, arraySize, &(firstItem.x));
}

unsigned Shader::getId() const
{
	return id;
}

Shader::~Shader()
{
	glDeleteProgram(id);
	id = 0;
}
