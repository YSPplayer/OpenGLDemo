#include "glManager.h"
#include "shaderTag.h"
#include <iostream>
#include "../Util/util.h"

namespace GL {
	using namespace Tool;
	GlManager::GlManager() {
		cmaera = new Camera;
	}

	void GlManager::ClearModels() {
		for (int i = 0; i < models.size(); ++i)
			Util::ReleasePointer(models[i], false);
		models.clear();
	}

	bool GlManager::Init(Param* args) {
		//float vertices[] = {
		//	0.5f, 0.5f, 0.0f,   // 右上角
		//	0.5f, -0.5f, 0.0f,  // 右下角
		//	-0.5f, -0.5f, 0.0f, // 左下角
		//	-0.5f, 0.5f, 0.0f,   // 左上角
		//	1.0f,0.5f,0.0f,
		//	1.0f,-0.5f,0.0f
		//};
		//unsigned int indices[] = {
		//	// 注意索引从0开始! 
		//	// 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
		//	// 这样可以由下标代表顶点组合成矩形
		//	0, 1, 3, // 第一个三角形
		//	1, 2, 3,  // 第二个三角形
		//	0,4,5,
		//	0,1,5
		//};
		float* pvertices = nullptr;
		unsigned int* pindices = nullptr;
		int vsize = 0;
		int isize = 0;
		float centerX = 0.0f;
		float centerY = 0.0f;
		if (args) {
			CreateRandomData(args->w, args->h, args->x, args->y, args->random == 1, &pvertices, &pindices, &vsize, &isize, &centerX, &centerY);
		}
		else {
			CreateRandomData(20, 6, 1, 1, true, &pvertices, &pindices, &vsize, &isize, &centerX, &centerY);
		}
		/*float vertices[] = {
			0.0f,0.0f,0.0,
			0.5,0,0,
			0,0.5,0,
			0.5,0.5,0
		};

		vsize = sizeof(vertices) / sizeof(vertices[0]);

		unsigned int indices[] = {
			0,1,3,
			0,2,3
		};

		isize = sizeof(indices) / sizeof(indices[0]);*/
		Model* model = new Model;
		bool success = model->CreateModel(vertexShader,colorShader, true, pvertices, vsize, pindices,isize);
		model->SetModelCenterPoisition(glm::vec3(centerX, centerY, 0.0f));
		cmaera->SetModelCenterPoisition(glm::vec3(centerX, centerY, 0.0f));
		cmaera->ReSetPoisition();
		if (!success) return false;
		models.push_back(model);
	/*	models.push_back(model->CopyModel(success));*/
		return true;
	}

	void GlManager::Render(const Data& data) {
		if (models.size() <= 0) return;
		const glm::mat4& view = data.reset ? cmaera->ReSetPoisition() : cmaera->UpdatePoisition(data);
		const glm::mat4& projection = cmaera->UpdateProjection(data);
		for (int i = 0; i < models.size(); ++i) {
			Model* model = models[i];
			Shader* shader = model->GetShader();
			const glm::mat4& mposition = data.reset ? model->ReSetPoisition() : model->UpdatePoisition(data);
			shader->UseShader();
			shader->SetShaderMat4(view, "view");
			shader->SetShaderMat4(mposition, "model");
			shader->SetShaderMat4(projection, "projection");
			model->Render(data);
		}
	}

	/// <summary>
	/// 创建随机矩形数据，以模拟我们的可视化数据，offset是单位之间的大小
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="vertices"></param>
	/// <param name="indices"></param>
	void GlManager::CreateRandomData(unsigned int width, unsigned int height, float xoffset, float yoffset, bool random, float** vertices, unsigned int** indices, int* vsize, int* isize, float* centerX, float* centerY) {
		int max = width > height ? width : height;
		int count = width * height;
		std::vector<Point> points;
		std::vector<Indice> vindices;
	/*	points.resize(count);*/
		float xSum = 0.0f;
		float ySum = 0.0f;
		for (unsigned int j = 0; j < height + 1; ++j) {//先赋值横向的宽度，再赋值纵向的高度
			for (unsigned int i = 0; i < width + 1; ++i) {
				Point point;
				point.z = random ? (Util::GetRandomFloat(-10.0f, 10.0f) / 100.0f) : 0.0f;;//归一化
				point.x = static_cast<float>((i * xoffset)) / static_cast<float>(max); //归一化
				point.y = static_cast<float>((j * yoffset)) / static_cast<float>(max);//归一化
				xSum += point.x;
				ySum += point.y;
				points.push_back(point);
				if (i < width && j < height) {
					vindices.push_back({ i + (width + 1) * j, i + 1 + (width + 1) * j, i + (width + 1) * (j + 1) });
					vindices.push_back({ (i + 1) + (width + 1) * j, i + (width + 1) * (j + 1), i + 1 + (width + 1) * (j + 1) });
				}
			}
		}
		//获取到模型的中心坐标位置
		*centerX = xSum / points.size();
		*centerY = ySum / points.size();

		*vsize = points.size() * 3;
		*vertices = new float[*vsize];
		for (int i = 0; i < points.size(); ++i) {
			Point& point = points[i];
			(*vertices)[i * 3 + 0] = point.x;
			(*vertices)[i * 3 + 1] = point.y;
			(*vertices)[i * 3 + 2] = point.z;
	/*		std::cout << "=== point_start ===" << std::endl;
			std::cout << "=== point_x ===" << point.x << std::endl;
			std::cout << "=== point_y ===" << point.y << std::endl;
			std::cout << "=== point_z ===" << point.z << std::endl;
			std::cout << "=== point_end ===" << std::endl;*/
		}
		*isize = vindices.size() * 3;
		*indices = new unsigned int[vindices.size() * 3];
		for (int i = 0; i < vindices.size(); ++i) {
			Indice& indice = vindices[i];
			(*indices)[i * 3 + 0] = indice.index1;
			(*indices)[i * 3 + 1] = indice.index2;
			(*indices)[i * 3 + 2] = indice.index3;
			/*std::cout << "=== indice_start ===" << std::endl;
			std::cout << "=== indice_0 ===" << indice.index1 << std::endl;
			std::cout << "=== indice_1 ===" << indice.index2 << std::endl;
			std::cout << "=== indice_2 ===" << indice.index3 << std::endl;
			std::cout << "=== indice_end ===" << std::endl;*/
		}
	}

}