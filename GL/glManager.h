#pragma once
#include "Camera.h"
#include "model.h"
#include "lightControl.h"
#include "data.h"
#include <vector>
namespace GL {
	class GlManager {
	public:
		void Render(const Data& data);
		void CreateRandomData(unsigned int width, unsigned int height,float xoffset, float yoffset, bool random,float** vertices, unsigned int** indices,float** textures,int * vsize,int*isize,
			int* tsize,float* centerX, float* centerY);
		void CreateModelTexture(const char* path, Model* model,float* ptextures,int tsize);
		GlManager();
		~GlManager();
		void ClearModels();
		bool Init(Param* args = nullptr);
	private:
		Camera* cmaera;
		std::vector<Model*> models;
		LightControl* lightControl;
	};
}