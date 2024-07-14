#pragma once
#include "Camera.h"
#include "model.h"
#include "data.h"
#include <vector>
namespace GL {
	class GlManager {
	public:
		void Render(const Data& data);
		void CreateRandomData(unsigned int width, unsigned int height,float xoffset, float yoffset, float** vertices, unsigned int** indices,int * vsize,int*isize);
		GlManager();
		~GlManager() {};
		void ClearModels();
		bool Init();
	private:
		Camera* cmaera;
		std::vector<Model*> models;
	};
}