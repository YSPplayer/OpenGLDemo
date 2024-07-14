#pragma once
namespace GL {
#define MOVE_FORWARD 0
#define MOVE_BACK 1
#define MOVE_LEFT 2
#define MOVE_RIGHT 3
#define MOVE_NONE 4
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
	};
}