#pragma once
#include "Camera.h"
#include "model.h"
#include "data.h"
#include <vector>
namespace GL {
	class GlManager {
	public:
		void Render(const Data& data);
		void CreateRandomData(unsigned int width, unsigned int height,float xoffset, float yoffset, bool random,float** vertices, unsigned int** indices,int * vsize,int*isize,
			float* centerX, float* centerY);
		GlManager();
		~GlManager() {};
		void ClearModels();
		bool Init(Param* args = nullptr);
	private:
		Camera* cmaera;
		std::vector<Model*> models;
	};
}