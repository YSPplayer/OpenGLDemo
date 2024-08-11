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
		glm::mat4  ReSetPoisition();
		inline void SetModelCenterPoisition(const glm::vec3& centerPosition) {
			this->centerPosition = centerPosition;
		};
		inline glm::vec3 GetCameraPos() {
			return cameraPos;
		}
		inline glm::vec3 GetCameraFront() {
			return cameraFront;
		}
	private:
		glm::vec3 cameraFront;//���������ķ��򣬿���Z��ĸ�����д����
		glm::vec3 cameraPos; //�������������ϵλ��
		glm::vec3 cameraRight;//ǰ����ͺ������Ľ������
		glm::vec3 cameraWorldUp;//��������ϵ���Ϸ����������̶����䣬�������������Z����������ˣ����Եõ�X��ķ���������д����
		glm::vec3 cameraUp;//������ӽ���������ʾ����ı����Ϸ���
		glm::mat4 view;//����ӽǵĹ۲���󣬶���������������ϵ�е�ģ�ͣ�������ת�������λ���� 
		glm::mat4 projection;//͸�ӻ�ƽ���ӿ�
		glm::vec3 centerPosition;//ģ�͵�����λ��
	};
}