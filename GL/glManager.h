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
		float CreateRandomData(unsigned int width, unsigned int height, float xoffset, float yoffset, float minZ, float maxZ, float* pointsZ, bool random, float randomRange, float** vertices, unsigned int** indices, float** textures, int* vsize, int* isize,
			int* tsize, glm::vec3& centerPos, unsigned int& outWidth, unsigned int& outHeight);
		float CreateX3pData(unsigned int _width, unsigned int _height, float xoffset, float yoffset, float minZ, float maxZ, float* pointsZ, float** vertices, unsigned int** indices, float** textures, int* vsize, int* isize,
			int* tsize, glm::vec3& centerPos, unsigned int& outWidth, unsigned int& outHeight);
		void CreateModelTexture(const char* path, Model* model, float* ptextures, int tsize);
		void ChangeModelSpecularImage(Model* model, double alpha, int beta);
		GlManager();
		~GlManager();
		void ClearModels();
		bool CreateX3pModel(unsigned int width, unsigned int height, float xoffset, float yoffset, float* zpointData, float minZ, float maxZ);
		bool CreateRandomModel(const UData& udata);
		bool Init(Param* args = nullptr);
		inline Model* GetCurrentModel() {
			return models.size() > 0 ? models[0] : nullptr;
		}
		inline LightControl* GetLightControl() {
			return lightControl;
		}
	private:
		Camera* cmaera;
		std::vector<Model*> models;
		LightControl* lightControl;
	};
}