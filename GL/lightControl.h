#pragma once
#include "model.h"
namespace GL {
	/*灯光控制类*/
	class LightControl {
	public:
		LightControl();
		~LightControl();
		glm::mat4 lightModelPos;//模型的位置
		Model* lightModel;//光源模型
		void EnableLightModel();
	private:
		//光源的当前位置
		glm::vec3 lightPos;
	};
}