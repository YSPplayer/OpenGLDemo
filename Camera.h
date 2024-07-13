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
		glm::vec3 cameraFront;//摄像机看向的方向，看向Z轴的负方向【写死】
		glm::vec3 cameraPos; //相机的世界坐标系位置
		glm::mat4 UpdateCameraPoisition(int type,float speed,float yaw,float pitch,bool isYaw);
		glm::mat4 UpdateModelPoisition(bool enable,bool rotateX, bool rotateZ,float rotationX ,float rotationZ, float LrotationX, float LrotationZ);
	private:
		float NormalizeAngle(float angle,float factor);
		glm::vec3 cameraUp;//摄像机的上方向向量，根据这个向量与Z方向向量叉乘，可以得到X轴的方向向量【写死】
		glm::mat4 view;//相机视角的观察矩阵，对于任意世界坐标系中的模型，都可以转换到这个位置中 
		glm::mat4 model;//默认的模型矩阵
		float modelRotationZ;//模型当前的旋转角度
	};
}