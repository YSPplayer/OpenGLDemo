#pragma once
#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
#include "data.h"
namespace GL {
	class Model {
	public:
		Model();
		~Model();
		bool CreateModel(const std::string& vertexShader, const std::string& colorShader,float vertices[], int vsize, unsigned int indices[] = nullptr, int isize = 0);
		Model* CopyModel(bool& success);
		glm::mat4 UpdatePoisition(const Data& data);
		void Render();
		inline Shader* GetShader() {
			return pshader;
		}
	private:
		bool copy;//��⵱ǰģ���Ƿ��ǿ���������
		float* vertices;//��Ż���ͼ��Ķ�������
		unsigned int* indices;//��Ŷ������ݵ�����λ����
		int verticesSize;//�������ݵĴ�С
		int indicesSize;//�������ݵĴ�С
		bool eboMode;
		GLuint VBO; //�����㻺�������Ҫ�洢�������ݣ�Ȼ���͸�gpu��Ⱦ��
		GLuint VAO;//��VBO����ͳһ����Ķ��󣬷����������
		GLuint EBO;//�������������󣬻��ƾ���ʱʹ�ã���������Ч�ʡ�
		glm::mat4 position;//Ĭ�ϵ�ģ�;������ڿ���ģ�͵�λ�úͷ���
		Shader* pshader;//��ɫ���������
	};
}