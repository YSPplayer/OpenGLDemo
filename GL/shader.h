#pragma once
#include <glad/glad.h> 
#include <string>
#include <glm/glm.hpp>
namespace GL {
	class Shader {
	public:
		Shader();
		~Shader();
		void UseShader();
		bool CreateShader(const std::string& vertexShader, const std::string& colorShader);
		void SetShaderMat4(const glm::mat4& mat4, const std::string& key);
		void SetShaderBoolean(bool value,const std::string& key);
	private:
		GLuint CreateShader(GLuint type, const std::string& strShader);
		GLuint shaderProgram;//着色器程序管理对象
	};
}