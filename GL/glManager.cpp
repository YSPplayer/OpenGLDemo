#define STB_IMAGE_IMPLEMENTATION //让stb_image.h变为源文件
#include "glManager.h"
#include "shaderTag.h"
#include <iostream>
#include <stb_image.h>
#include "../Util/util.h"

namespace GL {
	using namespace Tool;
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
		float centerX = 0.0f;
		float centerY = 0.0f;
		CreateX3pData(width - 1, height - 1, xoffset, yoffset, minZ,maxZ, zpointData,&pvertices, &pindices, &ptextures, &vsize, &isize, &tsize, &centerX, &centerY);
		Model* model = new Model;
		std::string vShader;
		std::string cShader;
		Util::LoadShader("Shader/model.vs", "Shader/model.fs", vShader, cShader);
		bool success = model->CreateModel(vShader, cShader, false, pvertices, vsize, pindices, isize);
		CreateModelTexture("", model, ptextures, tsize);//初始化纹理对象
		model->CalculateVertexNormals();//计算法线
		model->SetModelCenterPoisition(glm::vec3(centerX, centerY, 0.0f));
		cmaera->SetModelCenterPoisition(glm::vec3(centerX, centerY, 0.0f));
		cmaera->ReSetPoisition();
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
		float centerX = 0.0f;
		float centerY = 0.0f;
		CreateRandomData(udata.modelWidth, udata.modelHeight, udata.modelXOffset, udata.modelYOffset,0.0f,0.0f,nullptr, udata.modelRandomZ, udata.modelRandomRange,&pvertices, &pindices, &ptextures, &vsize, &isize, &tsize, &centerX, &centerY);
		Model* model = new Model;
		std::string vShader;
		std::string cShader;
		Util::LoadShader("Shader/model.vs", "Shader/model.fs", vShader, cShader);
		bool success = model->CreateModel(vShader, cShader, false, pvertices, vsize, pindices, isize);
		CreateModelTexture("", model, ptextures, tsize);//初始化纹理对象
		model->CalculateVertexNormals();//计算法线
		model->SetModelCenterPoisition(glm::vec3(centerX, centerY, 0.0f));
		cmaera->SetModelCenterPoisition(glm::vec3(centerX, centerY, 0.0f));
		cmaera->ReSetPoisition();
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
			if(model->TEXTURE) glBindTexture(GL_TEXTURE_2D, model->TEXTURE);
			Shader* shader = model->GetShader();
			Material& material = model->material;
			const glm::mat4& mposition = data.reset ? model->ReSetPoisition() : model->UpdatePoisition(data);
			shader->UseShader();
			shader->SetShaderMat4(view, "view");
			shader->SetShaderMat4(mposition, "model");
			//计算法线矩阵，兼容模型不规则变化时同步法线的位置
			const glm::mat3& normalMatrix = glm::transpose(glm::inverse(glm::mat3(mposition)));
			shader->SetShaderMat3(normalMatrix,"normalMatrix");
			glm::vec3 diffuseColor = glm::vec3(data.colors[2][0], data.colors[2][1], data.colors[2][2]) * glm::vec3(0.5f); 
			glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); 
			shader->SetShaderVec3(ambientColor,"light.ambient");
			shader->SetShaderVec3(diffuseColor,"light.diffuse");
			shader->SetShaderVec3(glm::vec3(1.0f, 1.0f, 1.0f),"light.specular"); //镜面反射
			shader->SetShaderVec3(lightControl->lightPos, "light.position");
			shader->SetShaderVec3(cmaera->GetCameraPos(), "viewPos");
	/*		shader->SetShaderVec3(glm::vec3(data.colors[1][0], data.colors[1][1], data.colors[1][2]), "defaultObjectColor");*/
			shader->SetShaderMat4(projection, "projection");
			shader->SetShaderBoolean(model->HasTexture(), "useTexture");
			shader->SetShaderBoolean(data.useLight, "useLight");

			shader->SetShaderVec3(material.ambient, "material.ambient");
			shader->SetShaderVec3(material.diffuse,"material.diffuse");
			shader->SetShaderVec3(material.specular,"material.specular");
			shader->SetShaderFloat(material.shininess,"material.shininess");
			model->Render(data);
		}
		//绘制灯光模型
		Model* lightModel = lightControl->lightModel;
		Shader* shader = lightModel->GetShader();
		const glm::mat4& mposition = data.reset ? lightModel->ReSetPoisition() : lightModel->UpdatePoisition(data);
		shader->UseShader();
		shader->SetShaderMat4(view, "view");
		shader->SetShaderMat4(lightControl->lightModelPos, "model");//灯光模型固定一个位置
		shader->SetShaderMat4(projection, "projection");
		lightModel->Render(data); 
	}

	/// <summary>
	/// 创建随机矩形数据，以模拟我们的可视化数据，offset是单位之间的大小
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="vertices"></param>
	/// <param name="indices"></param>
	void GlManager::CreateRandomData(unsigned int width, unsigned int height, float xoffset, float yoffset, float minZ, float maxZ, float* pointsZ,bool random, float randomRange, float** vertices, unsigned int** indices, float** textures, int* vsize, int* isize,
		int* tsize, float* centerX, float* centerY) {
#undef min
#undef max
		//if (xoffset > MAX_X_OFFSET) xoffset = MAX_X_OFFSET;
		//if (yoffset > MAX_Y_OFFSET) yoffset = MAX_Y_OFFSET;
		int max = width > height ? width : height;
		aspectUnit = Util::DivideByTenCount(max);
		int count = width * height;
		std::vector<Point> points;
		std::vector<Indice> vindices;
		*tsize = (height + 1) * (width + 1) * 2;
		*textures = new float[*tsize];
		float xSum = 0.0f;
		float ySum = 0.0f;
		int tindex = 0;
		float x = 0;
		float y = 0;
		int pindex = 0;
		int zindex = 0;
		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::lowest();
		float minY = std::numeric_limits<float>::max();
		float maxY = std::numeric_limits<float>::lowest();
		for (unsigned int j = 0; j < height + 1; ++j) {//先赋值横向的宽度，再赋值纵向的高度
			for (unsigned int i = 0; i < width + 1; ++i) {
				Point point;
				if (pointsZ) {
					point.z = pointsZ[zindex++];
				}
				else {
					point.z = random ? (Util::GetRandomFloat(-randomRange, randomRange) / 1000.0f) : 0.0f;;//归一化
				}
				//实际点云长度 = (索引 * (偏移量 / 最大偏移量)) /归一化值
			/*	x = static_cast<float>(i) * (xoffset / MAX_X_OFFSET);
				y = static_cast<float>(j) * (yoffset / MAX_Y_OFFSET);*/
				//point.x = x / static_cast<float>(max);//归一化
				//point.y = y / static_cast<float>(max);//归一化
				point.x = static_cast<float>(i) * xoffset;
				point.y = static_cast<float>(j) * yoffset;
				minX = std::min(minX, point.x);
				maxX = std::max(maxX, point.x);
				minY = std::min(minY, point.y);
				maxY = std::max(maxY, point.y);
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
		*vsize = points.size() * 3;
		*vertices = new float[*vsize];
		int absMax = maxX - minX > maxY - minY ? maxX - minX : maxY - minY;
		for (int i = 0; i < points.size(); ++i) {
			Point& point = points[i];
			float pointX = (point.x - minX) / absMax;
			float pointY = (point.y - minY) / absMax;
			(*vertices)[i * 3 + 0] = pointX;
			(*vertices)[i * 3 + 1] = pointY;
			float pointZ = 0.0f;
			if (pointsZ) {
				pointZ = (point.z - minZ) / (maxZ - minZ) / 5.0f;
			}
			else {
				pointZ = point.z;
			}
			(*vertices)[i * 3 + 2] = pointZ;
			xSum += pointX;
			ySum += pointY;
			//std::cout << " point.x:" << point.x << " " << " point.y:" << point.y << " " << " point.z:" << point.z << std::endl;
		}
		//获取到模型的中心坐标位置
		*centerX = xSum / points.size();
		*centerY = ySum / points.size();
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
	}

	void GlManager::CreateX3pData(unsigned int _width, unsigned int _height, float xoffset, float yoffset, float minZ, float maxZ, 
		float* pointsZ,float** vertices, unsigned int** indices, float** textures, int* vsize, int* isize, int* tsize, float* centerX, float* centerY) {
		CreateRandomData(_width,_height, xoffset,yoffset,minZ,maxZ, pointsZ,false,0.0f,  vertices, indices, textures, vsize,
			isize, tsize, centerX, centerY);
		return;
//		#undef min
//#undef max
//		//先生成点云数据
//		std::vector<Point> points;
//		std::vector<Indice> vindices;
//		points.resize(_width * _height);//2442 2042 4986564
//		int index = 0;
//		// 初始化最大最小值变量
//		float minX = std::numeric_limits<float>::max();
//		float maxX = std::numeric_limits<float>::lowest();
//		float minY = std::numeric_limits<float>::max();
//		float maxY = std::numeric_limits<float>::lowest();
//		float minZ = std::numeric_limits<float>::max();
//		float maxZ = std::numeric_limits<float>::lowest();
//		unsigned int oheight = _height - 1;
//		unsigned int owidth = _width - 1;
//		for (int j = 0; j < _height; ++j) {
//			float realY = static_cast<float>(j) * yoffset;
//			for (int i = 0; i < _width; ++i) {
//				float realX = static_cast<float>(i) * xoffset;
//				points.at(index) = { realX,realY, pointsZ[index] };	
//				minX = std::min(minX, realX);
//				maxX = std::max(maxX, realX);
//				minY = std::min(minY, realY);
//				maxY = std::max(maxY, realY);
//				minZ = std::min(minZ, pointsZ[index]);
//				maxZ = std::max(maxZ, pointsZ[index]);
//				++index;
//				if (i < owidth - 1 && j < owidth - 1) {
//					vindices.push_back({ i + (owidth + 1) * j, i + 1 + (owidth + 1) * j, i + (owidth + 1) * (j + 1) });
//					vindices.push_back({ (i + 1) + (owidth + 1) * j,  i + 1 + (owidth + 1) * (j + 1),i + (owidth + 1) * (j + 1) });
//				}
//			}
//		}
//		*vsize = points.size() * 3;
//		*vertices = new float[*vsize];
//		Point center = { 0.0f, 0.0f, 0.0f };
//		for (int i = 0; i < points.size(); ++i) {
//			Point& point = points[i];
//			float pointX = (point.x - minX) / (maxX - minX);
//			float pointY = (point.y - minY) / (maxY - minY);
//			float pointZ = (point.z - minZ) / (maxZ - minZ);
//			(*vertices)[i * 3 + 0] = pointX; //归一化
//			(*vertices)[i * 3 + 1] = pointY;
//			(*vertices)[i * 3 + 2] = pointZ;
//			center.x += pointX;
//			center.y += pointY;
//			center.z += pointZ;
//		}
//
//		*centerX = center.x / points.size();
//		*centerY = center.y / points.size();
//		*isize = vindices.size() * 3;
//		*indices = new unsigned int[vindices.size() * 3];
//		for (int i = 0; i < vindices.size(); ++i) {
//			Indice& indice = vindices[i];
//			(*indices)[i * 3 + 0] = indice.index1;
//			(*indices)[i * 3 + 1] = indice.index2;
//			(*indices)[i * 3 + 2] = indice.index3;
//		}
	}

	/// <summary>
	/// 创建纹理
	/// </summary>
	/// <param name="path"></param>
	void GlManager::CreateModelTexture(const char* path,Model* model,float* ptextures, int tsize) {
		if (!model) {
			std::cout << "[GlManager::CreateModelTexture] model is nullptr." << std::endl;
			return;
		} 
		int width, height, nrChannels;
		unsigned char* data = nullptr;
		//stbi_set_flip_vertically_on_load(true);//如果图片读取颠倒，修改这个参数
		// = stbi_load(path, &width, &height, &nrChannels, 0);
		bool success = Util::CvLoadImage(std::string(path),data, width, height, nrChannels);
		//float textures[] = {
			// 1.0f, 1.0f,
			// 1.0f, 0.0f,
			// 0.0f, 0.0f,
			// 0.0f, 1.0f
			//};
			//tsize = sizeof(textures) / sizeof(textures[0]);
		//如果没有图片，我们不会存在贴图，如果没有纹理数组，我们不创建纹理对象
		model->SetTexture(data, width, height, nrChannels, ptextures, tsize);
		stbi_image_free(data);
	}

}