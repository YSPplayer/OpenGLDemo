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
		glm::vec3 cameraFront;//摄像机看向的方向，看向Z轴的负方向【写死】
		glm::vec3 cameraPos; //相机的世界坐标系位置
		glm::vec3 cameraUp;//摄像机的上方向向量，根据这个向量与Z方向向量叉乘，可以得到X轴的方向向量【写死】
		glm::mat4 view;//相机视角的观察矩阵，对于任意世界坐标系中的模型，都可以转换到这个位置中 
		glm::mat4 projection;//透视或平行视口
	};
}