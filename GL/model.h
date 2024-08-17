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
		bool SetTexture(unsigned char* texture, unsigned char* specularTexture, int width,int height,int nrChannels,float datas[], int size,bool gammaCorrection);
		Material material;
		Model* CopyModel(bool& success);
		inline void SetModelCenterPoisition(const glm::vec3& centerPosition) {
			this->centerPosition = centerPosition;
		};
		inline glm::vec3 GetModelCenterPoisition() {
			return centerPosition;
		}
		glm::mat4 UpdatePoisition(Data& data);
		void Render(const Data& data);
		inline Shader* GetShader() {
			return pshader;
		}
		glm::mat4 ReSetPoisition();
		GLuint TEXTURE;//��ͼ������󣬲�����ͼ���ݵĶ�����󣬶���������VBO����
		GLuint SPECULAR_TEXTURE;//���淴����ͼ
		inline bool HasTexture() {
			return hasTexture && hasSpecularTexture;
		};
		bool CalculateVertexNormals();
	private:
		bool hasSpecularTexture;//�Ƿ��о�����ͼ
		bool hasTexture;//��� ��ǰģ���Ƿ�����ͼ
		bool copy;//��⵱ǰģ���Ƿ��ǿ���������
		float* vertices;//��Ż���ͼ��Ķ�������
		unsigned int* indices;//��Ŷ������ݵ�����λ����
		int verticesSize;//�������ݵĴ�С
		int indicesSize;//�������ݵĴ�С
		int normalSize;//���㷨�����ݴ�С
		float* normals;//���㷨������
		bool eboMode;
		std::vector<GLuint>* PVBOS; //�����㻺�������Ҫ�洢�������ݣ�Ȼ���͸�gpu��Ⱦ��
		GLuint VAO;//��VBO����ͳһ����Ķ��󣬷����������
		GLuint EBO;//�������������󣬻��ƾ���ʱʹ�ã���������Ч�ʡ�
		glm::mat4 position;//Ĭ�ϵ�ģ�;������ڿ���ģ�͵�λ�úͷ���
		Shader* pshader;//��ɫ���������
		glm::vec3 centerPosition;//ģ�͵�����λ��
	};
}