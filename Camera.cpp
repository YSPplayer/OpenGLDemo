#include "Camera.h"
#include <iostream>
namespace GL {
	Camera::Camera() {
		cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		model = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //Ĭ��ģ��Ϊ����45�ȵ���ʽ
		modelRotationZ = 0.0f;
	}
	Camera::~Camera(){}

	/// <summary>
	/// ͨ���ƶ������λ�����ı�������ӽ�
	/// </summary>
	/// <param name="cameraPos"></param>
	glm::mat4 Camera::UpdateCameraPoisition(int type,float speed, float yaw, float pitch,bool isYaw) {
		if (isYaw) {
			glm::vec3 front;
			front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			front.y = sin(glm::radians(pitch));
			front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			cameraFront = glm::normalize(front);
			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		}
		if(type == MOVE_NONE) return view;
		if(type == MOVE_FORWARD) cameraPos += speed * cameraFront;
		else if(type == MOVE_BACK) cameraPos -= speed * cameraFront;
		else if(type == MOVE_LEFT) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
		else if(type == MOVE_RIGHT) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
		//�����ӽǾ���
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		return view;
	}

	/// <summary>
	/// ����ģ�͵��ӽǾ��󣬷����������ģ�ͱ����״̬
	/// </summary>
	/// <param name="rotateZ"></param>
	/// <param name="rotationZ"></param>
	/// <returns></returns>
	glm::mat4 Camera::UpdateModelPoisition(bool enable,bool rotateX,bool rotateZ, float rotationX, float rotationZ, float LrotationX, float LrotationZ) {
		if ((!rotateX && !rotateZ)) return model;//(!rotateX && !rotateZ) || 
		model = glm::mat4(1.0f);
		if (rotateX) {
			float x = enable ? rotationX + LrotationX : LrotationX;
			model = glm::rotate(model, glm::radians(NormalizeAngle(x, 360.0f)), glm::vec3(1.0f, 0.0f, 0.0f)); //�Ƚ���X�����ת
		}
		else {
			model = glm::rotate(model, glm::radians(NormalizeAngle(LrotationX, 360.0f)), glm::vec3(1.0f, 0.0f, 0.0f)); //�Ƚ���X�����ת
		}
		if (rotateZ) {
			float z = enable ? rotationZ + LrotationZ : LrotationZ;
			model = glm::rotate(model, glm::radians(NormalizeAngle(z,360.0f)), glm::vec3(0.0f, 0.0f, 1.0f));//������Z����ת����ʱ����ʵ��Χ���Լ��Զ���ת��Ч��
		}
		else {
			model = glm::rotate(model, glm::radians(NormalizeAngle(LrotationZ,360.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
		}
		return model;
	}

	float  Camera::NormalizeAngle(float angle, float factor) {
		angle = fmod(angle, factor);
		if (angle < 0) {
			angle += factor;
		}
		return angle;
	}
}