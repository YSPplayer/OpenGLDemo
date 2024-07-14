#pragma once
#include "Camera.h"
#include "model.h"
#include "data.h"
#include <vector>
namespace GL {
	class GlManager {
	public:
		void Render(const Data& data);
		GlManager();
		~GlManager() {};
		void ClearModels();
		bool Init();
	private:
		Camera* cmaera;
		std::vector<Model*> models;
	};
}