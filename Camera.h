#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace GL {
#define MOVE_FORWARD 0
#define MOVE_BACK 1
#define MOVE_LEFT 2
#define MOVE_RIGHT 3
#define MOVE_NONE 4
	class Camera {
	public:
		Camera();
		~Camera();
		glm::vec3 cameraFront;//���������ķ��򣬿���Z��ĸ�����д����
		glm::vec3 cameraPos; //�������������ϵλ��
		glm::mat4 UpdateCameraPoisition(int type,float speed,float yaw,float pitch,bool isYaw);
		glm::mat4 UpdateModelPoisition(bool enable,bool rotateX, bool rotateZ,float rotationX ,float rotationZ, float LrotationX, float LrotationZ);
	private:
		float NormalizeAngle(float angle,float factor);
		glm::vec3 cameraUp;//��������Ϸ����������������������Z����������ˣ����Եõ�X��ķ���������д����
		glm::mat4 view;//����ӽǵĹ۲���󣬶���������������ϵ�е�ģ�ͣ�������ת�������λ���� 
		glm::mat4 model;//Ĭ�ϵ�ģ�;���
		float modelRotationZ;//ģ�͵�ǰ����ת�Ƕ�
	};
}