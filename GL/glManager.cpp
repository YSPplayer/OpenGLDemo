#include "glManager.h"
#include "shaderTag.h"
#include <iostream>
#include "../Util/util.h"
#include "../cwindow.h"
namespace GL {
	using namespace Tool;
	using namespace Window;
	float GlManager::aspectUnit = 0.0f;
	GlManager::GlManager() {
		cmaera = new Camera;
		lightControl = new LightControl;
		aspectUnit = 0.0f;
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

	/// <summary>
	/// ͨ��x3p��������ģ��
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="xoffset"></param>
	/// <param name="yoffset"></param>
	/// <param name="zpointData"></param>
	/// <returns></returns>
	bool GlManager::CreateX3pModel(unsigned int width, unsigned int height, float xoffset, float yoffset, float* zpointData, float minZ, float maxZ) {
		float* pvertices = nullptr;
		unsigned int* pindices = nullptr;
		float* ptextures = nullptr;
		int vsize = 0;
		int isize = 0;
		int tsize = 0;
		glm::vec3 centerPos;
		float lightMax = CreateX3pData(width - 1, height - 1, xoffset, yoffset, minZ, maxZ, zpointData, &pvertices, &pindices, &ptextures, &vsize, &isize, &tsize, centerPos);
		Model* model = new Model;
		std::string vShader;
		std::string cShader;
		Util::LoadShader("Shader/model.vs", "Shader/model.fs", vShader, cShader);
		bool success = model->CreateModel(vShader, cShader, false, pvertices, vsize, pindices, isize);
		CreateModelTexture("", model, ptextures, tsize);//��ʼ���������
		model->CalculateVertexNormals();//���㷨��
		model->SetModelCenterPoisition(glm::vec3(centerPos.x, centerPos.y, 0.0f));
		cmaera->SetModelCenterPoisition(glm::vec3(centerPos.x, centerPos.y, 0.0f));
		cmaera->ReSetPoisition();
		lightControl->lightPos = glm::vec3(centerPos.x, centerPos.y + (lightMax / 2.0f) + 1.0f, centerPos.z);
		//����Ĭ�ϵļ��Ǻͷ�λ��
		const glm::vec3& spherical = Util::CartesianToSpherical(centerPos, lightControl->lightPos);
		CWindow::data.theta = spherical.y;
		CWindow::data.phi = spherical.z;
		if (!success) return false;
		models.push_back(model);
		return true;
	}

	/// <summary>
	/// ����һ����׼�����ģ��
	/// </summary>
	/// <param name="udata"></param>
	bool GlManager::CreateRandomModel(const UData& udata) {
		float* pvertices = nullptr;
		unsigned int* pindices = nullptr;
		float* ptextures = nullptr;
		int vsize = 0;
		int isize = 0;
		int tsize = 0;
		glm::vec3 centerPos;
		CreateRandomData(udata.modelWidth, udata.modelHeight, udata.modelXOffset, udata.modelYOffset, 0.0f, 0.0f, nullptr, udata.modelRandomZ, udata.modelRandomRange, &pvertices, &pindices, &ptextures, &vsize, &isize, &tsize, centerPos);
		Model* model = new Model;
		std::string vShader;
		std::string cShader;
		Util::LoadShader("Shader/model.vs", "Shader/model.fs", vShader, cShader);
		bool success = model->CreateModel(vShader, cShader, false, pvertices, vsize, pindices, isize);
		CreateModelTexture("", model, ptextures, tsize);//��ʼ���������
		model->CalculateVertexNormals();//���㷨��
		model->SetModelCenterPoisition(glm::vec3(centerPos.x, centerPos.y, 0.0f));
		cmaera->SetModelCenterPoisition(glm::vec3(centerPos.x, centerPos.y, 0.0f));
		cmaera->ReSetPoisition();
		int max = udata.modelWidth > udata.modelHeight ? udata.modelWidth : udata.modelHeight;
		lightControl->lightPos = glm::vec3(centerPos.x, centerPos.y + (max / 2.0f) + 0.1f, centerPos.z);
		if (!success) return false;
		models.push_back(model);
		return true;
	}

	bool GlManager::Init(Param* args) {
		lightControl->EnableLightModel();//���ù���ģ��
		Model* model = new Model;
		//������Դ������
		//float vertices[] = {
		//	-0.5f, -0.5f, -0.5f, // Vertex 0
		//	 0.5f, -0.5f, -0.5f, // Vertex 1
		//	 0.5f,  0.5f, -0.5f, // Vertex 2
		//	-0.5f,  0.5f, -0.5f, // Vertex 3
		//	-0.5f, -0.5f,  0.5f, // Vertex 4
		//	 0.5f, -0.5f,  0.5f, // Vertex 5
		//	 0.5f,  0.5f,  0.5f, // Vertex 6
		//	-0.5f,  0.5f,  0.5f  // Vertex 7
		//};
		float vertices[] = {
	-0.5f, -0.5f, -0.5f,  // Vertex 0
	 0.5f, -0.5f, -0.5f,  // Vertex 1
	 0.5f,  0.5f, -0.5f,  // Vertex 2
	 0.5f,  0.5f, -0.5f,  // Vertex 3
	-0.5f,  0.5f, -0.5f,  // Vertex 4
	-0.5f, -0.5f, -0.5f,  // Vertex 5

	-0.5f, -0.5f,  0.5f,  // Vertex 6
	 0.5f, -0.5f,  0.5f,  // Vertex 7
	 0.5f,  0.5f,  0.5f,  // Vertex 8
	 0.5f,  0.5f,  0.5f,  // Vertex 9
	-0.5f,  0.5f,  0.5f,  // Vertex 10
	-0.5f, -0.5f,  0.5f,  // Vertex 11

	-0.5f,  0.5f,  0.5f,  // Vertex 12
	-0.5f,  0.5f, -0.5f,  // Vertex 13
	-0.5f, -0.5f, -0.5f,  // Vertex 14
	-0.5f, -0.5f, -0.5f,  // Vertex 15
	-0.5f, -0.5f,  0.5f,  // Vertex 16
	-0.5f,  0.5f,  0.5f,  // Vertex 17

	 0.5f,  0.5f,  0.5f,  // Vertex 18
	 0.5f,  0.5f, -0.5f,  // Vertex 19
	 0.5f, -0.5f, -0.5f,  // Vertex 20
	 0.5f, -0.5f, -0.5f,  // Vertex 21
	 0.5f, -0.5f,  0.5f,  // Vertex 22
	 0.5f,  0.5f,  0.5f,  // Vertex 23

	-0.5f, -0.5f, -0.5f,  // Vertex 24
	 0.5f, -0.5f, -0.5f,  // Vertex 25
	 0.5f, -0.5f,  0.5f,  // Vertex 26
	 0.5f, -0.5f,  0.5f,  // Vertex 27
	-0.5f, -0.5f,  0.5f,  // Vertex 28
	-0.5f, -0.5f, -0.5f,  // Vertex 29

	-0.5f,  0.5f, -0.5f,  // Vertex 30
	 0.5f,  0.5f, -0.5f,  // Vertex 31
	 0.5f,  0.5f,  0.5f,  // Vertex 32
	 0.5f,  0.5f,  0.5f,  // Vertex 33
	-0.5f,  0.5f,  0.5f,  // Vertex 34
	-0.5f,  0.5f, -0.5f   // Vertex 35
		};
		int vsize = sizeof(vertices) / sizeof(vertices[0]);
		//unsigned int indices[] = {
		//	0, 1, 2, 0, 2, 3,// Front face
		//	4, 5, 6, 4, 6, 7,// Back face
		//	4, 7, 3, 4, 3, 0,// Left face
		//	1, 5, 6, 1, 6, 2,// Right face
		//	0, 1, 5, 0, 5, 4,// Bottom face
		//	3, 2, 6, 3, 6, 7// Top face
		//};
		unsigned int indices[] = {
			 0,  2,  1,  // Front face
			 5,  4,  3,

			 6,  7,  8,  // Back face
			 9, 10, 11,

			12, 13, 14,  // Left face
			15, 16, 17,

			18, 20, 19,  // Right face
			23, 22, 21,

			24, 25, 26,  // Bottom face
			27, 28, 29,

			30, 32, 31,  // Top face
			35, 34, 33
		};
		int isize = sizeof(indices) / sizeof(indices[0]);
		std::string vShader;
		std::string cShader;
		Util::LoadShader("Shader/model.vs", "Shader/model.fs", vShader, cShader);
		//vertexShader, colorShader
		bool success = model->CreateModel(vShader, cShader, true, vertices, vsize, indices, isize);
		model->CalculateVertexNormals();
		models.push_back(model);
		return true;
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
	//	float* pvertices = nullptr;
	//	unsigned int* pindices = nullptr;
	//	float* ptextures = nullptr;
	//	int vsize = 0;
	//	int isize = 0;
	//	int tsize = 0;
	//	float centerX = 0.0f;
	//	float centerY = 0.0f;
	//	if (args) {
	//		CreateRandomData(args->w, args->h, args->x, args->y, args->random == 1,10.0f,&pvertices, &pindices,&ptextures, &vsize, &isize,&tsize,&centerX, &centerY);
	//	}
	//	else {
	//		/*
	//		�������ֵ 5000 * 5000 2GB 400 580 
	//		*/ 
	//		CreateRandomData(1, 1, 1, 1, false, 10.0f, &pvertices, &pindices, &ptextures, &vsize, &isize, &tsize, &centerX, &centerY);
	//	}
	//	/*float vertices[] = {
	//		 0.5f,  0.5f, 0.0f,
	//		0.5f, -0.5f, 0.0f,
	//		-0.5f, -0.5f, 0.0f,
	//		-0.5f,  0.5f, 0.0f
	//	};

	//	vsize = sizeof(vertices) / sizeof(vertices[0]);

	//	unsigned int indices[] = {
	//		 0, 1, 3,
	//		 1, 2, 3
	//	};

	//	isize = sizeof(indices) / sizeof(indices[0]);*/
	//	Model* model = new Model;
	//	bool success = model->CreateModel(vertexShader,colorShader, false, pvertices, vsize, pindices,isize);
	//	CreateModelTexture("", model,ptextures,tsize);//��ʼ���������
	//	delete[] ptextures;
	//	model->SetModelCenterPoisition(glm::vec3(centerX, centerY, 0.0f));
	//	cmaera->SetModelCenterPoisition(glm::vec3(centerX, centerY, 0.0f));
	//	cmaera->ReSetPoisition();
	//	if (!success) return false;
	//	models.push_back(model);
	///*	models.push_back(model->CopyModel(success));*/
	//	lightControl->EnableLightModel();//���ù���ģ��
	//	return true;
	}

	void GlManager::Render(Data& data) {
		if (models.size() <= 0) return;
		data.cullBackFace ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);//�Ƿ�ʹ�����޳�����
		if (data.reset) {
			data.aspect = DEFAULT_ASPECT;//����Ĭ�ϵ��ӿ����ű���
			data.yaw = -90.0f;//������ת�����
			data.pitch = 0.0f; //������ת�����
		}
		const glm::mat4& view = data.reset ? cmaera->ReSetPoisition() : cmaera->UpdatePoisition(data);
		const glm::mat4& projection = cmaera->UpdateProjection(data);
		for (int i = 0; i < models.size(); ++i) {
			Model* model = models[i];
			if (model->TEXTURE) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, model->TEXTURE);
			}
			if (model->SPECULAR_TEXTURE) {
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, model->SPECULAR_TEXTURE);
			}
			Shader* shader = model->GetShader();
			Material& material = model->material;
			const glm::mat4& mposition = data.reset ? model->ReSetPoisition() : model->UpdatePoisition(data);
			shader->UseShader();
			//Z��߶�
		/*	shader->SetShaderFloat(data.zFactor, "zFactor");*/

			//�ӽ�
			shader->SetShaderMat4(view, "view");
			//ģ��
			shader->SetShaderMat4(mposition, "model");
			//���㷨�߾��󣬼���ģ�Ͳ�����仯ʱͬ�����ߵ�λ��
			const glm::mat3& normalMatrix = glm::transpose(glm::inverse(glm::mat3(mposition)));
			//���߾���
			shader->SetShaderMat3(normalMatrix, "normalMatrix");
			shader->SetShaderMat4(projection, "projection");
			shader->SetShaderVec3(cmaera->GetCameraPos(), "viewPos");

			//ģ����ɫ
			shader->SetShaderVec3(glm::vec3(data.colors[1][0], data.colors[1][1], data.colors[1][2]), "defaultObjectColor");

			//ģ����ͼ
			shader->SetShaderBoolean(data.useTexture && model->HasTexture(), "useTexture");
			shader->SetShaderInt(0, "defaultTexture");
			shader->SetShaderInt(1, "specularTexture");

			//����
			glm::vec3 diffuseColor = glm::vec3(data.colors[2][0], data.colors[2][1], data.colors[2][2]) * glm::vec3(0.5f);
			glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
			shader->SetShaderBoolean(data.blinn, "blinn");//�Ƿ�����blinn��ɫ
			shader->SetShaderBoolean(data.gammaCorrection, "gamma");//�Ƿ�����٤��У��
			shader->SetShaderBoolean(data.useLight, "useLight");
			shader->SetShaderInt(data.lightType, "lightType");//�������
			shader->SetShaderVec3(ambientColor, "light.ambient");//�������շ���
			shader->SetShaderVec3(diffuseColor, "light.diffuse");//���������
			if (data.lightType == SPOT_LIGHT) { //�۹�
				shader->SetShaderVec3(cmaera->GetCameraPos(), "light.position");
				shader->SetShaderVec3(cmaera->GetCameraFront(), "light.direction");
				shader->SetShaderFloat(glm::cos(glm::radians(12.5f)), "light.cutOff");
				shader->SetShaderFloat(glm::cos(glm::radians(17.5f)), "light.outerCutOff");
			}
			else {
				shader->SetShaderVec3(lightControl->lightPos, "light.position");//��Դλ��
				shader->SetShaderVec3(glm::vec3(-0.2f, -1.0f, -0.3f), "light.direction");//ƽ�й�λ��
				shader->SetShaderFloat(0.0f, "light.cutOff");
			}

			shader->SetShaderVec3(glm::vec3(1.0f, 1.0f, 1.0f), "light.specular"); //���淴��
			shader->SetShaderFloat(1.0f, "light.constant");//�����Գ���1
			shader->SetShaderFloat(0.09f, "light.linear");//�����Գ���2
			shader->SetShaderFloat(0.032f, "light.quadratic");//�����Գ���3

			shader->SetShaderVec3(material.ambient, "material.ambient");//ģ�͵Ļ�����
			shader->SetShaderVec3(material.diffuse, "material.diffuse");//ģ�͵�������
			shader->SetShaderVec3(material.specular, "material.specular");//ģ�͵ľ��淴��
			shader->SetShaderFloat(material.shininess, "material.shininess");//ģ�͹��ɢ��뾶

			model->Render(data);
		}
		//���Ƶƹ�ģ��
		if (data.showLightMode) {
			Model* lightModel = lightControl->lightModel;
			lightControl->lightModelPos = glm::translate(glm::mat4(1.0f), lightControl->lightPos);//���¹�Դλ��
			lightControl->lightModelPos = glm::scale(lightControl->lightModelPos, glm::vec3(0.2f));
			Shader* shader = lightModel->GetShader();
			const glm::mat4& mposition = data.reset ? lightModel->ReSetPoisition() : lightModel->UpdatePoisition(data);
			shader->UseShader();
			shader->SetShaderMat4(view, "view");
			shader->SetShaderMat4(lightControl->lightModelPos, "model");//�ƹ�ģ�͹̶�һ��λ��
			shader->SetShaderMat4(projection, "projection");
			lightModel->Render(data);
		}
	}

	/// <summary>
	/// ��������������ݣ���ģ�����ǵĿ��ӻ����ݣ�offset�ǵ�λ֮��Ĵ�С
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="vertices"></param>
	/// <param name="indices"></param>
	float GlManager::CreateRandomData(unsigned int width, unsigned int height, float xoffset, float yoffset, float minZ, float maxZ, float* pointsZ, bool random, float randomRange, float** vertices, unsigned int** indices, float** textures, int* vsize, int* isize,
		int* tsize, glm::vec3& centerPos) {
#undef min
#undef max
		//if (xoffset > MAX_X_OFFSET) xoffset = MAX_X_OFFSET;
		//if (yoffset > MAX_Y_OFFSET) yoffset = MAX_Y_OFFSET;
		int max = width > height ? width : height;//1225*1021  
		aspectUnit = Util::DivideByTenCount(max);
		int count = (height + 1) * (width + 1); //�������� = (ģ�ͳ��� + 1) * (ģ�Ϳ�� + 1)
		std::vector<Point> points;
		std::vector<Indice> vindices;
		float xSum = 0.0f;
		float ySum = 0.0f;
		float zSum = 0.0f;
		int tindex = 0;
		float x = 0;
		float y = 0;
		int pindex = 0;
		int zindex = 0;
		float maxX = width * xoffset; //�����Ƿ�����ϡ���㷨��ģ�͵����ճ����ֲ��䣬����ϡ��ʱ������Ҫ�������ģ�͵ĵ�λ����
		float maxY = height * yoffset;
		float absMax = maxX > maxY ? maxX : maxY;
		int step = (count > 2000000) ? 2 : 1;  // ʹ��ϡ�軯�㷨
		if (!CWindow::data.sparsePoint) step = 1;//δ���õ���ϡ��
		int pointWidth = width + 1;
		int pointHeight = height + 1;
		if (step == 2) {
			//��ȡ��ʹ��ϡ�ͻ��㷨֮��ĵ��Ƴ�������
			bool xborder = false;
			bool yborder = false;
			pointWidth = Util::GetSparseSize(pointWidth, step, xborder);
			pointHeight = Util::GetSparseSize(pointHeight, step, yborder);
			*tsize = pointWidth * pointHeight * 2;
			*textures = new float[*tsize];
			for (unsigned int j = 0; j < pointHeight; j++) {//�ȸ�ֵ����Ŀ�ȣ��ٸ�ֵ����ĸ߶�
				for (unsigned int i = 0; i < pointWidth; i++) {
					Point point;
					if (pointsZ) {
						int original_j = j * step;
						int original_i = i * step;
						if (yborder && j == pointHeight - 1) {
							original_j = height;  // �������һ��
						}
						if (xborder && i == pointWidth - 1) {
							original_i = width;  // �������һ��
						}
						zindex = original_j * (width + 1) + original_i;
						point.z = pointsZ[zindex];
					}
					else {
						point.z = random ? (Util::GetRandomFloat(-randomRange, randomRange) / 1000.0f) : 0.0f;//��һ��
					}
					//ʵ�ʵ��Ƴ��� = (���� * (ƫ���� / ���ƫ����)) /��һ��ֵ
				/*	x = static_cast<float>(i) * (xoffset / MAX_X_OFFSET);
					y = static_cast<float>(j) * (yoffset / MAX_Y_OFFSET);*/
					//point.x = x / static_cast<float>(max);//��һ��
					//point.y = y / static_cast<float>(max);//��һ��
					if (xborder && i == pointWidth - 1) {
						point.x = (static_cast<float>(width) * xoffset) / absMax; //���߽����һ�����Ȳ�����ֵ
						(*textures)[tindex++] = static_cast<float>(width) / static_cast<float>(width);
					}
					else {
						point.x = (static_cast<float>(i * step) * xoffset) / absMax;
						(*textures)[tindex++] = static_cast<float>(i * step) / static_cast<float>(width);
					}
					if (yborder && j == pointHeight - 1) {
						point.y = (static_cast<float>(height) * yoffset) / absMax;
						(*textures)[tindex++] = static_cast<float>(height) / static_cast<float>(height);
					}
					else {
						point.y = (static_cast<float>(j * step) * yoffset) / absMax;
						(*textures)[tindex++] = static_cast<float>(j * step) / static_cast<float>(height);
					}


					points.push_back(point);
					if (i < pointWidth - 1 && j < pointHeight - 1) {
						/*
						ע������2�ε������εĶ��������Ļ���˳��Ҫ����һ�£�Ҫô����˳ʱ�룬
						Ҫô������ʱ�룬������㷨�߻�������������������
						OpenGL��Ҫ������ʱ�����ַ�����������Ҫʹ����ʱ�룬��������Ĺ��յ�����ģʽ�ļ���
						*/
						//vindices.push_back({ i + (pointWidth - 1 + 1) * j, i + 1 + (pointWidth - 1 + 1) * j, i + (pointWidth - 1 + 1) * (j + 1) });
						//vindices.push_back({ (i + 1) + (pointWidth - 1 + 1) * j,  i + 1 + (pointWidth - 1 + 1) * (j + 1),i + (pointWidth - 1 + 1) * (j + 1) });
						vindices.push_back({ i + pointWidth * j, i + 1 + pointWidth * j, i + pointWidth * (j + 1) });
						vindices.push_back({ i + 1 + pointWidth * j, i + 1 + pointWidth * (j + 1), i + pointWidth * (j + 1) });
					}
				}
			}
		}
		else {
			*tsize = count * 2;
			*textures = new float[*tsize];
			for (unsigned int j = 0; j < pointHeight; j++) {//�ȸ�ֵ����Ŀ�ȣ��ٸ�ֵ����ĸ߶�
				for (unsigned int i = 0; i < pointWidth; i++) {
					Point point;
					if (pointsZ) {
						point.z = pointsZ[zindex++];
					}
					else {
						point.z = random ? (Util::GetRandomFloat(-randomRange, randomRange) / 1000.0f) : 0.0f;//��һ��
					}
					//ʵ�ʵ��Ƴ��� = (���� * (ƫ���� / ���ƫ����)) /��һ��ֵ
					/*	x = static_cast<float>(i) * (xoffset / MAX_X_OFFSET);
					y = static_cast<float>(j) * (yoffset / MAX_Y_OFFSET);*/
					//point.x = x / static_cast<float>(max);//��һ��
					//point.y = y / static_cast<float>(max);//��һ��
					point.x = (static_cast<float>(i) * xoffset) / absMax;
					point.y = (static_cast<float>(j) * yoffset) / absMax;
					(*textures)[tindex++] = static_cast<float>(i) / static_cast<float>(width);
					(*textures)[tindex++] = static_cast<float>(j) / static_cast<float>(height);
					points.push_back(point);
					if (i < width && j < height) {
						/*
						ע������2�ε������εĶ��������Ļ���˳��Ҫ����һ�£�Ҫô����˳ʱ�룬
						Ҫô������ʱ�룬������㷨�߻�������������������
						OpenGL��Ҫ������ʱ�����ַ�����������Ҫʹ����ʱ�룬��������Ĺ��յ�����ģʽ�ļ���
						*/
						vindices.push_back({ i + (width + 1) * j, i + 1 + (width + 1) * j, i + (width + 1) * (j + 1) });
						vindices.push_back({ (i + 1) + (width + 1) * j,  i + 1 + (width + 1) * (j + 1),i + (width + 1) * (j + 1) });
					}
				}
			}
		}
		*vsize = points.size() * 3;
		*vertices = new float[*vsize];
		float lightMax = 0.0f;//��Ϊ��һ����ԭ������ֵ�ķ�Χ��0-1֮��
		for (int i = 0; i < points.size(); ++i) {
			Point& point = points[i];
			(*vertices)[i * 3 + 0] = point.x;
			(*vertices)[i * 3 + 1] = point.y;
			float pointZ = 0.0f;
			if (pointsZ) {
				pointZ = (point.z - minZ) / (maxZ - minZ) / 5.0f;
			}
			else {
				pointZ = point.z;
			}
			(*vertices)[i * 3 + 2] = pointZ;
			xSum += point.x;
			ySum += point.y;
			zSum += pointZ;
			if (lightMax < point.x)lightMax = point.x;
			if (lightMax < point.y) lightMax = point.y;
			if (lightMax < pointZ)lightMax = pointZ;
			//std::cout << " point.x:" << point.x << " " << " point.y:" << point.y << " " << " point.z:" << point.z << std::endl;
		}
		//��ȡ��ģ�͵���������λ��
		centerPos.x = xSum / points.size();
		centerPos.y = ySum / points.size();
		centerPos.z = zSum / points.size();
		/*for (int i = 0; i < *tsize; ++i) {
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
		return lightMax;
	}

	float GlManager::CreateX3pData(unsigned int _width, unsigned int _height, float xoffset, float yoffset, float minZ, float maxZ,
		float* pointsZ, float** vertices, unsigned int** indices, float** textures, int* vsize, int* isize, int* tsize, glm::vec3& centerPos) {
		return CreateRandomData(_width, _height, xoffset, yoffset, minZ, maxZ, pointsZ, false, 0.0f, vertices, indices, textures, vsize,
			isize, tsize, centerPos);
	}

	/// <summary>
	/// ��������
	/// </summary>
	/// <param name="path"></param>
	void GlManager::CreateModelTexture(const char* path, Model* model, float* ptextures, int tsize) {
		if (!model) {
			std::cout << "[GlManager::CreateModelTexture] model is nullptr." << std::endl;
			return;
		}
		int width, height, nrChannels;
		unsigned char* data = nullptr;
		unsigned char* specular_data = nullptr;
		bool success = Util::CvLoadImage(std::string(path), data, specular_data, CWindow::data.alpha, CWindow::data.beta,
			width, height, nrChannels);
		//���û��ͼƬ�����ǲ��������ͼ�����û���������飬���ǲ������������
		model->SetTexture(data, specular_data, width, height, nrChannels, ptextures, tsize, CWindow::data.gammaCorrection);
		Util::ReleasePointer(data, true);
		Util::ReleasePointer(specular_data, true);
	}

	/// <summary>
	/// �޸ľ�����ͼ
	/// </summary>
	/// <param name="alpha"></param>
	/// <param name="beta"></param>
	void GlManager::ChangeModelSpecularImage(Model* model, double alpha, int beta) {
		int width, height, nrChannels;
		unsigned char* specular_data = nullptr;
		bool success = Util::CvImageToSpecularImage(specular_data, alpha, beta, width,
			height, nrChannels);
		model->SetTexture(nullptr, specular_data, width, height, nrChannels, nullptr, 0, CWindow::data.gammaCorrection);
		Util::ReleasePointer(specular_data, true);
	}

}