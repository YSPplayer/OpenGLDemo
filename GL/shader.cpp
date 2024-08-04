#include "shader.h"
#include <iostream>
#include <vector>

#include <glm/gtc/type_ptr.hpp>
namespace GL {
	Shader::Shader() {
		shaderProgram = NULL;
	}

	Shader::~Shader() {
		//�ͷ���ɫ����������ڴ�
		if (shaderProgram) glDeleteProgram(shaderProgram);
	}

	void Shader::UseShader() {
		glUseProgram(shaderProgram);
	}

	/// <summary>
	/// �󶨶�����ɫ��+Ƭ����ɫ������
	/// </summary>
	/// <param name="vertexShader"></param>
	/// <param name="colorShader"></param>
	/// <returns></returns>
	bool Shader::CreateShader(const std::string& vertexShader, const std::string& colorShader) {
		shaderProgram = glCreateProgram();//��ɫ������������ڹ�������ɫ������
		GLuint vShader = CreateShader(GL_VERTEX_SHADER, vertexShader); //������ɫ��
		GLuint cShader = CreateShader(GL_FRAGMENT_SHADER, colorShader); //Ƭ����ɫ��
		if (vShader && cShader) {
			glAttachShader(shaderProgram, vShader);
			glAttachShader(shaderProgram, cShader);
			glLinkProgram(shaderProgram);
			glValidateProgram(shaderProgram);
			//�������֮���ͷ�ָ���ڴ�
			glDeleteShader(vShader);
			glDeleteShader(cShader);
			int success;
			std::string infoLog;
			glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shaderProgram, 512, NULL, &infoLog[0]); //��ȡ����Ĵ�����Ϣ
				std::cout << "[Error]{Shader::BindShader}" << infoLog << std::endl;
				return false;//����������ʧ�� 
			}
			return true;
		}
		return false;
	}

	/// <summary>
	/// ����shaderȫ�ֱ���
	/// </summary>
	/// <param name="mat4"></param>
	/// <param name="key"></param>
	void Shader::SetShaderMat4(const glm::mat4& mat4, const std::string& key) {
		int loc = glGetUniformLocation(shaderProgram, key.c_str());
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat4));
	}

	void Shader::SetShaderMat3(const glm::mat3& mat3, const std::string& key) {
		int loc = glGetUniformLocation(shaderProgram, key.c_str());
		glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(mat3));
	}

	void Shader::SetShaderVec3(const glm::vec3& vec3, const std::string& key) {
		int loc = glGetUniformLocation(shaderProgram, key.c_str());
		glUniform3fv(loc, 1, glm::value_ptr(vec3));
	}

	void Shader::SetShaderFloat(float value, const std::string& key) {
		int loc = glGetUniformLocation(shaderProgram, key.c_str());
		glUniform1f(loc, value);
	}

	void Shader::SetShaderInt(int value, const std::string& key) {
		int loc = glGetUniformLocation(shaderProgram, key.c_str());
		glUniform1i(loc, value);
	}

	/// <summary>
	/// ����shader�еĲ�������
	/// </summary>
	/// <param name="value"></param>
	/// <param name="key"></param>
	void Shader::SetShaderBoolean(bool value, const std::string& key) {
		int loc = glGetUniformLocation(shaderProgram, key.c_str());
		glUniform1i(loc, value ? GL_TRUE : GL_FALSE);
	}

	/// <summary>
    /// ������������ɫ������
    /// </summary>
    /// <param name="type"></param>
    /// <param name="strShader"></param>
    /// <returns></returns>
    GLuint Shader::CreateShader(GLuint type, const std::string& strShader) {
		GLuint shader;  
		shader = glCreateShader(type); //����һ����ɫ������
		const char* source = strShader.c_str(); //shader����
		glShaderSource(shader, 1, &source, NULL);//����shader������б���
		glCompileShader(shader);
		int success;
		std::string infoLog;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {//����ʧ��
			GLint infoLogLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
			std::vector<char> infoLog(infoLogLength);
			glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog.data());
			std::cerr << "[Error]{Shader::CreateShader} " << infoLog.data() << std::endl;
			return NULL;//����ʧ�ܷ��ؿ�ָ�� 
		}
		return shader;
    }
}