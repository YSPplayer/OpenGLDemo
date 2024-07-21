#pragma once
#include <vector>
#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
#include "data.h"
#include "common.h"
namespace GL {
	class Model {
	public:
		Model();
		~Model();
		bool CreateModel(const std::string& vertexShader, const std::string& colorShader, bool copy, float vertices[], int vsize, unsigned int indices[] = nullptr, int isize = 0);
		bool SetTexture(unsigned char* texture,int width,int height,int nrChannels,float datas[], int size);
		Model* CopyModel(bool& success);
		inline void SetModelCenterPoisition(const glm::vec3& centerPosition) {
			this->centerPosition = centerPosition;
		};
		glm::mat4 UpdatePoisition(const Data& data);
		void Render(const Data& data);
		inline Shader* GetShader() {
			return pshader;
		}
		glm::mat4 ReSetPoisition();
		GLuint TEXTURE;//贴图管理对象，不是贴图数据的顶点对象，顶点对象放在VBO管理
		inline bool HasTexture() {
			return hasTexture;
		};
	private:
		bool hasTexture;//检查 当前模型是否有贴图
		bool copy;//检测当前模型是否是拷贝出来的
		float* vertices;//存放绘制图像的顶点数据
		unsigned int* indices;//存放顶点数据的索引位数据
		int verticesSize;//顶点数据的大小
		int indicesSize;//索引数据的大小
		bool eboMode;
		std::vector<GLuint>* PVBOS; //【顶点缓冲对象，主要存储顶点数据，然后发送给gpu渲染】
		GLuint VAO;//对VBO进行统一管理的对象，方便后续调用
		GLuint EBO;//【顶点索引对象，绘制矩形时使用，提升绘制效率】
		glm::mat4 position;//默认的模型矩阵，用于控制模型的位置和方向
		Shader* pshader;//着色器管理对象
		glm::vec3 centerPosition;//模型的中心位置
	};
}