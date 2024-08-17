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

#define LIGHT_PARALLEL 0 //ƽ�й�
#define LIGHT_LINEAR_POINT 1 //���Ե��Դ
#define NON_LINEAR_POINT_LIGHT 2 //�����Ե��Դ
#define SPOT_LIGHT 3 //�۹�

	//���ƫ����
#define MAX_X_OFFSET 10.0f
#define MAX_Y_OFFSET 10.0f
//-0.0 -55.0
#define DEFAULT_MODEL_X_RADIANS 0.0f //Ĭ��ģ�͵Ŀ�ʼX��Ƕ�
#define DEFAULT_ASPECT 0.3f //Ĭ�ϵ����Ŵ�С
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
		bool cullBackFace;//�Ƿ��޳�ģ�ͱ���
		float moveSpeed;//��������ƶ��ٶȣ�����֡��
		float moveSpeedUnit; //[1-10]
		float sensitivity;//����ӽ�������[0.01-0.1]
		float rotationZ;
		float lastRotationZ;
		float colors[3][4];//��ű�����ģ�͡�������ɫ
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
		int drawMode;//����ģʽ
		float ambientStrength;//�������յ�������
		float specularStrength;//������յ�������
		float reflectivity;//���������
		bool useLight;//�Ƿ�ʹ�õƹ�
		bool useTexture;//�Ƿ�������ͼ
		double alpha;//������ͼ�Աȶ�[1.0-10]
		int beta;//������ͼ����[0-100]
		bool transparentBg;//imgui�����Ƿ�����͸��
		int lightType;//������ͣ�ƽ�й� ���Դ �۹⣩
		float phi;//���巽λ��[0 - ��] ��ֱ
		float theta;//���弫��[-�� - ��] ˮƽ��ת
		bool showLightMode;
		bool sparsePoint;//����ϡ��
		float zFactor;//Z��Ŵ���
		bool blinn;//�Ƿ�����Blinn-Phong���� �߹����ͣ��ʺϱ��ֽ�Ϊ�⻬��������϶�ı��档  Phong:�߹��������ʺϱ��ֽ�Ӳ�ġ��߶ȷ���ı��档
		bool gammaCorrection;//����٤��У��-ͼ�����ȸ���Ȼ��ϸ�ڱ������ã������Ӿ�Ч�����������۵ĸ�֪
	};
	//UI�����ŵ�����
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
		glm::vec3 ambient; //��������ɫ
		glm::vec3 diffuse; //��������ɫ
		glm::vec3 specular; //����߹����ɫ   
		float shininess;//���ɢ��뾶
	};

}