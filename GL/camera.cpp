#include "Camera.h"
#include <iostream>
namespace GL {
	Camera::Camera() {
		cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraWorldUp = glm::vec3(0.0f, 1.0f,  0.0f);
		cameraRight = glm::normalize(glm::cross(cameraFront, cameraWorldUp));
		cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		projection = glm::mat4(1.0f);
		centerPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	Camera::~Camera(){}

	/// <summary>
	/// 通过移动相机的位置来改变相机的视角
	/// </summary>
	/// <param name="cameraPos"></param>
	glm::mat4 Camera::UpdatePoisition(const Data& data) {
		if (data.isYaw) {
			glm::vec3 front;
			front.x = cos(glm::radians(data.yaw)) * cos(glm::radians(data.pitch));
			front.y = sin(glm::radians(data.pitch));
			front.z = sin(glm::radians(data.yaw)) * cos(glm::radians(data.pitch));
			cameraFront = glm::normalize(front);
			cameraRight = glm::normalize(glm::cross(cameraFront, cameraWorldUp));
			cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		}
		if(data.moveType == MOVE_NONE) return view;
		if(data.moveType == MOVE_FORWARD) cameraPos += data.moveSpeed * cameraFront;
		else if(data.moveType == MOVE_BACK) cameraPos -= data.moveSpeed * cameraFront;
		else if(data.moveType == MOVE_LEFT) cameraPos -=  data.moveSpeed * cameraRight;
		else if(data.moveType == MOVE_RIGHT) cameraPos += data.moveSpeed * cameraRight;
		//更新视角矩阵
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		return view;
	}

	/// <summary>
	/// 透视或平行视口
	/// </summary>
	/// <param name="data"></param>
	/// <returns></returns>
	glm::mat4 Camera::UpdateProjection(const Data& data) {
		projection = glm::perspective(data.aspect, static_cast<float>(data.width) / static_cast<float>(data.height), 0.1f, 100.0f);
		return projection;
	}

	/// <summary>
	/// 重置相机位置
	/// </summary>
	/// <param name="pos"></param>
	glm::mat4  Camera::ReSetPoisition() {
		cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		// 相机位置到模型中心的向量
		glm::vec3 direction = centerPosition - cameraPos;
		cameraFront = glm::normalize(direction);
		cameraRight = glm::normalize(glm::cross(cameraFront, cameraWorldUp));
		cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		return view;
	}
}