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
	/// 初始化
	/// </summary>
	/// <returns></returns>
	bool GlManager::InitGL() {
		if(vertices == nullptr) return false;
		return BindVAO() &&  (!eboMode || (eboMode & BindEBO())) && BindVBO()  && BindShader();
	}

	/// <summary>
	/// 设置需要绘制的数据
	/// </summary>
	/// <param name="vertices"></param>
	void GlManager::SetVerticeData(float vertices[],int size) {
		verticesSize = size;
		this->vertices = new float[verticesSize];
		std::memcpy(this->vertices, vertices, sizeof(float) * verticesSize);  //拷贝数据的内存到指针
	}

	/// <summary>
	/// 设置EBO的索引位数据，如果设置则自动开启EBO渲染模式
	/// </summary>
	/// <param name="indices"></param>
	void GlManager::SetIndicesData(unsigned int indices[],int size) {
		eboMode = true;
		indicesSize = size;
		this->indices = new unsigned int[indicesSize];
		std::memcpy(this->indices, indices, sizeof(unsigned int) * indicesSize);
	}

	/// <summary>
	/// 绑定VAO
	/// </summary>
	/// <returns></returns>
	bool GlManager::BindVAO()
	{
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);// 绑定VAO
		return true;
	}

	/// <summary>
	/// 绑定VBO
	/// </summary>
	bool GlManager::BindVBO() {
		glGenBuffers(1,&VBO);//new一个顶点缓冲对象，存在VBO中
		//使得glBufferData函数绑的的数组对象作用于该VBO，设置当前的顶点缓冲对象的上下文为VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		/*
		  3个参数用于提升显卡渲染效率：
		  GL_STATIC_DRAW 数据不变 
		  GL_DYNAMIC_DRAW 数据会改变很多
		  GL_STREAM_DRAW 数据每次绘制都会改变
		*/
		glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(vertices), vertices, GL_STATIC_DRAW);
		/*
			1.设置顶点属性指针
			2.顶点数据首个元素的索引
			3.多少个数据组成一个vec对象
			4.vec对象数据类型
			5.是否把数据归一化(0-1)中?
			6.一个vec对象的大小
			7.数据在缓冲中起始位置的偏移量
		*/
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);//启动顶点属性，默认索引为0的地方开始启用
		return true;
	}


	/// <summary>
	/// 绑定EBO
	/// </summary>
	/// <returns></returns>
	bool GlManager::BindEBO() {
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(indices), indices, GL_STATIC_DRAW);
		return true;
	}

	/// <summary>
	/// 绑定片段着色器和顶点着色器
	/// </summary>
	bool GlManager::BindShader() {
		shaderProgram =  glCreateProgram();//着色器程序对象，用于管理多个着色器对象
		GLuint vShader = CreateShader(GL_VERTEX_SHADER,vertexShader); //顶点着色器
		GLuint cShader = CreateShader(GL_FRAGMENT_SHADER,colorShader); //片段着色器
		if(vShader && cShader) {
			glAttachShader(shaderProgram, vShader);
			glAttachShader(shaderProgram, cShader);
			glLinkProgram(shaderProgram);
			glValidateProgram(shaderProgram);
			//链接完成之后释放指针内存
			glDeleteShader(vShader);
			glDeleteShader(cShader);
			int success;
			std::string infoLog;
			glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
			if(!success) {
				glGetShaderInfoLog(shaderProgram, 512, NULL, &infoLog[0]); //获取编译的错误消息
				std::cout << "[Error]{GlManager::BindShader}" << infoLog << std::endl;
				return false;//链接器编译失败 
			}
			return true;
		}
		return false;
	}

	/// <summary>
	/// 绘制数据
	/// </summary>
	void GlManager::Draw(const Data& data) {
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glm::mat4 model = cmaera.UpdateModelPoisition(data.enable,data.rotateX, data.rotateZ,data.rotationX, data.rotationZ,
			data.lastRotationX,data.lastRotationZ);
		//// 注意，我们将矩阵向我们要进行移动场景的反方向移动。
		//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); //相机视角Z轴反方向移动距离
		glm::mat4 view = cmaera.UpdateCameraPoisition(data.moveType,data.moveSpeed,data.yaw,data.pitch,data.isYaw);
		glm::mat4 projection = glm::mat4(1.0f); //透视视口
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
		//参数三是需要绘制的顶点个数，单个三角形就是3个顶点，注意矩形是6个顶点而不是4个，就这么规定的，它的大小就是indices的长度
		eboMode ? glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0) : glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(NULL);
	}

	/// <summary>
	/// 编译并返回shader着色器对象指针,用于顶点着色器和片段着色器
	/// </summary>
	/// <param name="shader"></param>
	/// <returns></returns>
	GLuint GlManager::CreateShader(GLuint type,const std::string& strShader) {
		GLuint shader;
		shader = glCreateShader(type); //创建一个着色器对象
		const char* source = strShader.c_str(); //shader代码
		glShaderSource(shader, 1, &source, NULL);//传入shader代码进行编译
		glCompileShader(shader);
		int success;
		std::string infoLog;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success) {//编译失败
			glGetShaderInfoLog(shader, 512, NULL, &infoLog[0]); //获取编译的错误消息
			std::cout << "[Error]{GlManager::CreateShader}" << infoLog << std::endl;
			return NULL;//编译失败返回空指针 
		} 
		return shader;
	}
}