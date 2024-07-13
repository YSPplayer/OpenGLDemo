#include "glManager.h"
#include "shader.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace GL {
	GlManager::GlManager() {
		/*float vertices[] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f,  0.5f, 0.0f
		};*/
		verticesSize = 0;
		indicesSize = 0;
		eboMode = false;
		vertices = nullptr; 
		indices = nullptr;
		VBO = NULL;
		VAO = NULL;
		EBO = NULL;
		shaderProgram = NULL;
	}

	/// <summary>
	/// ��ʼ��
	/// </summary>
	/// <returns></returns>
	bool GlManager::InitGL() {
		if(vertices == nullptr) return false;
		return BindVAO() &&  (!eboMode || (eboMode & BindEBO())) && BindVBO()  && BindShader();
	}

	/// <summary>
	/// ������Ҫ���Ƶ�����
	/// </summary>
	/// <param name="vertices"></param>
	void GlManager::SetVerticeData(float vertices[],int size) {
		verticesSize = size;
		this->vertices = new float[verticesSize];
		std::memcpy(this->vertices, vertices, sizeof(float) * verticesSize);  //�������ݵ��ڴ浽ָ��
	}

	/// <summary>
	/// ����EBO������λ���ݣ�����������Զ�����EBO��Ⱦģʽ
	/// </summary>
	/// <param name="indices"></param>
	void GlManager::SetIndicesData(unsigned int indices[],int size) {
		eboMode = true;
		indicesSize = size;
		this->indices = new unsigned int[indicesSize];
		std::memcpy(this->indices, indices, sizeof(unsigned int) * indicesSize);
	}

	/// <summary>
	/// ��VAO
	/// </summary>
	/// <returns></returns>
	bool GlManager::BindVAO()
	{
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);// ��VAO
		return true;
	}

	/// <summary>
	/// ��VBO
	/// </summary>
	bool GlManager::BindVBO() {
		glGenBuffers(1,&VBO);//newһ�����㻺����󣬴���VBO��
		//ʹ��glBufferData������ĵ�������������ڸ�VBO�����õ�ǰ�Ķ��㻺������������ΪVBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		/*
		  3���������������Կ���ȾЧ�ʣ�
		  GL_STATIC_DRAW ���ݲ��� 
		  GL_DYNAMIC_DRAW ���ݻ�ı�ܶ�
		  GL_STREAM_DRAW ����ÿ�λ��ƶ���ı�
		*/
		glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(vertices), vertices, GL_STATIC_DRAW);
		/*
			1.���ö�������ָ��
			2.���������׸�Ԫ�ص�����
			3.���ٸ��������һ��vec����
			4.vec������������
			5.�Ƿ�����ݹ�һ��(0-1)��?
			6.һ��vec����Ĵ�С
			7.�����ڻ�������ʼλ�õ�ƫ����
		*/
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);//�����������ԣ�Ĭ������Ϊ0�ĵط���ʼ����
		return true;
	}


	/// <summary>
	/// ��EBO
	/// </summary>
	/// <returns></returns>
	bool GlManager::BindEBO() {
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(indices), indices, GL_STATIC_DRAW);
		return true;
	}

	/// <summary>
	/// ��Ƭ����ɫ���Ͷ�����ɫ��
	/// </summary>
	bool GlManager::BindShader() {
		shaderProgram =  glCreateProgram();//��ɫ������������ڹ�������ɫ������
		GLuint vShader = CreateShader(GL_VERTEX_SHADER,vertexShader); //������ɫ��
		GLuint cShader = CreateShader(GL_FRAGMENT_SHADER,colorShader); //Ƭ����ɫ��
		if(vShader && cShader) {
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
			if(!success) {
				glGetShaderInfoLog(shaderProgram, 512, NULL, &infoLog[0]); //��ȡ����Ĵ�����Ϣ
				std::cout << "[Error]{GlManager::BindShader}" << infoLog << std::endl;
				return false;//����������ʧ�� 
			}
			return true;
		}
		return false;
	}

	/// <summary>
	/// ��������
	/// </summary>
	void GlManager::Draw(const Data& data) {
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glm::mat4 model = cmaera.UpdateModelPoisition(data.enable,data.rotateX, data.rotateZ,data.rotationX, data.rotationZ,
			data.lastRotationX,data.lastRotationZ);
		//// ע�⣬���ǽ�����������Ҫ�����ƶ������ķ������ƶ���
		//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); //����ӽ�Z�ᷴ�����ƶ�����
		glm::mat4 view = cmaera.UpdateCameraPoisition(data.moveType,data.moveSpeed,data.yaw,data.pitch,data.isYaw);
		glm::mat4 projection = glm::mat4(1.0f); //͸���ӿ�
		//glm::radians(45.0f)
		/*std::cout << "data.aspect" << data.aspect << std::endl;*/
		projection = glm::perspective(data.aspect, static_cast<float>(data.width) / static_cast<float>(data.height), 0.1f, 100.0f);
		int modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	/*	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/
		//����������Ҫ���ƵĶ�����������������ξ���3�����㣬ע�������6�����������4��������ô�涨�ģ����Ĵ�С����indices�ĳ���
		eboMode ? glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0) : glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(NULL);
	}

	/// <summary>
	/// ���벢����shader��ɫ������ָ��,���ڶ�����ɫ����Ƭ����ɫ��
	/// </summary>
	/// <param name="shader"></param>
	/// <returns></returns>
	GLuint GlManager::CreateShader(GLuint type,const std::string& strShader) {
		GLuint shader;
		shader = glCreateShader(type); //����һ����ɫ������
		const char* source = strShader.c_str(); //shader����
		glShaderSource(shader, 1, &source, NULL);//����shader������б���
		glCompileShader(shader);
		int success;
		std::string infoLog;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success) {//����ʧ��
			glGetShaderInfoLog(shader, 512, NULL, &infoLog[0]); //��ȡ����Ĵ�����Ϣ
			std::cout << "[Error]{GlManager::CreateShader}" << infoLog << std::endl;
			return NULL;//����ʧ�ܷ��ؿ�ָ�� 
		} 
		return shader;
	}
}