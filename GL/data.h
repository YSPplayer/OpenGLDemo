#pragma once
#include <glm/glm.hpp>
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
//-0.0 -55.0
#define DEFAULT_MODEL_X_RADIANS 0.0f //默认模型的开始X轴角度
#define DEFAULT_ASPECT 0.3f //默认的缩放大小
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
		bool cullBackFace;//是否剔除模型背面
		float moveSpeed;//相机最终移动速度，带有帧率
		float moveSpeedUnit; //[1-10]
		float sensitivity;//相机视角灵敏度[0.01-0.1]
		float rotationZ;
		float lastRotationZ;
		float colors[3][4];//存放背景、模型、光照颜色
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
		float ambientStrength;//环境光照调节因子
		float specularStrength;//镜面光照调节因子
		float reflectivity;//反射度因子
		bool useLight;//是否使用灯光
		bool useTexture;//是否启用贴图
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
	struct Material {
		glm::vec3 ambient; //环境光照色
		glm::vec3 diffuse; //漫反射颜色
		glm::vec3 specular; //镜面高光的颜色   
		float shininess;//光的散射半径
	};

}