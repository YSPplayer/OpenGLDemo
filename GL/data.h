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

#define LIGHT_PARALLEL 0 //平行光
#define LIGHT_LINEAR_POINT 1 //线性点光源
#define NON_LINEAR_POINT_LIGHT 2 //非线性点光源
#define SPOT_LIGHT 3 //聚光

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
		double alpha;//镜面贴图对比度[1.0-10]
		int beta;//镜面贴图亮度[0-100]
		bool transparentBg;//imgui背景是否设置透明
		int lightType;//光的类型（平行光 点光源 聚光）
		float phi;//球体方位角[0 - π] 垂直
		float theta;//球体极角[-π - π] 水平旋转
		bool showLightMode;
		bool sparsePoint;//点云稀疏
		float zFactor;//Z轴放大倍数
		bool blinn;//是否启用Blinn-Phong光照 高光更柔和，适合表现较为光滑、漫反射较多的表面。  Phong:高光点更尖锐，适合表现较硬的、高度反射的表面。
		bool gammaCorrection;//光照伽马校正-图像亮度更自然，细节保留更好，整体视觉效果更符合人眼的感知
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