#pragma once
#include "../GL/data.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <random>
#include <codecvt>
#include <Windows.h>
#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <glm/glm.hpp>
#include <tinyxml2/tinyxml2.h>
#include <opencv2/opencv.hpp>
#include <glm/gtc/constants.hpp>
namespace GL {
	namespace Tool {
        using namespace tinyxml2;

		class Util {
		public:
            template<typename T>
            static void ReleasePointer(T*& point, bool isArray = false) {
                if (point == nullptr) return;
                try {
                    isArray ? delete[] point : delete point;
                    point = nullptr;
                }
                catch (const std::exception& e) {
                    std::cout << "[Util::ReleasePointer]error:" << e.what();
                }
            }

            static void ReleasePointer(void**& point, const int& size) {
                if (point == nullptr) return;
                for (int i = 0; i < size; ++i) ReleasePointer(point[i]);
                ReleasePointer(point, true);
            }

			static float NormalizeAngle(float angle, float factor) {
				angle = fmod(angle, factor);
				if (angle < 0) {
					angle += factor;
				}
				return angle;
			}
            // 用于从配置中获取值的函数
            static std::string GetValueFromConfig(const std::map<std::string, std::string>& configDict,
                const std::string& section, const std::string& key) {
                std::string compositeKey = section + "." + key;  // 构建复合键
                auto it = configDict.find(compositeKey);
                if (it != configDict.end()) {
                    return it->second;  // 如果找到键，返回对应的值
                }
                return "";  // 否则返回默认值
            }
            static glm::vec3 GetStringValues(const std::string& str) {
                std::vector<float> numbers;
                std::stringstream ss(str);  // 创建一个字符串流对象
                std::string item;
                while (std::getline(ss, item, ',')) { 
                    numbers.push_back(std::stof(item)); 
                }
                return glm::vec3(numbers[0], numbers[1], numbers[2]);
            }
            static std::string GetValuesString(const glm::vec3& numbers) {
                std::ostringstream ss;
                for (size_t i = 0; i < 3; ++i) {
                    if (i > 0) {
                        ss << ","; // 添加逗号分隔符，除了第一个元素之外
                    }
                    ss << numbers[i]; // 将浮点数添加到流中
                }
                return ss.str(); // 返回构建的字符串
            }
            
            /*
            球面坐标系由三个参数定义：
            半径 radius(x)、方位角 theta(y)（绕 Z 轴的角度，从 X 轴正方向开始计算）
            ，以及极角 phi(z)（从 Z 轴正方向到点的角度）。
            */

            /// <summary>
            /// 球面坐标转笛卡尔坐标
            /// </summary>
            /// <param name="center"></param>
            /// <param name="spherical"></param>
            /// <returns></returns>
            static glm::vec3 SphericalToCartesian(const glm::vec3& center, const glm::vec3& spherical) {
                float x = center.x + spherical.x * sin(spherical.z) * cos(spherical.y);
                float y = center.y + spherical.x * sin(spherical.z) * sin(spherical.y);
                float z = center.z + spherical.x * cos(spherical.z);
                return { x, y, z };
            }

            /// <summary>
            /// 笛卡尔坐标转球面坐标
            /// </summary>
            /// <param name="center"></param>
            /// <param name="point"></param>
            /// <returns></returns>
            static glm::vec3 CartesianToSpherical(const glm::vec3& center, const glm::vec3& point) {
                float dx = point.x - center.x;
                float dy = point.y - center.y;
                float dz = point.z - center.z;
                float radius = sqrt(dx * dx + dy * dy + dz * dz);
                float phi = acos(dz / radius);
                float theta = atan2(dy, dx);
                return { radius, theta, phi };
            }

            /// <summary>
            /// 设置当前球面坐标
            /// </summary>
            /// <param name="center"></param>
            /// <param name="point"></param>
            /// <returns></returns>
            static glm::vec3 CalculateNewSphericalCoordinates(const glm::vec3& center, const glm::vec3& point, float theta, float phi) {
                const glm::vec3& spherical = CartesianToSpherical(center, point);
                // 根据给定的极角和方位角计算新的XYZ坐标
                float x = spherical.x * sin(phi) * cos(theta);
                float y = spherical.x * sin(phi) * sin(theta);
                float z = spherical.x * cos(phi);
                // 将结果转换为基于中心点的全局坐标
                glm::vec3 newPoint = center + glm::vec3(x, y, z);
                return newPoint;
            }



            /// <summary>
            /// 移动度数
            /// </summary>
            /// <param name="phiChange"></param>
            /// <param name="thetaChange"></param>
            /// <param name="currentSpherical"></param>
            /// <returns></returns>
            static glm::vec3 SetSpherical(float newPhi, float newTheta, const glm::vec3& currentSpherical) {
                // 限制 phi 在 0 到 π 之间
                newPhi = std::max(0.0f, std::min(glm::pi<float>(), newPhi));
                // 将 theta 规范到 -π 到 π 的范围
                if (newTheta > glm::pi<float>()) {
                    newTheta -= 2 * glm::pi<float>();
                }
                else if (newTheta < -glm::pi<float>()) {
                    newTheta += 2 * glm::pi<float>();
                }
                return { currentSpherical.x, newTheta, newPhi }; // radius 不变，新的 theta 和 phi

            }
            static bool SaveMaterial(Material& material, const std::wstring& name, bool completePath = false) {
                const std::wstring& path = completePath ? name : (Util::GetRootPath() + L"Material\\" + name);
                std::ofstream cfile; 
                cfile.open(path, std::ios::out | std::ios::trunc);//替换方式写入文本，不存在则创建
                if (!cfile.is_open()) {
                    return false;
                }
                //写入默认的文本流
                cfile << "[Material]\n";
                cfile << "Ambient=" + GetValuesString(material.ambient) + "\n";
                cfile << "Diffuse=" + GetValuesString(material.specular) + "\n";
                cfile << "Specular=" + GetValuesString(material.diffuse) + "\n";
                cfile << "Shininess=" + std::to_string(material.shininess) + "\n";
                cfile.close();
                return true;
            }

            static glm::vec3 RGBToDiffuse(const glm::vec3& color) {
                return glm::vec3(color[0] / 255.0f, color[1] / 255.0f, color[2] / 255.0f);
            }
            static glm::vec3 DiffuseToRGB(const glm::vec3& diffuse) {
                return glm::vec3(static_cast<int>(round(diffuse[0] * 255)), static_cast<int>(round(diffuse[1] * 255))
                , static_cast<int>(round(diffuse[2] * 255)));
            }
            static bool LoadMaterial(Material& material, const std::wstring& name, bool completePath = false) {
                std::map<std::string, std::string> configDict; 
                const std::wstring& path = completePath ? name : Util::GetRootPath() + L"Material\\" + name;
                std::ifstream file(path);
                if (!file.is_open()) {
                    return false; 
                }
                std::string line;
                std::string currentSection;
                while (std::getline(file, line)) {
                    if (line.empty() || line[0] == '#') continue; // 跳过空行和注释
                    line = std::string(line.begin(), std::remove(line.begin(), line.end(), ' ')); // 去除空格
                    if (line.front() == '[' && line.back() == ']') {
                        currentSection = line.substr(1, line.size() - 2); 
                    }
                    else if (!line.empty()) {
                        size_t equalSignIndex = line.find('=');
                        if (equalSignIndex != std::string::npos) {
                            std::string key = line.substr(0, equalSignIndex);
                            std::string value = line.substr(equalSignIndex + 1);
                            std::string compositeKey = currentSection + "." + key;
                            configDict[compositeKey] = value; 
                        }
                    }
                }
                file.close();
                std::string section = "Material";
                material.ambient = GetStringValues(GetValueFromConfig(configDict,section,"Ambient"));
                material.diffuse = GetStringValues(GetValueFromConfig(configDict, section, "Diffuse"));
                material.specular = GetStringValues(GetValueFromConfig(configDict, section, "Specular"));
                material.shininess = std::stof(GetValueFromConfig(configDict, section, "Shininess"));
                return true;
            }
            static bool CvImageToSpecularImage(unsigned char*& specular_data, double alpha, int beta, int& width, int& height, int& nrChannels) {
                cv::Mat adjustedImg; //创建镜面反射贴图
                img.convertTo(adjustedImg, -1, alpha, beta);
                specular_data = new unsigned char[adjustedImg.total() * adjustedImg.elemSize()];
                std::memcpy(specular_data, adjustedImg.data, adjustedImg.total() * adjustedImg.elemSize());
                // 更新宽度、高度和通道数
                width = img.cols;
                height = img.rows;
                nrChannels = img.channels();
                return true;
            }
            /// <summary>
            /// 计算切线和副切线数组
            /// </summary>
            /// <param name="vertices"></param>
            /// <param name="uvs"></param>
            /// <param name="normals"></param>
            /// <param name="vertexCount"></param>
            /// <param name="tangents"></param>
            /// <param name="bitangents"></param>
            static void CalculateTangentAndBitangent(const float* vertices, const float* uvs, const float* normals,
                int vertexCount, float* tangents, float* bitangents) {
                for (int i = 0; i < vertexCount / 3; i += 3) {
                    // 获取三角形的三个顶点
                    glm::vec3 vertex1(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);
                    glm::vec3 vertex2(vertices[(i + 1) * 3], vertices[(i + 1) * 3 + 1], vertices[(i + 1) * 3 + 2]);
                    glm::vec3 vertex3(vertices[(i + 2) * 3], vertices[(i + 2) * 3 + 1], vertices[(i + 2) * 3 + 2]);

                    // 获取三角形的三个UV坐标
                    glm::vec2 uv1(uvs[i * 2], uvs[i * 2 + 1]);
                    glm::vec2 uv2(uvs[(i + 1) * 2], uvs[(i + 1) * 2 + 1]);
                    glm::vec2 uv3(uvs[(i + 2) * 2], uvs[(i + 2) * 2 + 1]);

                    // 获取三角形的法线
                    glm::vec3 normal1(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]);

                    // 计算三角形的边和UV差异
                    glm::vec3 edge1 = vertex2 - vertex1;
                    glm::vec3 edge2 = vertex3 - vertex1;
                    glm::vec2 deltaUV1 = uv2 - uv1;
                    glm::vec2 deltaUV2 = uv3 - uv1;

                    // 计算f因子
                    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

                    // 计算切线和副切线
                    glm::vec3 tangent, bitangent;
                    tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
                    bitangent = f * (deltaUV1.x * edge2 - deltaUV2.x * edge1);

                    // 正交化切线
                    tangent = glm::normalize(tangent - normal1 * glm::dot(normal1, tangent));
                    tangent = glm::normalize(tangent);  // 确保切线是单位向量
                    // 将结果存储到数组中
                    tangents[i * 3] = tangent.x;
                    tangents[i * 3 + 1] = tangent.y;
                    tangents[i * 3 + 2] = tangent.z;

                    bitangents[i * 3] = bitangent.x;
                    bitangents[i * 3 + 1] = bitangent.y;
                    bitangents[i * 3 + 2] = bitangent.z;
                }
            }

            /// <summary>
            /// 将法线数组转换为法线贴图的函数
            /// </summary>
            /// <param name="normals"></param>
            /// <param name="width"></param>
            /// <param name="height"></param>
            /// <returns></returns>
            static cv::Mat ConvertNormalsToNormalMap(float* normals, int width, int height) {
                cv::Mat normalMap(height, width, CV_32FC3);
                for (int y = 0; y < height; ++y) {
                    for (int x = 0; x < width; ++x) {
                        int index = (y * width + x) * 3; // 计算法线数组的索引
                        cv::Vec3f normal(normals[index], normals[index + 1], normals[index + 2]);
                        // 将法线映射到[0, 1]范围
                        normal = (normal + cv::Vec3f(1.0f, 1.0f, 1.0f)) * 0.5f;
                        normalMap.at<cv::Vec3f>(y, x) = normal;
                    }
                } 
                cv::flip(normalMap, normalMap, 0);
                return normalMap; 
            }
            /// <summary>
            /// 加载图片
            /// </summary>
            /// <param name="path"></param>
            /// <returns></returns>
            static bool CvLoadImage(const std::string& path, unsigned char*& data, unsigned char*& specular_data, double alpha, int beta,int& width, int& height, int& nrChannels,bool textureFlip) {
                if (path == "") return false;
                // 读取图像
                img = cv::imread(path,cv::ImreadModes::IMREAD_UNCHANGED);
                // 检查图像是否成功加载
                if (img.empty()) {
                    std::cerr << "Failed to load image." << std::endl;
                    return false;
                }
                if (!img.isContinuous()) img = img.clone(); //不加这段转换图片类型会阻塞
                if (img.channels() == 1) {
                    //是灰度图，转换为RGB的灰度图
                    cv::cvtColor(img, img, cv::COLOR_GRAY2RGB);
                }
                else if (img.channels() == 3) {
                    // 将BGR转换为RGB
                    cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
                }
                // 将图像垂直翻转
                if(textureFlip) cv::flip(img, img, 0);
                // 确保图像是连续的，这对 OpenGL 处理很重要
                if (!img.isContinuous())img = img.clone();
                // 分配内存用于存储图像数据（需要在适当时候释放这块内存）
                data = new unsigned char[img.total() * img.elemSize()];
                std::memcpy(data, img.data, img.total() * img.elemSize());
                return CvImageToSpecularImage(specular_data,alpha,beta, width, height, nrChannels);
            }
            /// <summary>
            /// 加载配置文件
            /// </summary>
            /// <param name="data"></param>
            /// <param name="udata"></param>
            /// <returns></returns>
            static bool LoadConfig(Data& data, UData& udata) {
                tinyxml2::XMLDocument doc;
                char* pathstr = Util::WStringToChar((Util::GetRootPath() + L"Config\\config.xml"));
                XMLError eResult = doc.LoadFile(pathstr);
                delete[] pathstr;
                if (eResult != XML_SUCCESS) return false;
                XMLElement* root = doc.RootElement();
                XMLElement* model = root->FirstChildElement("Model");
                udata.modelWidth = std::stoi(model->FirstChildElement("Width")->GetText());
                udata.modelHeight = std::stoi(model->FirstChildElement("Height")->GetText());
                udata.modelXOffset = std::stod(model->FirstChildElement("XOffset")->GetText());
                udata.modelYOffset = std::stod(model->FirstChildElement("YOffset")->GetText());
                udata.modelRandomZ = std::string(model->FirstChildElement("ZRandom")->GetText()) == "1" ? true : false;
                udata.modelRandomRange = std::stod(model->FirstChildElement("ZRandomRange")->GetText());
                udata.selectedRadio = std::stoi(model->FirstChildElement("ShowMode")->GetText());
                data.drawMode = udata.selectedRadio;
                XMLElement* texture = root->FirstChildElement("Texture");
                auto text  = texture->FirstChildElement("Path")->GetText();
                std::string selectedFilePath = std::string(text ? text : "");
                strncpy_s(udata.pathBuf, selectedFilePath.c_str(), (sizeof(udata.pathBuf) / sizeof(udata.pathBuf)[0]) - 1);
                udata.pathBuf[(sizeof(udata.pathBuf) / sizeof(udata.pathBuf)[0]) - 1] = '\0'; // 确保字符串以空字符结尾
                XMLElement* color = root->FirstChildElement("Color");
                std::vector<std::vector<float>> values;
                for (int i = 0; i < 3; ++i) {
                    values.push_back(std::vector<float>());
                    std::string key = "";
                    if (i == 0) key = "Background";
                    else if (i == 1) key = "Model";
                    else  key = "Light";
                    std::istringstream ss(color->FirstChildElement(key.c_str())->GetText());
                    std::string valuestr;
                    while (std::getline(ss, valuestr, ',')) {
                        float value = std::stof(valuestr);
                        values[i].push_back(value);
                    }
                }
               
                float(*colors)[4] = data.colors;
                for (int i = 0; i < 3; ++i) {
                    colors[i][0] = values[i][0];
                    colors[i][1] = values[i][1];
                    colors[i][2] = values[i][2];
                    colors[i][3] = values[i][3];
                }
                XMLElement* system = root->FirstChildElement("System");
                data.cullBackFace = system->FirstChildElement("CullBack")->GetText() == "1" ? true : false;
                data.moveSpeedUnit = std::stof(system->FirstChildElement("CameraSpeed")->GetText());
                data.sensitivity = std::stof(system->FirstChildElement("CameraSensitivity")->GetText());
                return true;
            }


            /// <summary>
            /// 创建配置文件
            /// </summary>
            /// <param name="data"></param>
            /// <returns></returns>
            static bool CreateConfig(const Data& data,const UData& udata) {
                tinyxml2::XMLDocument doc;
                // 添加声明
                XMLDeclaration* decl = doc.NewDeclaration();
                doc.InsertFirstChild(decl);
                XMLElement* root = doc.NewElement("Root");
                doc.InsertEndChild(root);
                //model
                XMLElement* model = doc.NewElement("Model");
                root->InsertEndChild(model);
                XMLElement* width = doc.NewElement("Width");
                model->InsertEndChild(width);
                width->SetText(udata.modelWidth);
                XMLElement* height = doc.NewElement("Height");
                model->InsertEndChild(height);
                height->SetText(udata.modelHeight);
                XMLElement* xOffset = doc.NewElement("XOffset");
                model->InsertEndChild(xOffset);
                xOffset->SetText(udata.modelXOffset);
                XMLElement* yOffset = doc.NewElement("YOffset");
                model->InsertEndChild(yOffset);
                yOffset->SetText(udata.modelYOffset);
                XMLElement* zRandomRange = doc.NewElement("ZRandomRange");
                model->InsertEndChild(zRandomRange);
                zRandomRange->SetText(udata.modelRandomRange);
                XMLElement* zRandom = doc.NewElement("ZRandom");
                model->InsertEndChild(zRandom);
                zRandom->SetText(udata.modelRandomZ ? "1" : "0");
                XMLElement* showMode = doc.NewElement("ShowMode");
                model->InsertEndChild(showMode);
                showMode->SetText(udata.selectedRadio);
                //Texture
                XMLElement* texture = doc.NewElement("Texture");
                root->InsertEndChild(texture);
                XMLElement* path = doc.NewElement("Path");
                texture->InsertEndChild(path);
                path->SetText(udata.pathBuf);
                //Color
                const float(*colors)[4] = data.colors;
                XMLElement* color = doc.NewElement("Color");
                root->InsertEndChild(color);
                XMLElement* background = doc.NewElement("Background");
                color->InsertEndChild(background);
                background->SetText(GetColorString(colors[0]).c_str());
                XMLElement* cmodel = doc.NewElement("Model");
                color->InsertEndChild(cmodel);
                cmodel->SetText(GetColorString(colors[1]).c_str());
                XMLElement* light = doc.NewElement("Light");
                color->InsertEndChild(light);
                light->SetText(GetColorString(colors[2]).c_str());
                //System
                XMLElement* system = doc.NewElement("System");
                root->InsertEndChild(system);
                XMLElement* cullBack = doc.NewElement("CullBack");
                system->InsertEndChild(cullBack);
                cullBack->SetText(data.cullBackFace ? "1" : "0");
                XMLElement* cameraSpeed = doc.NewElement("CameraSpeed");
                system->InsertEndChild(cameraSpeed);
                cameraSpeed->SetText(data.moveSpeedUnit);
                XMLElement* cameraSensitivity = doc.NewElement("CameraSensitivity");
                system->InsertEndChild(cameraSensitivity);
                cameraSensitivity->SetText(data.sensitivity);
                char* pathstr = Util::WStringToChar((Util::GetRootPath() + L"Config\\config.xml"));
                XMLError eResult = doc.SaveFile(pathstr);
                delete[] pathstr;
                return eResult == XML_SUCCESS;
            }

            /// <summary>
            /// 计算面法线
            /// </summary>
            /// <param name="v1"></param>
            /// <param name="v2"></param>
            /// <param name="v3"></param>
            /// <returns></returns>
            static glm::vec3 CalculateFaceNormal(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
                glm::vec3 edge1 = v2 - v1;
                glm::vec3 edge2 = v3 - v1;
                glm::vec3 normal = glm::cross(edge1, edge2);
                return glm::normalize(normal); // 归一化法线向量
            }
            static float GetRandomFloat(float min, float max) {
                std::uniform_real_distribution<float> dis(min, max);
                return dis(gen);
            }
            /// <summary>
            /// 获取当前程序运行的主目录
            /// </summary>
            /// <returns></returns>
            inline static std::wstring GetRootPath() {
                return rootPath;
            }
            static std::wstring StringToWString(const std::string& str) {
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                return converter.from_bytes(str);
            }

            static std::string WStringToString(const std::wstring& wstr) {
                std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                return converter.to_bytes(wstr);
            }
            static char* WStringToChar(const std::wstring& wstr) {
                const std::string& str = WStringToString(wstr);
                char* charPtr = new char[str.length() + 1]; //char内存需要手动管理释放
                strcpy_s(charPtr, str.length() + 1, str.c_str());
                return charPtr;
            }
            
            static int GetSparseSize(int n, int step, bool& border) {
                border = false;
                int sparseWidth = (n + step - 1) / step;
                if ((n - 1) % step != 0) {
                    sparseWidth++;
                    border = true;//有边界
                }
                return sparseWidth;
            }
            
            /// <summary>
            /// 读取外部shader
            /// </summary>
            /// <param name="vertexPath"></param>
            /// <param name="colorPath"></param>
            /// <param name="vertexShader"></param>
            /// <param name="colorShader"></param>
            /// <returns></returns>
            static bool LoadShader(const char* vertexPath, const char* colorPath,std::string& vertexShader,std::string& colorShader) {
                std::ifstream vShaderFile;
                std::ifstream fShaderFile;
                vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                try {
                    vShaderFile.open(vertexPath);
                    fShaderFile.open(colorPath);
                    std::stringstream vShaderStream, fShaderStream;
                    vShaderStream << vShaderFile.rdbuf();
                    fShaderStream << fShaderFile.rdbuf();
                    vShaderFile.close();
                    fShaderFile.close();
                    vertexShader = vShaderStream.str();
                    colorShader = fShaderStream.str();
                }
                catch (std::ifstream::failure& e) {
                    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
                    return false;
                }
                return true;
            }

            /// <summary>
            /// 获取当前的时间
            /// </summary>
            /// <returns></returns>
            static std::wstring GetCurrentDateTime() {
                auto now = std::chrono::system_clock::now();
                std::time_t now_time = std::chrono::system_clock::to_time_t(now);
                std::tm now_tm;
                localtime_s(&now_tm, &now_time); // 使用线程安全的localtime_s
                wchar_t buffer[100];
                wcsftime(buffer, sizeof(buffer), L"%Y-%m-%d %H:%M:%S", &now_tm);
                return buffer;
            }

            // 追加模式写入文本
            static void WriteLog(const std::wstring& text) {
                // 打开文件，以追加模式写入
                std::wofstream ofs((rootPath + L"debug.log").c_str(), std::ios_base::app);
                if (!ofs.is_open()) return;
                // 设置流的区域设置为 UTF-8
                ofs.imbue(std::locale(ofs.getloc(), new std::codecvt_utf8<wchar_t>));
                // 获取当前时间
                const std::wstring& currentDateTime = GetCurrentDateTime();
                // 写入文件
                ofs << L"[" << currentDateTime << L"]" << text << std::endl;
                ofs.close();
            }

            static float DivideByTenCount(int n) {
                int count = 0;
                while (n >= 10) {
                    n /= 10;
                    count++;
                }
                return static_cast<float>(count);
            }
        private:
            static std::random_device rd;
            static std::mt19937 gen;
            static std::wstring rootPath;
            static cv::Mat img;//存储贴图模型，方便转镜面贴图
            /// <summary>
            /// 初始化当前程序运行的主目录
            /// </summary>
            /// <returns></returns>
            static std::wstring InitPath() {
                wchar_t exePath[MAX_PATH];
                GetModuleFileNameW(NULL, exePath, MAX_PATH);
                // 从 exe 路径中提取出目录部分
                wchar_t* lastSlash = wcsrchr(exePath, L'\\');
                if (lastSlash) {
                    *lastSlash = L'\0';
                }
                return  std::wstring(exePath) + L"\\";
            }

            static std::string GetColorString(const float * color) {
                return std::to_string(color[0]) + "," + std::to_string(color[1]) + "," +
                    std::to_string(color[2]) + "," + std::to_string(color[3]);
            }
        };
          
	}
}