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
		normals = nullptr;
		tangents = nullptr;
		bitangents = nullptr;
		eboMode = false;
		hasTexture = false;
		hasSpecularTexture = false;
		verticesSize = 0;
		indicesSize = 0;
		normalSize = 0;
		PVBOS = new std::vector<GLuint>(VBO_MAX,NULL);
		VAO = NULL;
		EBO = NULL;
		TEXTURE = NULL;
		SPECULAR_TEXTURE = NULL;
		NORMALS_TEXTURE = NULL;
		position = glm::mat4(1.0f);
		centerPosition = glm::vec3(0.0f,0.0f,0.0f);//Ĭ��ģ�͵�����λ��Ϊ0,0
		material.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
		material.diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
		material.specular = glm::vec3(0.0f, 0.0f, 0.0f);
		material.shininess = 0.0f;
		Util::LoadMaterial(material,L"default.material");//����ģ�͵�Ĭ�ϲ���
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
	bool Model::CreateModel(const std::string& vertexShader, const std::string& colorShader,bool copy, float vertices[], int vsize, unsigned int indices[], int isize, unsigned int width, unsigned int height){
	/*	eboMode = (indices != nullptr);*/
		this->width = width;
		this->height = height;
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
		float* verticesBuffer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, verticesSize * sizeof(float), GL_MAP_WRITE_BIT);//| GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
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
			// gpu����ָ��־�ӳ��cpu������| GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
			unsigned int* indicesBuffer = (unsigned int*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, indicesSize * sizeof(unsigned int), GL_MAP_WRITE_BIT );
			if (!indicesBuffer) return false;
			std::memcpy(indicesBuffer, this->indices, sizeof(unsigned int) * indicesSize);//�����ڴ浽gpu
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);//���������Ҫcpu�ϵ�gpuָ�룬��ȡ��ӳ��
		}
		//�ֿ�������ϴ���gpu  55
		position = glm::rotate(glm::mat4(1.0f), glm::radians(DEFAULT_MODEL_X_RADIANS), glm::vec3(1.0f, 0.0f, 0.0f)); //Ĭ��ģ��Ϊ����45�ȵ���ʽ
		pshader = new Shader;
		return pshader->CreateShader(vertexShader,colorShader);
	}

	/// <summary>
	/// ����ģ�͵���ͼ
	/// </summary>
	/// <returns></returns>
	bool Model::SetTexture(unsigned char* texture, unsigned char* specularTexture,int width, int height, int nrChannels, float datas[], int size, bool gammaCorrection) {
		if (datas != nullptr) {
			this->uvs = datas;
			this->uvsSize = size; 
			glBindVertexArray(VAO);
			glGenBuffers(1, &PVBOS->at(VBO_TEXTURE));
			glBindBuffer(GL_ARRAY_BUFFER, PVBOS->at(VBO_TEXTURE));
			glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), nullptr, GL_STATIC_DRAW);//�󶨿�ָ��| GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
			float* textureBuffer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, size * sizeof(float), GL_MAP_WRITE_BIT );
			if (!textureBuffer) return false;
			std::memcpy(textureBuffer, datas, sizeof(float) * size);//�����ڴ浽gpu
			glUnmapBuffer(GL_ARRAY_BUFFER);
			//��Ϊ��������ĵ�λ������2.����ָ��2
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			for (int i = 0; i < 2; ++i) {
				//��������
				glGenTextures(1, i == 0 ? &TEXTURE : &SPECULAR_TEXTURE);
				i == 0 ? glActiveTexture(GL_TEXTURE0) : glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, i == 0 ? TEXTURE : SPECULAR_TEXTURE);
				//����������ķ�Χ����0-1��������Χ���ֲ�
				float borderColor[] = { 255.0f, 255.0f, 255.0f, 1.0f };
				glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
				/*
					�����������˷�ʽ����ģ�ͷŴ����Сʱ�����ı仯��ʽ������ʹ��Զ����ֵ��
				*/
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
		}
		else {
			//����ǰ��vbo�������������ͼ��
			if (texture) {
				hasTexture = true;
				/*�����2��������ͼ��opengl�Ḳ�ǵ�����ͼ�����ݣ����ǲ����ͷ��ڴ棬�����һ�ε�
				���õ���ͼ�ڴ�ȵڶ��δ�ʵ�ʵ����������ڴ滹�Ǻ͵�һ�ε�һ����ֻ���²��ֵĶ����ڴ汻
				���ǵ���*/
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, TEXTURE);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				//GPUҪ��ͼƬ���ȵĴ�Сһ����4�ı�������Ҫǰ�����ţ�����תΪRGBA����ΪRGBAͼƬһ����4�ı���
				//ʹ���ڴ����ķ�ʽ���ϴ�ͼƬ���ݵ�gpu�������ʧһ����Ч��
				glTexImage2D(GL_TEXTURE_2D, 0, gammaCorrection ? GL_SRGB : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			if (specularTexture) {
				hasSpecularTexture = true;
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, SPECULAR_TEXTURE);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glTexImage2D(GL_TEXTURE_2D, 0, gammaCorrection ? GL_SRGB : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, specularTexture);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
		}
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
		cmodel->hasTexture = hasTexture;
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
	glm::mat4 Model::UpdatePoisition(Data& data) {
		if ((!data.rotateX && !data.rotateZ)) return position;//(!rotateX && !rotateZ) || 
		// �Ƚ�ģ��ƽ�Ƶ�ָ�����ģ���ʹ��ģ��ʼ��Χ��������������ת
		glm::mat4 translationToCenter = glm::translate(glm::mat4(1.0f), centerPosition);
		position = translationToCenter;
		if (data.rotateX) {
			float x = data.enable ? data.rotationX + data.lastRotationX : data.lastRotationX;//lastRotationX
			float rotationAngle = Util::NormalizeAngle(x, 360.0f);
			// ������ת�Ƕ�
			 // ������180�ȵĽǶ�ת����������Χ��
			//if (rotationAngle > 180.0f) {
			//	rotationAngle -= 360.0f;
			//}
			//if (rotationAngle > 90.0f) {
			//	rotationAngle = 90.0f;
			//}
			//else if (rotationAngle < 0.0f) {
			//	rotationAngle = 0.0f;
			//}
			//data.lastRotationX = rotationAngle;
			position = glm::rotate(position, glm::radians(rotationAngle), glm::vec3(1.0f, 0.0f, 0.0f)); //�Ƚ���X�����ת
		}
		else {
			float rotationAngle = Util::NormalizeAngle(data.lastRotationX, 360.0f);
	/*		if (rotationAngle > 180.0f) {
				rotationAngle -= 360.0f;
			}
			if (rotationAngle > 90.0f) {
				rotationAngle = 90.0f;
			}
			else if (rotationAngle < 0.0f) {
				rotationAngle = 0.0f;
			}
			data.lastRotationX = rotationAngle;*/
			position = glm::rotate(position, glm::radians(rotationAngle), glm::vec3(1.0f, 0.0f, 0.0f)); //�Ƚ���X�����ת
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
		if (data.drawMode == DRAW_MODE_POINT) { //���Ƶ���
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawElements(GL_POINTS, indicesSize, GL_UNSIGNED_INT, 0);
		}
		else if (data.drawMode == DRAW_MODE_GRID) { //��������
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //������
			glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
		}
		
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
		position = glm::rotate(glm::mat4(1.0f), glm::radians(DEFAULT_MODEL_X_RADIANS), glm::vec3(1.0f, 0.0f, 0.0f)); //Ĭ��ģ��Ϊ����45�ȵ���ʽ
		return position;
	}

	/// <summary>
	/// ���㵱ǰģ�͵Ķ��㷨��
	/// </summary>
	bool Model::CalculateVertexNormals() {
		//���߼�����Ҫ�������ݺ�������������ݣ�û��ֱ�ӷ���
		if (!PVBOS->at(VBO_VERTEX) || !EBO) return false;
		normalSize = verticesSize;//�����������Ͷ�����������ͬ
		normals = new float[normalSize];
		int normalsv3Size = normalSize / 3;
		glm::vec3* normalsv3 = new glm::vec3[normalsv3Size];
		for (int i = 0; i < normalsv3Size; ++i) {
			normalsv3[i] = glm::vec3(0.0f, 0.0f, 0.0f); //��ʼ��ֵ
		}
		for (int i = 0; i < indicesSize; i += 3) {
			//���������һ���棬һ������Ҫ�������꣬������Ҫ9����������
			int index = i;
			const glm::vec3& v1 = glm::vec3(vertices[indices[index] * 3 + 0], vertices[indices[index] * 3 + 1], vertices[indices[index] * 3 + 2]);
			++index;
			const glm::vec3& v2 = glm::vec3(vertices[indices[index] * 3 + 0], vertices[indices[index] * 3 + 1], vertices[indices[index] * 3 + 2]);
			++index;
			const glm::vec3& v3 = glm::vec3(vertices[indices[index] * 3 + 0], vertices[indices[index] * 3 + 1], vertices[indices[index] * 3 + 2]);
			glm::vec3 normal = Util::CalculateFaceNormal(v1, v2, v3);
			normalsv3[indices[i]] += normal;
			normalsv3[indices[i + 1]] += normal;
			normalsv3[indices[i + 2]] += normal;
		}
		//�洢���ߵ�ʵ������
		for (int i = 0; i < normalsv3Size; ++i) {
			const glm::vec3& normal = glm::normalize(normalsv3[i]);
			normals[i * 3 + 0] = normal.x;
			normals[i * 3 + 1] = normal.y;
			normals[i * 3 + 2] = normal.z;
		/*	Util::WriteLog(L"========");
			Util::WriteLog(L"vec3_normal"+ std::to_wstring(i) + L":" + std::to_wstring(normal.x) + L"," + std::to_wstring(normal.y) + L"," + std::to_wstring(normal.z));*/
		}
		delete[] normalsv3;
		//�󶨷���VBO
		glBindVertexArray(VAO);
		glGenBuffers(1, &PVBOS->at(VBO_NORMAL));
		glBindBuffer(GL_ARRAY_BUFFER, PVBOS->at(VBO_NORMAL));
		glBufferData(GL_ARRAY_BUFFER, normalSize * sizeof(float), nullptr, GL_STATIC_DRAW);
		float* normalBuffer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, normalSize * sizeof(float), GL_MAP_WRITE_BIT);//| GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
		if (!normalBuffer) return false;
		std::memcpy(normalBuffer, this->normals, sizeof(float) * normalSize);
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);
		return true;
	}

	/// <summary>
	/// ���㷨����ͼ����صĹ���
	/// </summary>
	bool Model::CalculateNormalsTexture() {
		
		if (width != 0 && height != 0) { //���·�����ͼ
			auto normalMap = Util::ConvertNormalsToNormalMap(normals, width - 1, height - 1);
			normalMap.convertTo(normalMap, CV_8UC3, 255.0);
			cv::cvtColor(normalMap, normalMap, cv::COLOR_BGR2RGB);
			unsigned char* data = new unsigned char[normalMap.total() * normalMap.elemSize()];
			std::memcpy(data, normalMap.data, normalMap.total() * normalMap.elemSize());
			/*auto normalMap = cv::imread("E:\\open3d\\OpenGLDemo\\x64\\Debug\\Data\\a.jpg", cv::ImreadModes::IMREAD_UNCHANGED);
			cv::cvtColor(normalMap, normalMap, cv::COLOR_BGR2RGB);
			cv::flip(normalMap, normalMap, 0);
			if (!normalMap.isContinuous())normalMap = normalMap.clone();*/
			/*unsigned char* data = new unsigned char[normalMap.total() * normalMap.elemSize()];
			std::memcpy(data, normalMap.data, normalMap.total() * normalMap.elemSize());*/
			glGenTextures(1, &NORMALS_TEXTURE);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, NORMALS_TEXTURE);
			float borderColor[] = { 255.0f, 255.0f, 255.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			//������ͼ��Ҫԭʼ����ֵ������Ҫ��٤��У��
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, normalMap.cols, normalMap.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			this->tangents = new float[verticesSize];
			this->bitangents = new float[verticesSize];
			//�������ߺ͸�����
			Util::CalculateTangentAndBitangent(vertices,uvs,normals,verticesSize,tangents,bitangents);
			glBindVertexArray(VAO);
			//�����ߺ͸�����VBO����С����vertices����Ĵ�С
			glGenBuffers(1, &PVBOS->at(VBO_TANGENTS));
			glBindBuffer(GL_ARRAY_BUFFER, PVBOS->at(VBO_TANGENTS));
			glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(float), nullptr, GL_STATIC_DRAW);
			float* tangentsBuffer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, verticesSize * sizeof(float), GL_MAP_WRITE_BIT);
			if (!tangentsBuffer) return false;
			std::memcpy(tangentsBuffer, this->tangents, sizeof(float) * verticesSize);
			glUnmapBuffer(GL_ARRAY_BUFFER);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(3);

			glGenBuffers(1, &PVBOS->at(VBO_BITANGENTS));
			glBindBuffer(GL_ARRAY_BUFFER, PVBOS->at(VBO_BITANGENTS));
			glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(float), nullptr, GL_STATIC_DRAW);
			float* bitangentsBuffer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, verticesSize * sizeof(float), GL_MAP_WRITE_BIT);
			if (!bitangentsBuffer) return false;
			std::memcpy(bitangentsBuffer, this->bitangents, sizeof(float) * verticesSize);
			glUnmapBuffer(GL_ARRAY_BUFFER);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(4);
			//ֵ����
			// ��������ͼת��Ϊ8λͼ�񲢱���
			normalMap.convertTo(normalMap, CV_8UC3, 255.0);
			cv::cvtColor(normalMap, normalMap, cv::COLOR_BGR2RGB);
			cv::imwrite("C:\\Users\\User\\Desktop\\Atest\\fast.png", normalMap);
			return true;
			
		}
	}

}