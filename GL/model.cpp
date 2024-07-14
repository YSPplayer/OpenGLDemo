#include "model.h"
#include <iostream>
#include "../Util/util.h"
namespace GL {
	using namespace Tool;
	Model::Model() {
		copy = false;
		vertices = nullptr;
		indices = nullptr;
		pshader = nullptr;
		eboMode = false;
		verticesSize = 0;
		indicesSize = 0;
		VBO = NULL;
		VAO = NULL;
		EBO = NULL;
		position = glm::mat4(1.0f);
	}

	Model::~Model() {
		if (copy) return;//拷贝的模型不做内存的释放，只释放唯一的那一个
		if(EBO) glDeleteBuffers(1, &EBO);
		if(VBO) glDeleteBuffers(1, &VBO);
	 	if(VAO) glDeleteVertexArrays(1,&VAO);
		Util::ReleasePointer(vertices, true);
		Util::ReleasePointer(indices, true);
		Util::ReleasePointer(pshader);
	}

	/// <summary>
	/// 创建模型
	/// </summary>
	/// <param name="vertices"></param>
	/// <param name="vsize"></param>
	/// <param name="indices"></param>
	/// <param name="isize"></param>
	/// <returns></returns>
	bool Model::CreateModel(const std::string& vertexShader, const std::string& colorShader,float vertices[], int vsize, unsigned int indices[], int isize) {
		eboMode = indices != nullptr;
		verticesSize = vsize;
		this->vertices = new float[verticesSize];
		std::memcpy(this->vertices, vertices, sizeof(float) * verticesSize);  //拷贝数据的内存到指针
		if (eboMode) {
			indicesSize = isize;
			this->indices = new unsigned int[indicesSize];
			std::memcpy(this->indices, indices, sizeof(unsigned int) * indicesSize);
		}
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);// 绑定VAO
		glGenBuffers(1, &VBO);//new一个顶点缓冲对象，存在VBO中
		glBindBuffer(GL_ARRAY_BUFFER, VBO); //设置当前VBO上下文
		/*
		  3个参数用于提升显卡渲染效率：
		  GL_STATIC_DRAW 数据不变
		  GL_DYNAMIC_DRAW 数据会改变很多
		  GL_STREAM_DRAW 数据每次绘制都会改变
		*/
		glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(vertices), vertices, GL_STATIC_DRAW);
		/*
			-1.设置顶点属性指针
			0.顶点数据首个元素的索引
			1.多少个数据组成一个vec对象
			2.vec对象数据类型
			3.是否把数据归一化(0-1)中?
			4.一个vec对象的大小
			5.数据在缓冲中起始位置的偏移量
		*/
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);//启动顶点属性，默认索引为0的地方开始启用
		if (eboMode) {
			glGenBuffers(1, &EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(indices), indices, GL_STATIC_DRAW);
		}
		position = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //默认模型为躺下45度的形式
		pshader = new Shader;
		return pshader->CreateShader(vertexShader,colorShader);
	}

	Model* Model::CopyModel(bool& success) {
		Model* cmodel = new Model;
		if (!VAO && !VBO) {
			success = false;
			return cmodel;
		}
		cmodel->copy = true;
		cmodel->VAO = VAO;
		cmodel->VBO = VBO;
		cmodel->eboMode = eboMode;
		cmodel->EBO = EBO;
		cmodel->vertices = vertices;
		cmodel->indices = indices;
		cmodel->verticesSize = verticesSize;
		cmodel->indicesSize = indicesSize;
		cmodel->position = position;
		success = true;
		return cmodel;
	}


	/// <summary>
	/// 更新模型的位置
	/// </summary>
	/// <param name="data"></param>
	/// <returns></returns>
	glm::mat4 Model::UpdatePoisition(const Data& data) {
		if ((!data.rotateX && !data.rotateZ)) return position;//(!rotateX && !rotateZ) || 
		position = glm::mat4(1.0f);
		if (data.rotateX) {
			float x = data.enable ? data.rotationX + data.lastRotationX : data.lastRotationZ;
			position = glm::rotate(position, glm::radians(Util::NormalizeAngle(x, 360.0f)), glm::vec3(1.0f, 0.0f, 0.0f)); //先进行X轴的旋转
		}
		else {
			position = glm::rotate(position, glm::radians(Util::NormalizeAngle(data.lastRotationX, 360.0f)), glm::vec3(1.0f, 0.0f, 0.0f)); //先进行X轴的旋转
		}
		if (data.rotateZ) {
			float z = data.enable ? data.rotationZ + data.lastRotationZ : data.lastRotationZ;
			position = glm::rotate(position, glm::radians(Util::NormalizeAngle(z, 360.0f)), glm::vec3(0.0f, 0.0f, 1.0f));//再沿着Z轴旋转，此时可以实现围绕自己自动旋转的效果
		}
		else {
			position = glm::rotate(position, glm::radians(Util::NormalizeAngle(data.lastRotationZ, 360.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
		}
		return position;
	}

	/// <summary>
	/// 渲染绘制
	/// </summary>
	void Model::Render() {
		glBindVertexArray(VAO);
		//参数三是需要绘制的顶点个数，单个三角形就是3个顶点，注意矩形是6个顶点而不是4个，就这么规定的，它的大小就是indices的长度
		eboMode ? glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0) : glDrawArrays(GL_TRIANGLES, 0, verticesSize);
	}
}