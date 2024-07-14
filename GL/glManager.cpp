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

	bool GlManager::Init() {
		float vertices[] = {
			0.5f, 0.5f, 0.0f,   // 右上角
			0.5f, -0.5f, 0.0f,  // 右下角
			-0.5f, -0.5f, 0.0f, // 左下角
			-0.5f, 0.5f, 0.0f,   // 左上角
			1.0f,0.5f,0.0f,
			1.0f,-0.5f,1.0f
		};
		unsigned int indices[] = {
			// 注意索引从0开始! 
			// 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
			// 这样可以由下标代表顶点组合成矩形
			0, 1, 3, // 第一个三角形
			1, 2, 3,  // 第二个三角形
			0,4,5,
			0,1,5
		};
		Model* model = new Model;
		bool success = model->CreateModel(vertexShader,colorShader, vertices, sizeof(vertices) / sizeof(vertices[0]), indices, sizeof(indices) / sizeof(indices[0]));
		if (!success) return false;
		models.push_back(model);
		return true;
	}

	void GlManager::Render(const Data& data) {
		if (models.size() <= 0) return;
		const glm::mat4& view = cmaera->UpdatePoisition(data);
		const glm::mat4& projection = cmaera->UpdateProjection(data);
		for (int i = 0; i < models.size(); ++i) {
			Model* model = models[i];
			Shader* shader = model->GetShader();
			const glm::mat4& mposition = model->UpdatePoisition(data);
			shader->UseShader();
			shader->SetShaderMat4(view,"view");
			shader->SetShaderMat4(mposition, "model");
			shader->SetShaderMat4(projection, "projection");
			model->Render();
		}
	}

}