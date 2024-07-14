#pragma once
namespace GL {
#define MOVE_FORWARD 0
#define MOVE_BACK 1
#define MOVE_LEFT 2
#define MOVE_RIGHT 3
#define MOVE_NONE 4
	struct Data {
		//屏幕长宽
		int width;
		int height;
		int moveType;//移动模式
		float moveSpeed;//移动速度
		float rotationZ;
		float lastRotationZ;
		bool rotateZ;
		float rotationX;
		float lastRotationX;
		bool enable;
		bool rotateX;
		float aspect;//相机视野大小
		float pitch;
		float yaw;
		bool isYaw;
	};
}