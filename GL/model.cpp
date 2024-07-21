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
		centerPosition = glm::vec3(0.0f,0.0f,0.0f);//默认模型的中心位置为0,0
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
	bool Model::CreateModel(const std::string& vertexShader, const std::string& colorShader,bool copy, float vertices[], int vsize, unsigned int indices[], int isize) {
	/*	eboMode = (indices != nullptr);*/
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
		float* verticesBuffer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, verticesSize * sizeof(float), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
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
			// gpu数据指针持久映射cpu缓冲区
			unsigned int* indicesBuffer = (unsigned int*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, indicesSize * sizeof(unsigned int), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
			if (!indicesBuffer) return false;
			std::memcpy(indicesBuffer, this->indices, sizeof(unsigned int) * indicesSize);//拷贝内存到gpu
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);//如果不再需要cpu上的gpu指针，就取消映射
		}
		//分块把数据上传到gpu 
		position = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //默认模型为躺下45度的形式
		pshader = new Shader;
		return pshader->CreateShader(vertexShader,colorShader);
	}

	/// <summary>
	/// 设置模型的贴图
	/// </summary>
	/// <returns></returns>
	bool Model::SetTexture(unsigned char* texture, int width, int height, int nrChannels, float datas[], int size) {
		glBindVertexArray(VAO);
		glGenBuffers(1, &PVBOS->at(VBO_TEXTURE));
		glBindBuffer(GL_ARRAY_BUFFER, PVBOS->at(VBO_TEXTURE));
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), nullptr, GL_STATIC_DRAW);//绑定空指针
		float* textureBuffer = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, size * sizeof(float), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		if (!textureBuffer) return false;
		std::memcpy(textureBuffer, datas, sizeof(float) * size);//拷贝内存到gpu
		glUnmapBuffer(GL_ARRAY_BUFFER);
		//因为纹理对象的单位长度是2.所以指定2
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		//作用纹理
		glGenTextures(1, &TEXTURE);
		glBindTexture(GL_TEXTURE_2D, TEXTURE); 
		//当纹理坐标的范围超出0-1，超出范围用弥补
		float borderColor[] = { 255.0f, 255.0f, 255.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		/*
			设置纹理过滤方式，当模型放大或缩小时纹理的变化方式，这里使用远近差值法
		*/
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//给当前的vbo纹理对象绑定纹理图象
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
	/// 更新模型的位置
	/// </summary>
	/// <param name="data"></param>
	/// <returns></returns>
	glm::mat4 Model::UpdatePoisition(const Data& data) {
		if ((!data.rotateX && !data.rotateZ)) return position;//(!rotateX && !rotateZ) || 
		// 先将模型平移到指定中心，以使得模型始终围绕自身的中心旋转
		glm::mat4 translationToCenter = glm::translate(glm::mat4(1.0f), centerPosition);
		position = translationToCenter;
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
		data.drawLine ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
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
		position = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //默认模型为躺下45度的形式
		return position;
	} 
}