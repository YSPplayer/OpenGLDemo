#pragma once
#include "model.h"
namespace GL {
	/*�ƹ������*/
	class LightControl {
	public:
		LightControl();
		~LightControl();
		glm::mat4 lightModelPos;//ģ�͵�λ��
		Model* lightModel;//��Դģ��
		void EnableLightModel();
	private:
		//��Դ�ĵ�ǰλ��
		glm::vec3 lightPos;
	};
}