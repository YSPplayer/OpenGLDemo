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
		PVBOS = new std::vector<GLuint>(VBO_MAX,NULL);
		VAO = NULL;
		EBO = NULL;
		TEXTURE = NULL;
		position = glm::mat4(1.0f);
		centerPosition = glm::vec3(0.0f,0.0f,0.0f);//Ĭ��ģ�͵�����λ��Ϊ0,0
	}

	Model::~Model() {
		if (copy) return;//������ģ�Ͳ����ڴ���ͷţ�ֻ�ͷ�Ψһ����һ��
		if(EBO) glDeleteBuffers(1, &EBO);
		if (TEXTURE) glDeleteTextures(1, &TEXTURE);
		if (PVBOS) {
			for (int i = 0; i < PVBOS->size(); ++i) 
				glDeleteBuffers(1, &PVBOS->at(i));
			Util::ReleasePointer(PVBOS);
		}
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
	bool Model::CreateModel(const std::string& vertexShader, const std::string& colorShader,bool copy, float vertices[], int vsize, unsigned int indices[], int isize) {
	/*	eboMode = (indices != nullptr);*/
		eboMode = true;
		verticesSize = vsize;
		if (copy) { //�����ջ����������Ҫ�����ڴ�
			this->vertices = new float[verticesSize];
			std::memcpy(this->vertices, vertices, sizeof(float) * verticesSize);  //�������ݵ��ڴ浽ָ��
			if (eboMode) {
				indicesSize = isize;
				this->indices = new unsigned int[indicesSize];
				std::memcpy(this->indices, indices, sizeof(unsigned int) * indicesSize);
			}
		}
		else { //����Ѿ������˶����ڴ棬ֱ�Ӹ�ֵ����
			this->vertices = vertices;
			if (eboMode) {
				indicesSize = isize;
				this->indices = indices;
			}
		}
		
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);// ��VAO
		glGenBuffers(1, &PVBOS->at(VBO_VERTEX));//newһ�����㻺����󣬴���VBO��
		glBindBuffer(GL_ARRAY_BUFFER, PVBOS->at(VBO_VERTEX)); //���õ�ǰVBO������
		/*
		* ����������ݹ��࣬��Ǩ�Ƶ�ѭ���зֿ鴦��
		  3���������������Կ���ȾЧ�ʣ�
		  GL_STATIC_DRAW ���ݲ���
		  GL_DYNAMIC_DRAW ���ݻ�ı�ܶ�
		  GL_STREAM_DRAW ����ÿ�λ��ƶ���ı�
		*/
		//glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(vertices), vertices, GL_STATIC_DRAW);
		
		glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(float), nullptr, GL_STATIC_DRAW);//�Ȱ󶨿�ָ�룬ֱ�Ӱ󶨴����Դ����������
		// gpu����ָ��־�ӳ��cpu������
		float* verticesBuffer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, verticesSize * sizeof(float), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		if (!verticesBuffer) return false;
		std::memcpy(verticesBuffer, this->vertices, sizeof(float) * verticesSize);//�����ڴ浽gpu
		glUnmapBuffer(GL_ARRAY_BUFFER);//����ȡ��ӳ��
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
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);
			// gpu����ָ��־�ӳ��cpu������
			unsigned int* indicesBuffer = (unsigned int*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, indicesSize * sizeof(unsigned int), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
			if (!indicesBuffer) return false;
			std::memcpy(indicesBuffer, this->indices, sizeof(unsigned int) * indicesSize);//�����ڴ浽gpu
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);//���������Ҫcpu�ϵ�gpuָ�룬��ȡ��ӳ��
		}
		//�ֿ�������ϴ���gpu 
		position = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //Ĭ��ģ��Ϊ����45�ȵ���ʽ
		pshader = new Shader;
		return pshader->CreateShader(vertexShader,colorShader);
	}

	/// <summary>
	/// ����ģ�͵���ͼ
	/// </summary>
	/// <returns></returns>
	bool Model::SetTexture(unsigned char* texture, int width, int height, int nrChannels, float datas[], int size) {
		glBindVertexArray(VAO);
		glGenBuffers(1, &PVBOS->at(VBO_TEXTURE));
		glBindBuffer(GL_ARRAY_BUFFER, PVBOS->at(VBO_TEXTURE));
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), nullptr, GL_STATIC_DRAW);//�󶨿�ָ��
		float* textureBuffer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, size * sizeof(float), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		if (!textureBuffer) return false;
		std::memcpy(textureBuffer, datas, sizeof(float) * size);//�����ڴ浽gpu
		glUnmapBuffer(GL_ARRAY_BUFFER);
		//��Ϊ�������ĵ�λ������2.����ָ��2
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		//��������
		glGenTextures(1, &TEXTURE);
		glBindTexture(GL_TEXTURE_2D, TEXTURE); 
		//����������ķ�Χ����0-1��������Χ���ֲ�
		float borderColor[] = { 255.0f, 255.0f, 255.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		/*
			����������˷�ʽ����ģ�ͷŴ����Сʱ����ı仯��ʽ������ʹ��Զ����ֵ��
		*/
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//����ǰ��vbo������������ͼ��
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
		glGenerateMipmap(GL_TEXTURE_2D);
		return true;
	}

	Model* Model::CopyModel(bool& success) {
		Model* cmodel = new Model;
		if (!VAO && (!PVBOS || !PVBOS->at(VBO_VERTEX))) {
			success = false;
			return cmodel;
		}
		cmodel->copy = true;
		cmodel->VAO = VAO;
		cmodel->TEXTURE = TEXTURE;
		cmodel->PVBOS = PVBOS;
		cmodel->eboMode = eboMode;
		cmodel->EBO = EBO;
		cmodel->vertices = vertices;
		cmodel->indices = indices;
		cmodel->verticesSize = verticesSize;
		cmodel->indicesSize = indicesSize;
		cmodel->pshader = pshader;
		cmodel->position = position;
		cmodel->centerPosition = centerPosition;
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
		// �Ƚ�ģ��ƽ�Ƶ�ָ�����ģ���ʹ��ģ��ʼ��Χ�������������ת
		glm::mat4 translationToCenter = glm::translate(glm::mat4(1.0f), centerPosition);
		position = translationToCenter;
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
		// �ٽ�ģ��ƽ�ƻ�ԭ����λ��
		glm::mat4 translationBack = glm::translate(glm::mat4(1.0f), glm::vec3(-centerPosition.x, -centerPosition.y, 0.0f));
		position = position * translationBack;
		return position;
	}

	/// <summary>
	/// ��Ⱦ����
	/// </summary>
	void Model::Render(const Data& data) {
		glBindVertexArray(VAO);
		data.drawLine ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
		//int indicesCount = indicesSize / 3;
		//// ��ÿһ�鶥�����ݣ�����ϴ��������ݲ�����
		//for (int i = 0; i < indicesCount; i += PLANE_BLOCK_SIZE) {
		//	int currentBlockSize = std::min(PLANE_BLOCK_SIZE, indicesCount - i);
		//	int indicesIndex = i * 3;
		//	/*
		//	0 ,1 ,2
		//	1,2,0,
		//	0,1,0,
		//	1,1,0
		//	*/
		//	// ����һ����ʱ����洢��Ҫ�Ķ������ݣ�һ����������[0,1,2]����*3�����Ķ�������
		//	float* tempVertices = new float[currentBlockSize * 3 * 3];
		//	int* tempIndices = new int[currentBlockSize * 3];
		//	int tempIndex = 0;
		//	for (int j = 0; j < currentBlockSize * 3; ++j) {//����һ��ֻ��ȡ����������һ��λ�õ�ֵ��һ��λ�õ�ֵ��Ӧ�Ķ���������3��
		//		int index = indices[indicesIndex++];
		//		tempVertices[tempIndex++] = vertices[index * 3 + 0];
		//		tempVertices[tempIndex++] = vertices[index * 3 + 1];
		//		tempVertices[tempIndex++] = vertices[index * 3 + 2];
		//		tempIndices[j] = j;
		//	}
		//	//�ϴ���ʱ�������ݵ�VBO
		//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//	glBufferData(GL_ARRAY_BUFFER, currentBlockSize * 3 * 3 * sizeof(float), tempVertices, GL_DYNAMIC_DRAW);
		//	//�ٰ�EBO
		//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//	//*3����Ϊ����һ����������
		//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentBlockSize * 3 * sizeof(unsigned int), tempIndices, GL_DYNAMIC_DRAW);
		//	// ���Ƶ�ǰ��Ķ���
		//	glDrawElements(GL_TRIANGLES, currentBlockSize * 3, GL_UNSIGNED_INT, 0);
		//}
	}

	/// <summary>
	/// ����ģ�͵���ʼ����
	/// </summary>
	glm::mat4 Model::ReSetPoisition() {
		position = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //Ĭ��ģ��Ϊ����45�ȵ���ʽ
		return position;
	} 
}