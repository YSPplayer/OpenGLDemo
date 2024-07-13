#pragma once
#include <glad/glad.h> 
#include <string>
#include "Camera.h"
namespace GL {
	struct Data {
		//屏幕长宽
		int width;
		int height;
		int moveType;//移动模式
		float moveSpeed;//移动速度
		float rotationZ;
		float lastRotationZ;
		bool rotateZ;
		float rotationX;
		float lastRotationX;
		bool enable;
		bool rotateX;
		float aspect;//相机视野大小
		float pitch;
		float yaw;
		bool isYaw;
	};
	class GlManager {
	public:
		void Draw(const Data& data);
		GlManager();
		~GlManager() {};
		bool InitGL();
		void SetVerticeData(float vertices[],int size);
		void SetIndicesData(unsigned int indices[],int size);
	private:
		Camera cmaera;
		bool BindVAO();
		bool BindVBO();
		bool BindEBO();
		bool BindShader();
		float* vertices;//存放绘制图像的顶点数据
		unsigned int* indices;//存放顶点数据的索引位数据
		bool eboMode;
		int verticesSize;//顶点数据的大小
		int indicesSize;//索引数据的大小
		GLuint VBO; //【顶点缓冲对象，主要存储顶点数据，然后发送给gpu渲染】
		GLuint VAO;//对VBO进行统一管理的对象，方便后续调用
		GLuint EBO;//【顶点索引对象，绘制矩形时使用，提升绘制效率】
		GLuint shaderProgram;//着色器程序管理对象
		GLuint CreateShader(GLuint type,const std::string& strShader);
	};
}