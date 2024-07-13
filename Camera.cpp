#include "Camera.h"
#include <iostream>
namespace GL {
	Camera::Camera() {
		cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		model = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //默认模型为躺下45度的形式
		modelRotationZ = 0.0f;
	}
	Camera::~Camera(){}

	/// <summary>
	/// 通过移动相机的位置来改变相机的视角
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
		//更新视角矩阵
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		return view;
	}

	/// <summary>
	/// 更新模型的视角矩阵，非相机，更新模型本身的状态
	/// </summary>
	/// <param name="rotateZ"></param>
	/// <param name="rotationZ"></param>
	/// <returns></returns>
	glm::mat4 Camera::UpdateModelPoisition(bool enable,bool rotateX,bool rotateZ, float rotationX, float rotationZ, float LrotationX, float LrotationZ) {
		if ((!rotateX && !rotateZ)) return model;//(!rotateX && !rotateZ) || 
		model = glm::mat4(1.0f);
		if (rotateX) {
			float x = enable ? rotationX + LrotationX : LrotationX;
			model = glm::rotate(model, glm::radians(NormalizeAngle(x, 360.0f)), glm::vec3(1.0f, 0.0f, 0.0f)); //先进行X轴的旋转
		}
		else {
			model = glm::rotate(model, glm::radians(NormalizeAngle(LrotationX, 360.0f)), glm::vec3(1.0f, 0.0f, 0.0f)); //先进行X轴的旋转
		}
		if (rotateZ) {
			float z = enable ? rotationZ + LrotationZ : LrotationZ;
			model = glm::rotate(model, glm::radians(NormalizeAngle(z,360.0f)), glm::vec3(0.0f, 0.0f, 1.0f));//再沿着Z轴旋转，此时可以实现围绕自己自动旋转的效果
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