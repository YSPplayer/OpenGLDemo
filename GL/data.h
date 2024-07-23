#pragma once
namespace GL {
#define MOVE_FORWARD 0
#define MOVE_BACK 1
#define MOVE_LEFT 2
#define MOVE_RIGHT 3
#define MOVE_NONE 4

#define DRAW_MODE_POINT 0
#define DRAW_MODE_GRID 1
#define DRAW_MODE_SURFACE 2

//最大偏移量
#define MAX_X_OFFSET 10.0f
#define MAX_Y_OFFSET 10.0f
	struct Param {
		int w;
		int h;
		float x;
		float y;
		int random;
	};
	struct Point {
		float x;
		float y;
		float z;
	};
	struct Indice {
		unsigned int index1;
		unsigned int index2;
		unsigned int index3;
	};
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
		bool reset;//重置模型+相机位置
		int drawMode;//绘制模式
	};
	//UI组件存放的数据
	struct UData {
		double modelWidth;
		double modelHeight;
		double modelXOffset;
		double modelYOffset;
		double modelRandomRange;
		bool modelRandomZ;
		char pathBuf[256];
		int selectedRadio;
	};
}