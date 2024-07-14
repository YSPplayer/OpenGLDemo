#pragma once
namespace GL {
#define MOVE_FORWARD 0
#define MOVE_BACK 1
#define MOVE_LEFT 2
#define MOVE_RIGHT 3
#define MOVE_NONE 4
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
		//��Ļ����
		int width;
		int height;
		int moveType;//�ƶ�ģʽ
		float moveSpeed;//�ƶ��ٶ�
		float rotationZ;
		float lastRotationZ;
		bool rotateZ;
		float rotationX;
		float lastRotationX;
		bool enable;
		bool rotateX;
		float aspect;//�����Ұ��С
		float pitch;
		float yaw;
		bool isYaw;
		bool reset;//����ģ��+���λ��
		bool drawLine;//�Ƿ�����������ģʽ
	};
}