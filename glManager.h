#pragma once
#include <glad/glad.h> 
#include <string>
#include "Camera.h"
namespace GL {
	struct Data {
		//��Ļ����
		int width;
		int height;
		int moveType;//�ƶ�ģʽ
		float moveSpeed;//�ƶ��ٶ�
		float rotationZ;
		float lastRotationZ;
		bool rotateZ;
		float rotationX;
		float lastRotationX;
		bool enable;
		bool rotateX;
		float aspect;//�����Ұ��С
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
		float* vertices;//��Ż���ͼ��Ķ�������
		unsigned int* indices;//��Ŷ������ݵ�����λ����
		bool eboMode;
		int verticesSize;//�������ݵĴ�С
		int indicesSize;//�������ݵĴ�С
		GLuint VBO; //�����㻺�������Ҫ�洢�������ݣ�Ȼ���͸�gpu��Ⱦ��
		GLuint VAO;//��VBO����ͳһ����Ķ��󣬷����������
		GLuint EBO;//�������������󣬻��ƾ���ʱʹ�ã���������Ч�ʡ�
		GLuint shaderProgram;//��ɫ������������
		GLuint CreateShader(GLuint type,const std::string& strShader);
	};
}