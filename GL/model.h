#pragma once
#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
#include "data.h"
namespace GL {
#define POINT_BLOCK_SIZE 16 //�������ݴ���ķֿ�����
#define PLANE_BLOCK_SIZE 50 //�����ݵ�λ�ϴ��Դ洦��ķֿ�����
	class Model {
	public:
		Model();
		~Model();
		bool CreateModel(const std::string& vertexShader, const std::string& colorShader, bool copy, float vertices[], int vsize, unsigned int indices[] = nullptr, int isize = 0);
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
		glm::vec3 centerPosition;//ģ�͵�����λ��
	};
}