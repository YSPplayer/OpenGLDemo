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
		glm::vec3 cameraFront;//摄像机看向的方向，看向Z轴的负方向【写死】
		glm::vec3 cameraPos; //相机的世界坐标系位置
		glm::vec3 cameraRight;//前方向和后方向计算的结果向量
		glm::vec3 cameraWorldUp;//世界坐标系的上方向向量，固定不变，根据这个向量与Z方向向量叉乘，可以得到X轴的方向向量【写死】
		glm::vec3 cameraUp;//相机的视角向量，表示相机的本地上方向
		glm::mat4 view;//相机视角的观察矩阵，对于任意世界坐标系中的模型，都可以转换到这个位置中 
		glm::mat4 projection;//透视或平行视口
		glm::vec3 centerPosition;//模型的中心位置
	};
}