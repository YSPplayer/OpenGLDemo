#pragma once
#include "Camera.h"
#include "model.h"
#include "lightControl.h"
#include "data.h"
#include <vector>
namespace GL {
	class GlManager {
	public:
		static float aspectUnit;
		void Render(Data& data);
		void CreateRandomData(unsigned int width, unsigned int height, float xoffset, float yoffset, float minZ,float maxZ,float* pointsZ, bool random, float randomRange, float** vertices, unsigned int** indices, float** textures, int* vsize, int* isize,
			int* tsize, float* centerX, float* centerY);
		void CreateX3pData(unsigned int _width, unsigned int _height, float xoffset, float yoffset, float minZ, float maxZ, float* pointsZ,float** vertices, unsigned int** indices, float** textures, int* vsize, int* isize,
			int* tsize, float* centerX, float* centerY);
		void CreateModelTexture(const char* path, Model* model,float* ptextures,int tsize);
		GlManager();
		~GlManager();
		void ClearModels();
		bool CreateX3pModel(unsigned int width, unsigned int height, float xoffset, float yoffset, float* zpointData,float minZ,float maxZ);
		bool CreateRandomModel(const UData& udata);
		bool Init(Param* args = nullptr);
		inline Model* GetCurrentModel() {
			return models.size() > 0 ? models[0] : nullptr;
		}
	private:
		Camera* cmaera;
		std::vector<Model*> models;
		LightControl* lightControl;
	};
}