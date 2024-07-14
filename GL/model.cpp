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
		if (copy) return;//������ģ�Ͳ����ڴ���ͷţ�ֻ�ͷ�Ψһ����һ��
		if(EBO) glDeleteBuffers(1, &EBO);
		if(VBO) glDeleteBuffers(1, &VBO);
	 	if(VAO) glDeleteVertexArrays(1,&VAO);
		Util::ReleasePointer(vertices, true);
		Util::ReleasePointer(indices, true);
		Util::ReleasePointer(pshader);
	}

	/// <summary>
	/// ����ģ��
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
		std::memcpy(this->vertices, vertices, sizeof(float) * verticesSize);  //�������ݵ��ڴ浽ָ��
		if (eboMode) {
			indicesSize = isize;
			this->indices = new unsigned int[indicesSize];
			std::memcpy(this->indices, indices, sizeof(unsigned int) * indicesSize);
		}
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);// ��VAO
		glGenBuffers(1, &VBO);//newһ�����㻺����󣬴���VBO��
		glBindBuffer(GL_ARRAY_BUFFER, VBO); //���õ�ǰVBO������
		/*
		  3���������������Կ���ȾЧ�ʣ�
		  GL_STATIC_DRAW ���ݲ���
		  GL_DYNAMIC_DRAW ���ݻ�ı�ܶ�
		  GL_STREAM_DRAW ����ÿ�λ��ƶ���ı�
		*/
		glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(vertices), vertices, GL_STATIC_DRAW);
		/*
			-1.���ö�������ָ��
			0.���������׸�Ԫ�ص�����
			1.���ٸ��������һ��vec����
			2.vec������������
			3.�Ƿ�����ݹ�һ��(0-1)��?
			4.һ��vec����Ĵ�С
			5.�����ڻ�������ʼλ�õ�ƫ����
		*/
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);//�����������ԣ�Ĭ������Ϊ0�ĵط���ʼ����
		if (eboMode) {
			glGenBuffers(1, &EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(indices), indices, GL_STATIC_DRAW);
		}
		position = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //Ĭ��ģ��Ϊ����45�ȵ���ʽ
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
	/// ����ģ�͵�λ��
	/// </summary>
	/// <param name="data"></param>
	/// <returns></returns>
	glm::mat4 Model::UpdatePoisition(const Data& data) {
		if ((!data.rotateX && !data.rotateZ)) return position;//(!rotateX && !rotateZ) || 
		position = glm::mat4(1.0f);
		if (data.rotateX) {
			float x = data.enable ? data.rotationX + data.lastRotationX : data.lastRotationZ;
			position = glm::rotate(position, glm::radians(Util::NormalizeAngle(x, 360.0f)), glm::vec3(1.0f, 0.0f, 0.0f)); //�Ƚ���X�����ת
		}
		else {
			position = glm::rotate(position, glm::radians(Util::NormalizeAngle(data.lastRotationX, 360.0f)), glm::vec3(1.0f, 0.0f, 0.0f)); //�Ƚ���X�����ת
		}
		if (data.rotateZ) {
			float z = data.enable ? data.rotationZ + data.lastRotationZ : data.lastRotationZ;
			position = glm::rotate(position, glm::radians(Util::NormalizeAngle(z, 360.0f)), glm::vec3(0.0f, 0.0f, 1.0f));//������Z����ת����ʱ����ʵ��Χ���Լ��Զ���ת��Ч��
		}
		else {
			position = glm::rotate(position, glm::radians(Util::NormalizeAngle(data.lastRotationZ, 360.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
		}
		return position;
	}

	/// <summary>
	/// ��Ⱦ����
	/// </summary>
	void Model::Render() {
		glBindVertexArray(VAO);
		//����������Ҫ���ƵĶ�����������������ξ���3�����㣬ע�������6�����������4��������ô�涨�ģ����Ĵ�С����indices�ĳ���
		eboMode ? glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0) : glDrawArrays(GL_TRIANGLES, 0, verticesSize);
	}
}