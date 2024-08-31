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
		centerPosition = glm::vec3(0.0f,0.0f,0.0f);//默认模型的中心位置为0,0
		material.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
		material.diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
		material.specular = glm::vec3(0.0f, 0.0f, 0.0f);
		material.shininess = 0.0f;
		Util::LoadMaterial(material,L"default.material");//加载模型的默认材质
	}

	Model::~Model() {
		if (copy) return;//拷贝的模型不做内存的释放，只释放唯一的那一个
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
	/// 创建模型
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
		if (copy) { //如果在栈区创建，需要拷贝内存
			this->vertices = new float[verticesSize];
			std::memcpy(this->vertices, vertices, sizeof(float) * verticesSize);  //拷贝数据的内存到指针
			if (eboMode) {
				indicesSize = isize;
				this->indices = new unsigned int[indicesSize];
				std::memcpy(this->indices, indices, sizeof(unsigned int) * indicesSize);
			}
		}
		else { //如果已经创建了堆区内存，直接赋值即可
			this->vertices = vertices;
			if (eboMode) {
				indicesSize = isize;
				this->indices = indices;
			}
		}
		
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);// 绑定VAO
		glGenBuffers(1, &PVBOS->at(VBO_VERTEX));//new一个顶点缓冲对象，存在VBO中
		glBindBuffer(GL_ARRAY_BUFFER, PVBOS->at(VBO_VERTEX)); //设置当前VBO上下文
		/*
		* 如果顶点数据过多，则迁移到循环中分块处理
		  3个参数用于提升显卡渲染效率：
		  GL_STATIC_DRAW 数据不变
		  GL_DYNAMIC_DRAW 数据会改变很多
		  GL_STREAM_DRAW 数据每次绘制都会改变
		*/
		//glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(vertices), vertices, GL_STATIC_DRAW);
		
		glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(float), nullptr, GL_STATIC_DRAW);//先绑定空指针，直接绑定存在显存溢出的问题
		// gpu数据指针持久映射cpu缓冲区
		float* verticesBuffer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, verticesSize * sizeof(float), GL_MAP_WRITE_BIT);//| GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
		if (!verticesBuffer) return false;
		std::memcpy(verticesBuffer, this->vertices, sizeof(float) * verticesSize);//拷贝内存到gpu
		glUnmapBuffer(GL_ARRAY_BUFFER);//必须取消映射
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
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);
			// gpu数据指针持久映射cpu缓冲区| GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
			unsigned int* indicesBuffer = (unsigned int*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, indicesSize * sizeof(unsigned int), GL_MAP_WRITE_BIT );
			if (!indicesBuffer) return false;
			std::memcpy(indicesBuffer, this->indices, sizeof(unsigned int) * indicesSize);//拷贝内存到gpu
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);//如果不再需要cpu上的gpu指针，就取消映射
		}
		//分块把数据上传到gpu  55
		position = glm::rotate(glm::mat4(1.0f), glm::radians(DEFAULT_MODEL_X_RADIANS), glm::vec3(1.0f, 0.0f, 0.0f)); //默认模型为躺下45度的形式
		pshader = new Shader;
		return pshader->CreateShader(vertexShader,colorShader);
	}

	/// <summary>
	/// 设置模型的贴图
	/// </summary>
	/// <returns></returns>
	bool Model::SetTexture(unsigned char* texture, unsigned char* specularTexture,int width, int height, int nrChannels, float datas[], int size, bool gammaCorrection) {
		if (datas != nullptr) {
			this->uvs = datas;
			this->uvsSize = size; 
			glBindVertexArray(VAO);
			glGenBuffers(1, &PVBOS->at(VBO_TEXTURE));
			glBindBuffer(GL_ARRAY_BUFFER, PVBOS->at(VBO_TEXTURE));
			glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), nullptr, GL_STATIC_DRAW);//绑定空指针| GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
			float* textureBuffer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, size * sizeof(float), GL_MAP_WRITE_BIT );
			if (!textureBuffer) return false;
			std::memcpy(textureBuffer, datas, sizeof(float) * size);//拷贝内存到gpu
			glUnmapBuffer(GL_ARRAY_BUFFER);
			//因为纹理对象的单位长度是2.所以指定2
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			for (int i = 0; i < 2; ++i) {
				//作用纹理
				glGenTextures(1, i == 0 ? &TEXTURE : &SPECULAR_TEXTURE);
				i == 0 ? glActiveTexture(GL_TEXTURE0) : glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, i == 0 ? TEXTURE : SPECULAR_TEXTURE);
				//当纹理坐标的范围超出0-1，超出范围用弥补
				float borderColor[] = { 255.0f, 255.0f, 255.0f, 1.0f };
				glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
				/*
				设置纹理过滤方式，当模型放大或缩小时纹理的变化方式，这里使用远近差值法
				*/
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
		}
		else {
			//给当前的vbo纹理对象绑定纹理图象
			if (texture) {
				hasTexture = true;
				/*如果是2次设置贴图，opengl会覆盖掉旧贴图的数据，但是不会释放内存，如果第一次的
				作用的贴图内存比第二次大，实际的纹理对象内存还是和第一次的一样，只是新部分的对象内存被
				覆盖掉了*/
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, TEXTURE);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				//GPU要求图片宽度的大小一定是4的倍数，需要前置缩放，或者转为RGBA，因为RGBA图片一定是4的倍数
				//使用内存对齐的方式来上传图片数据到gpu，这会损失一定的效率
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
	/// 更新模型位置
	/// </summary>
	/// <param name="data"></param>
	/// <returns></returns>
	glm::mat4 Model::UpdatePoisition(Data& data) {
		if ((!data.rotateX && !data.rotateZ)) return position;
		// 先将模型平移到指定中心，以使得模型始终围绕自身的中心旋转
		glm::mat4 translationToCenter = glm::translate(glm::mat4(1.0f), centerPosition);
		position = translationToCenter;
		position = glm::rotate(position, glm::radians(data.lastRotationX), glm::vec3(1.0f, 0.0f, 0.0f));
		position = glm::rotate(position, glm::radians(data.lastRotationZ), glm::vec3(0.0f, 0.0f, 1.0f));
		// 再将模型平移回原来的位置
		glm::mat4 translationBack = glm::translate(glm::mat4(1.0f), glm::vec3(-centerPosition.x, -centerPosition.y, 0.0f));
		position = position * translationBack;
		return position;
	}
	                  
	/// <summary>
	/// 渲染绘制
	/// </summary>
	void Model::Render(const Data& data) {
		glBindVertexArray(VAO);
		if (data.drawMode == DRAW_MODE_POINT) { //绘制点云
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawElements(GL_POINTS, indicesSize, GL_UNSIGNED_INT, 0);
		}
		else if (data.drawMode == DRAW_MODE_GRID) { //绘制网格
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //绘制面
			glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
		}
		
		//int indicesCount = indicesSize / 3;
		//// 对每一块顶点数据，逐块上传索引数据并绘制
		//for (int i = 0; i < indicesCount; i += PLANE_BLOCK_SIZE) {
		//	int currentBlockSize = std::min(PLANE_BLOCK_SIZE, indicesCount - i);
		//	int indicesIndex = i * 3;
		//	/*
		//	0 ,1 ,2
		//	1,2,0,
		//	0,1,0,
		//	1,1,0
		//	*/
		//	// 创建一个临时数组存储需要的顶点数据，一个索引数组[0,1,2]代表*3倍数的顶点数据
		//	float* tempVertices = new float[currentBlockSize * 3 * 3];
		//	int* tempIndices = new int[currentBlockSize * 3];
		//	int tempIndex = 0;
		//	for (int j = 0; j < currentBlockSize * 3; ++j) {//我们一次只读取了索引数组一个位置的值，一个位置的值对应的顶点数据是3个
		//		int index = indices[indicesIndex++];
		//		tempVertices[tempIndex++] = vertices[index * 3 + 0];
		//		tempVertices[tempIndex++] = vertices[index * 3 + 1];
		//		tempVertices[tempIndex++] = vertices[index * 3 + 2];
		//		tempIndices[j] = j;
		//	}
		//	//上传临时顶点数据到VBO
		//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//	glBufferData(GL_ARRAY_BUFFER, currentBlockSize * 3 * 3 * sizeof(float), tempVertices, GL_DYNAMIC_DRAW);
		//	//再绑定EBO
		//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//	//*3是因为代表一组索引数组
		//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentBlockSize * 3 * sizeof(unsigned int), tempIndices, GL_DYNAMIC_DRAW);
		//	// 绘制当前块的顶点
		//	glDrawElements(GL_TRIANGLES, currentBlockSize * 3, GL_UNSIGNED_INT, 0);
		//}
	}

	/// <summary>
	/// 重置模型到初始坐标
	/// </summary>
	glm::mat4 Model::ReSetPoisition() {
		position = glm::rotate(glm::mat4(1.0f), glm::radians(DEFAULT_MODEL_X_RADIANS), glm::vec3(1.0f, 0.0f, 0.0f)); 
		return position;
	}

	/// <summary>
	/// 设置模型的colorMap
	/// </summary>
	/// <param name="type"></param>
	void Model::SetColorMap(MapColorType type) {
		glBindVertexArray(VAO);
		if (!PVBOS->at(VBO_MAPCOLOR)) {
			glGenBuffers(1, &PVBOS->at(VBO_MAPCOLOR));
		}
		//下次再次调用会替换原先的内存，不用重新创建一个vbo
		glBindBuffer(GL_ARRAY_BUFFER, PVBOS->at(VBO_MAPCOLOR));
		glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(float), nullptr, GL_STATIC_DRAW);
		float* colorMapsBuffer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, verticesSize * sizeof(float), GL_MAP_WRITE_BIT);
		std::memcpy(colorMapsBuffer, this->colorMaps[type], sizeof(float) * verticesSize);
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(5);
	}

	/// <summary>
	/// 计算当前模型的顶点法线
	/// </summary>
	bool Model::CalculateVertexNormals() {
		//法线计算需要顶点数据和索引数组的数据，没有直接返回
		if (!PVBOS->at(VBO_VERTEX) || !EBO) return false;
		normalSize = verticesSize;//法线数据量和顶点数据量相同
		normals = new float[normalSize];
		int normalsv3Size = normalSize / 3;
		glm::vec3* normalsv3 = new glm::vec3[normalsv3Size];
		for (int i = 0; i < normalsv3Size; ++i) {
			normalsv3[i] = glm::vec3(0.0f, 0.0f, 0.0f); //初始化值
		}
		for (int i = 0; i < indicesSize; i += 3) {
			//三个点组成一个面，一个点需要三个坐标，所以需要9个坐标数据
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
		//存储法线的实际数据
		for (int i = 0; i < normalsv3Size; ++i) {
			const glm::vec3& normal = glm::normalize(normalsv3[i]);
			normals[i * 3 + 0] = normal.x;
			normals[i * 3 + 1] = normal.y;
			normals[i * 3 + 2] = normal.z;
		/*	Util::WriteLog(L"========");
			Util::WriteLog(L"vec3_normal"+ std::to_wstring(i) + L":" + std::to_wstring(normal.x) + L"," + std::to_wstring(normal.y) + L"," + std::to_wstring(normal.z));*/
		}
		delete[] normalsv3;
		//绑定法线VBO
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
	/// 计算法线贴图的相关的工作
	/// </summary>
	bool Model::CalculateNormalsTexture() {
		
		if (width != 0 && height != 0) { //更新法线贴图
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
			//法线贴图需要原始数据值，不需要用伽马校正
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, normalMap.cols, normalMap.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			this->tangents = new float[verticesSize];
			this->bitangents = new float[verticesSize];
			//计算切线和副切线
			Util::CalculateTangentAndBitangent(vertices,uvs,normals,verticesSize,tangents,bitangents);
			glBindVertexArray(VAO);
			//绑定切线和副切线VBO，大小都是vertices数组的大小
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
			//值传入
			// 将法线贴图转换为8位图像并保存
			//normalMap.convertTo(normalMap, CV_8UC3, 255.0);
			//cv::cvtColor(normalMap, normalMap, cv::COLOR_BGR2RGB);
			//cv::imwrite("C:\\Users\\User\\Desktop\\Atest\\fast.png", normalMap);
			return true;
			
		}
	}

}