#include "lightControl.h"
#include "shaderTag.h"
#include "../Util/util.h"
namespace GL {
	using namespace GL::Tool;
	LightControl::LightControl() {
		lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
		lightModelPos = glm::translate(glm::mat4(1.0f), lightPos);
		lightModelPos = glm::scale(lightModelPos, glm::vec3(0.2f)); //模型缩小
		lightModel = new Model;
	}

	/// <summary>
	/// 启用光照模型
	/// </summary>
	void LightControl::EnableLightModel() {
		//创建光源正方体
		float vertices[] = {
			-0.5f, -0.5f, -0.5f, // Vertex 0
			 0.5f, -0.5f, -0.5f, // Vertex 1
			 0.5f,  0.5f, -0.5f, // Vertex 2
			-0.5f,  0.5f, -0.5f, // Vertex 3
			-0.5f, -0.5f,  0.5f, // Vertex 4
			 0.5f, -0.5f,  0.5f, // Vertex 5
			 0.5f,  0.5f,  0.5f, // Vertex 6
			-0.5f,  0.5f,  0.5f  // Vertex 7
		};
		int vsize = sizeof(vertices) / sizeof(vertices[0]);
		unsigned int indices[] = {
			0, 1, 2, 0, 2, 3,// Front face
			4, 5, 6, 4, 6, 7,// Back face
			4, 7, 3, 4, 3, 0,// Left face
			1, 5, 6, 1, 6, 2,// Right face
			0, 1, 5, 0, 5, 4,// Bottom face
			3, 2, 6, 3, 6, 7// Top face
		};
		int isize = sizeof(indices) / sizeof(indices[0]);
		lightModel->CreateModel(vertexShader, colorLightShader, true, vertices, vsize, indices, isize);
	}

	LightControl::~LightControl() {
		Util::ReleasePointer(lightModel);
	}
}
