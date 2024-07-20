#define STB_IMAGE_IMPLEMENTATION //��stb_image.h��ΪԴ�ļ�
#include "glManager.h"
#include "shaderTag.h"
#include <iostream>
#include <stb_image.h>
#include "../Util/util.h"

namespace GL {
	using namespace Tool;
	GlManager::GlManager() {
		cmaera = new Camera;
		lightControl = new LightControl;
	}

	GlManager::~GlManager() {
		Util::ReleasePointer(cmaera);
		Util::ReleasePointer(lightControl);
	}

	void GlManager::ClearModels() {
		for (int i = 0; i < models.size(); ++i)
			Util::ReleasePointer(models[i], false);
		models.clear();
	}

	bool GlManager::Init(Param* args) {
		//float vertices[] = {
		//	0.5f, 0.5f, 0.0f,   // ���Ͻ�
		//	0.5f, -0.5f, 0.0f,  // ���½�
		//	-0.5f, -0.5f, 0.0f, // ���½�
		//	-0.5f, 0.5f, 0.0f,   // ���Ͻ�
		//	1.0f,0.5f,0.0f,
		//	1.0f,-0.5f,0.0f
		//};
		//unsigned int indices[] = {
		//	// ע��������0��ʼ! 
		//	// ����������(0,1,2,3)���Ƕ�������vertices���±꣬
		//	// �����������±��������ϳɾ���
		//	0, 1, 3, // ��һ��������
		//	1, 2, 3,  // �ڶ���������
		//	0,4,5,
		//	0,1,5
		//};
		float* pvertices = nullptr;
		unsigned int* pindices = nullptr;
		float* ptextures = nullptr;
		int vsize = 0;
		int isize = 0;
		int tsize = 0;
		float centerX = 0.0f;
		float centerY = 0.0f;
		if (args) {
			CreateRandomData(args->w, args->h, args->x, args->y, args->random == 1, &pvertices, &pindices,&ptextures, &vsize, &isize,&tsize,&centerX, &centerY);
		}
		else {
			/*
			�������ֵ 5000 * 5000 2GB 400 580
			*/
			CreateRandomData(400, 580, 1, 1, false, &pvertices, &pindices, &ptextures, &vsize, &isize, &tsize, &centerX, &centerY);
		}
		/*float vertices[] = {
			 0.5f,  0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			-0.5f, -0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};

		vsize = sizeof(vertices) / sizeof(vertices[0]);

		unsigned int indices[] = {
			 0, 1, 3,
			 1, 2, 3
		};

		isize = sizeof(indices) / sizeof(indices[0]);*/
		Model* model = new Model;
		bool success = model->CreateModel(vertexShader,colorShader, false, pvertices, vsize, pindices,isize);
		/*CreateModelTexture("C:/Users/YSP/Desktop/pics/1.jpg", model,ptextures,tsize);*/
		model->SetModelCenterPoisition(glm::vec3(centerX, centerY, 0.0f));
		cmaera->SetModelCenterPoisition(glm::vec3(centerX, centerY, 0.0f));
		cmaera->ReSetPoisition();
		if (!success) return false;
		models.push_back(model);
	/*	models.push_back(model->CopyModel(success));*/
		lightControl->EnableLightModel();//���ù���ģ��
		return true;
	}

	void GlManager::Render(const Data& data) {
		if (models.size() <= 0) return;
		const glm::mat4& view = data.reset ? cmaera->ReSetPoisition() : cmaera->UpdatePoisition(data);
		const glm::mat4& projection = cmaera->UpdateProjection(data);
		for (int i = 0; i < models.size(); ++i) {
			Model* model = models[i];
			if(model->TEXTURE) glBindTexture(GL_TEXTURE_2D, model->TEXTURE);
			Shader* shader = model->GetShader();
			const glm::mat4& mposition = data.reset ? model->ReSetPoisition() : model->UpdatePoisition(data);
			shader->UseShader();
			shader->SetShaderMat4(view, "view");
			shader->SetShaderMat4(mposition, "model");
			shader->SetShaderMat4(projection, "projection");
			shader->SetShaderBoolean(model->TEXTURE,"useTexture");
			model->Render(data);
		}
		//���Ƶƹ�ģ��
		Model* lightModel = lightControl->lightModel;
		Shader* shader = lightModel->GetShader();
		const glm::mat4& mposition = data.reset ? lightModel->ReSetPoisition() : lightModel->UpdatePoisition(data);
		shader->UseShader();
		shader->SetShaderMat4(view, "view");
		shader->SetShaderMat4(lightControl->lightModelPos, "model");//�ƹ�ģ�͹̶�һ��λ��
		shader->SetShaderMat4(projection, "projection");
		lightModel->Render(data);
	}

	/// <summary>
	/// ��������������ݣ���ģ�����ǵĿ��ӻ����ݣ�offset�ǵ�λ֮��Ĵ�С
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="vertices"></param>
	/// <param name="indices"></param>
	void GlManager::CreateRandomData(unsigned int width, unsigned int height, float xoffset, float yoffset, bool random, float** vertices, unsigned int** indices, float** textures, int* vsize, int* isize,
		int* tsize, float* centerX, float* centerY) {
		int max = width > height ? width : height;
		int count = width * height;
		std::vector<Point> points;
		std::vector<Indice> vindices;
	/*	points.resize(count);*/
		*tsize = (height + 1) * (width + 1) * 2;
		*textures = new float[*tsize];
		float xSum = 0.0f;
		float ySum = 0.0f;
		int tindex = 0;
		int x = 0;
		int y = 0;
		for (unsigned int j = 0; j < height + 1; ++j) {//�ȸ�ֵ����Ŀ�ȣ��ٸ�ֵ����ĸ߶�
			for (unsigned int i = 0; i < width + 1; ++i) {
				Point point;
				point.z = random ? (Util::GetRandomFloat(-10.0f, 10.0f) / 1000.0f) : 0.0f;;//��һ��
				x = static_cast<float>((i * xoffset));
				y = static_cast<float>((j * yoffset));
				point.x = x / static_cast<float>(max * xoffset); //��һ��
				point.y = y / static_cast<float>(max * yoffset);//��һ��
				(*textures)[tindex++] = x / static_cast<float>(width * xoffset);
				(*textures)[tindex++] = y / static_cast<float>(height * yoffset);
				xSum += point.x;
				ySum += point.y;
				points.push_back(point);
				if (i < width && j < height) {
					vindices.push_back({ i + (width + 1) * j, i + 1 + (width + 1) * j, i + (width + 1) * (j + 1) });
					vindices.push_back({ (i + 1) + (width + 1) * j, i + (width + 1) * (j + 1), i + 1 + (width + 1) * (j + 1) });
				}
			}
		}
		//��ȡ��ģ�͵���������λ��
		*centerX = xSum / points.size();
		*centerY = ySum / points.size();

		*vsize = points.size() * 3;
		*vertices = new float[*vsize];
		for (int i = 0; i < points.size(); ++i) {
			Point& point = points[i];
			(*vertices)[i * 3 + 0] = point.x;
			(*vertices)[i * 3 + 1] = point.y;
			(*vertices)[i * 3 + 2] = point.z;
			//std::cout << " point.x:" << point.x << " " << " point.y:" << point.y << " " << " point.z:" << point.z << std::endl;
		}
	/*	for (int i = 0; i < *tsize; ++i) {
			std::cout << "textures:" << (*textures)[i] << std::endl;
		}*/
		*isize = vindices.size() * 3;
		*indices = new unsigned int[vindices.size() * 3];
		for (int i = 0; i < vindices.size(); ++i) {
			Indice& indice = vindices[i];
			(*indices)[i * 3 + 0] = indice.index1;
			(*indices)[i * 3 + 1] = indice.index2;
			(*indices)[i * 3 + 2] = indice.index3;
		}
	}

	/// <summary>
	/// ��������
	/// </summary>
	/// <param name="path"></param>
	void GlManager::CreateModelTexture(const char* path,Model* model,float* ptextures, int tsize) {
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);//���ͼƬ��ȡ�ߵ����޸��������
		unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
		if (data) { //�ⲿͼƬ���سɹ�
			//float textures[] = {
			// 1.0f, 1.0f,
			// 1.0f, 0.0f,
			// 0.0f, 0.0f,
			// 0.0f, 1.0f
			//};
			//tsize = sizeof(textures) / sizeof(textures[0]);
			model->SetTexture(data, width, height, nrChannels, ptextures, tsize);
		}
		stbi_image_free(data);
	}

}