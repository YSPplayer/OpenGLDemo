#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "data.h"
namespace GL {
	class Camera {
	public:
		Camera();
		~Camera();
		glm::mat4 UpdatePoisition(const Data& data);
		glm::mat4 UpdateProjection(const Data& data);
	private:
		glm::vec3 cameraFront;//���������ķ��򣬿���Z��ĸ�����д����
		glm::vec3 cameraPos; //�������������ϵλ��
		glm::vec3 cameraUp;//��������Ϸ����������������������Z����������ˣ����Եõ�X��ķ���������д����
		glm::mat4 view;//����ӽǵĹ۲���󣬶���������������ϵ�е�ģ�ͣ�������ת�������λ���� 
		glm::mat4 projection;//͸�ӻ�ƽ���ӿ�
	};
}