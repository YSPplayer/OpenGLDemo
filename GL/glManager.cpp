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
	/// 通过x3p数据生成模型
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
		CreateModelTexture("", model, ptextures, tsize);//初始化纹理对象
		model->CalculateVertexNormals();//计算法线
		model->SetModelCenterPoisition(glm::vec3(centerPos.x, centerPos.y, 0.0f));
		cmaera->SetModelCenterPoisition(glm::vec3(centerPos.x, centerPos.y, 0.0f));
		cmaera->ReSetPoisition();
		lightControl->lightPos = glm::vec3(centerPos.x, centerPos.y + (lightMax / 2.0f) + 1.0f, centerPos.z);
		//设置默认的极角和方位角
		const glm::vec3& spherical = Util::CartesianToSpherical(centerPos, lightControl->lightPos);
		CWindow::data.theta = spherical.y;
		CWindow::data.phi = spherical.z;
		if (!success) return false;
		models.push_back(model);
		return true;
	}

	/// <summary>
	/// 创建一个标准化随机模型
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
		CreateModelTexture("", model, ptextures, tsize);//初始化纹理对象
		model->CalculateVertexNormals();//计算法线
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
		lightControl->EnableLightModel();//启用光照模型
		Model* model = new Model;
		//创建光源正方体
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
	//		长宽最大值 5000 * 5000 2GB 400 580 
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
	//	CreateModelTexture("", model,ptextures,tsize);//初始化纹理对象
	//	delete[] ptextures;
	//	model->SetModelCenterPoisition(glm::vec3(centerX, centerY, 0.0f));
	//	cmaera->SetModelCenterPoisition(glm::vec3(centerX, centerY, 0.0f));
	//	cmaera->ReSetPoisition();
	//	if (!success) return false;
	//	models.push_back(model);
	///*	models.push_back(model->CopyModel(success));*/
	//	lightControl->EnableLightModel();//启用光照模型
	//	return true;
	}

	void GlManager::Render(Data& data) {
		if (models.size() <= 0) return;
		data.cullBackFace ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);//是否使用面剔除功能
		if (data.reset) {
			data.aspect = DEFAULT_ASPECT;//更新默认的视口缩放比例
			data.yaw = -90.0f;//更新旋转轴参数
			data.pitch = 0.0f; //更新旋转轴参数
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
			//Z轴高度
		/*	shader->SetShaderFloat(data.zFactor, "zFactor");*/

			//视角
			shader->SetShaderMat4(view, "view");
			//模型
			shader->SetShaderMat4(mposition, "model");
			//计算法线矩阵，兼容模型不规则变化时同步法线的位置
			const glm::mat3& normalMatrix = glm::transpose(glm::inverse(glm::mat3(mposition)));
			//法线矩阵
			shader->SetShaderMat3(normalMatrix, "normalMatrix");
			shader->SetShaderMat4(projection, "projection");
			shader->SetShaderVec3(cmaera->GetCameraPos(), "viewPos");

			//模型颜色
			shader->SetShaderVec3(glm::vec3(data.colors[1][0], data.colors[1][1], data.colors[1][2]), "defaultObjectColor");

			//模型贴图
			shader->SetShaderBoolean(data.useTexture && model->HasTexture(), "useTexture");
			shader->SetShaderInt(0, "defaultTexture");
			shader->SetShaderInt(1, "specularTexture");

			//光照
			glm::vec3 diffuseColor = glm::vec3(data.colors[2][0], data.colors[2][1], data.colors[2][2]) * glm::vec3(0.5f);
			glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
			shader->SetShaderBoolean(data.blinn, "blinn");//是否启用blinn着色
			shader->SetShaderBoolean(data.gammaCorrection, "gamma");//是否启用伽马校正
			shader->SetShaderBoolean(data.useLight, "useLight");
			shader->SetShaderInt(data.lightType, "lightType");//光照类别
			shader->SetShaderVec3(ambientColor, "light.ambient");//环境光照分量
			shader->SetShaderVec3(diffuseColor, "light.diffuse");//漫反射分量
			if (data.lightType == SPOT_LIGHT) { //聚光
				shader->SetShaderVec3(cmaera->GetCameraPos(), "light.position");
				shader->SetShaderVec3(cmaera->GetCameraFront(), "light.direction");
				shader->SetShaderFloat(glm::cos(glm::radians(12.5f)), "light.cutOff");
				shader->SetShaderFloat(glm::cos(glm::radians(17.5f)), "light.outerCutOff");
			}
			else {
				shader->SetShaderVec3(lightControl->lightPos, "light.position");//光源位置
				shader->SetShaderVec3(glm::vec3(-0.2f, -1.0f, -0.3f), "light.direction");//平行光位置
				shader->SetShaderFloat(0.0f, "light.cutOff");
			}

			shader->SetShaderVec3(glm::vec3(1.0f, 1.0f, 1.0f), "light.specular"); //镜面反射
			shader->SetShaderFloat(1.0f, "light.constant");//非线性常量1
			shader->SetShaderFloat(0.09f, "light.linear");//非线性常量2
			shader->SetShaderFloat(0.032f, "light.quadratic");//非线性常量3

			shader->SetShaderVec3(material.ambient, "material.ambient");//模型的环境光
			shader->SetShaderVec3(material.diffuse, "material.diffuse");//模型的漫反射
			shader->SetShaderVec3(material.specular, "material.specular");//模型的镜面反射
			shader->SetShaderFloat(material.shininess, "material.shininess");//模型光的散射半径

			model->Render(data);
		}
		//绘制灯光模型
		if (data.showLightMode) {
			Model* lightModel = lightControl->lightModel;
			lightControl->lightModelPos = glm::translate(glm::mat4(1.0f), lightControl->lightPos);//更新光源位置
			lightControl->lightModelPos = glm::scale(lightControl->lightModelPos, glm::vec3(0.2f));
			Shader* shader = lightModel->GetShader();
			const glm::mat4& mposition = data.reset ? lightModel->ReSetPoisition() : lightModel->UpdatePoisition(data);
			shader->UseShader();
			shader->SetShaderMat4(view, "view");
			shader->SetShaderMat4(lightControl->lightModelPos, "model");//灯光模型固定一个位置
			shader->SetShaderMat4(projection, "projection");
			lightModel->Render(data);
		}
	}

	/// <summary>
	/// 创建随机矩形数据，以模拟我们的可视化数据，offset是单位之间的大小
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
		int count = (height + 1) * (width + 1); //点云数量 = (模型长度 + 1) * (模型宽度 + 1)
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
		float maxX = width * xoffset; //不管是否运用稀疏算法，模型的最终长宽保持不变，点云稀疏时我们需要变更的是模型的单位增量
		float maxY = height * yoffset;
		float absMax = maxX > maxY ? maxX : maxY;
		int step = (count > 2000000) ? 2 : 1;  // 使用稀疏化算法
		if (!CWindow::data.sparsePoint) step = 1;//未启用点云稀疏
		int pointWidth = width + 1;
		int pointHeight = height + 1;
		if (step == 2) {
			//获取到使用稀释化算法之后的点云长宽数量
			bool xborder = false;
			bool yborder = false;
			pointWidth = Util::GetSparseSize(pointWidth, step, xborder);
			pointHeight = Util::GetSparseSize(pointHeight, step, yborder);
			*tsize = pointWidth * pointHeight * 2;
			*textures = new float[*tsize];
			for (unsigned int j = 0; j < pointHeight; j++) {//先赋值横向的宽度，再赋值纵向的高度
				for (unsigned int i = 0; i < pointWidth; i++) {
					Point point;
					if (pointsZ) {
						int original_j = j * step;
						int original_i = i * step;
						if (yborder && j == pointHeight - 1) {
							original_j = height;  // 保留最后一行
						}
						if (xborder && i == pointWidth - 1) {
							original_i = width;  // 保留最后一列
						}
						zindex = original_j * (width + 1) + original_i;
						point.z = pointsZ[zindex];
					}
					else {
						point.z = random ? (Util::GetRandomFloat(-randomRange, randomRange) / 1000.0f) : 0.0f;//归一化
					}
					//实际点云长度 = (索引 * (偏移量 / 最大偏移量)) /归一化值
				/*	x = static_cast<float>(i) * (xoffset / MAX_X_OFFSET);
					y = static_cast<float>(j) * (yoffset / MAX_Y_OFFSET);*/
					//point.x = x / static_cast<float>(max);//归一化
					//point.y = y / static_cast<float>(max);//归一化
					if (xborder && i == pointWidth - 1) {
						point.x = (static_cast<float>(width) * xoffset) / absMax; //带边界最后一个长度不再增值
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
						注意如下2次的三角形的顶点组成面的环绕顺序要保持一致，要么都是顺时针，
						要么都是逆时针，否则计算法线会出现零向量抵消的情况
						OpenGL的要求是逆时针右手法则，所以我们要使用逆时针，方便后续的光照等其他模式的计算
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
			for (unsigned int j = 0; j < pointHeight; j++) {//先赋值横向的宽度，再赋值纵向的高度
				for (unsigned int i = 0; i < pointWidth; i++) {
					Point point;
					if (pointsZ) {
						point.z = pointsZ[zindex++];
					}
					else {
						point.z = random ? (Util::GetRandomFloat(-randomRange, randomRange) / 1000.0f) : 0.0f;//归一化
					}
					//实际点云长度 = (索引 * (偏移量 / 最大偏移量)) /归一化值
					/*	x = static_cast<float>(i) * (xoffset / MAX_X_OFFSET);
					y = static_cast<float>(j) * (yoffset / MAX_Y_OFFSET);*/
					//point.x = x / static_cast<float>(max);//归一化
					//point.y = y / static_cast<float>(max);//归一化
					point.x = (static_cast<float>(i) * xoffset) / absMax;
					point.y = (static_cast<float>(j) * yoffset) / absMax;
					(*textures)[tindex++] = static_cast<float>(i) / static_cast<float>(width);
					(*textures)[tindex++] = static_cast<float>(j) / static_cast<float>(height);
					points.push_back(point);
					if (i < width && j < height) {
						/*
						注意如下2次的三角形的顶点组成面的环绕顺序要保持一致，要么都是顺时针，
						要么都是逆时针，否则计算法线会出现零向量抵消的情况
						OpenGL的要求是逆时针右手法则，所以我们要使用逆时针，方便后续的光照等其他模式的计算
						*/
						vindices.push_back({ i + (width + 1) * j, i + 1 + (width + 1) * j, i + (width + 1) * (j + 1) });
						vindices.push_back({ (i + 1) + (width + 1) * j,  i + 1 + (width + 1) * (j + 1),i + (width + 1) * (j + 1) });
					}
				}
			}
		}
		*vsize = points.size() * 3;
		*vertices = new float[*vsize];
		float lightMax = 0.0f;//因为归一化的原因，所以值的范围在0-1之间
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
		//获取到模型的中心坐标位置
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
	/// 创建纹理
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
		//如果没有图片，我们不会存在贴图，如果没有纹理数组，我们不创建纹理对象
		model->SetTexture(data, specular_data, width, height, nrChannels, ptextures, tsize, CWindow::data.gammaCorrection);
		Util::ReleasePointer(data, true);
		Util::ReleasePointer(specular_data, true);
	}

	/// <summary>
	/// 修改镜面贴图
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