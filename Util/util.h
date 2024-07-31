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

            static bool SaveMaterial(Material& material, const std::wstring& name) {
                char* pathstr = Util::WStringToChar((Util::GetRootPath() + L"Material\\" + name));
                std::ifstream file(pathstr);
                if (!file.is_open()) {
                    delete[] pathstr;
                    return false; // 无法打开文件
                }
                std::vector<std::string> sections = { "Material" };
                std::vector<std::string> keys = { "Ambient", "Diffuse","Specular","Shininess"};
                std::vector<std::string> values = { GetValuesString(material.ambient),
                   GetValuesString(material.diffuse),GetValuesString(material.specular),
                std::to_string(material.shininess)};
                std::vector<std::string> lines;
                std::string line;
                while (std::getline(file, line)) {
                    lines.push_back(line);
                }
                file.close(); // 关闭文件以重新打开进行写入
                std::set<std::string> updatedKeys; // 跟踪已更新的键
                std::string currentSection;
                for (size_t i = 0; i < lines.size(); ++i) {
                    line = lines[i];
                    // 忽略注释行
                    if (line.find('#') == 0) {
                        continue;
                    }
                    if (line.front() == '[' && line.back() == ']') {
                        currentSection = line.substr(1, line.length() - 2);
                        if (std::find(sections.begin(), sections.end(), currentSection) == sections.end())
                            continue; // 如果当前节不是目标节，跳过
                    }
                    for (size_t j = 0; j < keys.size(); ++j) {
                        const std::string& key = keys[j];
                        if (line.find(key + "=") == 0) {
                            lines[i] = key + "=" + values[j]; // 更新值
                            updatedKeys.insert(key);
                        }
                    }
                }

                // 检查是否所有的键都已更新
                if (updatedKeys.size() != keys.size()) {
                    delete[] pathstr;
                    return false;
                } 
                std::ofstream outfile(pathstr);
                delete[] pathstr;
                if (!outfile.is_open()) {
                    return false; // 无法打开文件进行写入
                }
                for (const auto& ln : lines) {
                    outfile << ln << "\n";
                }
                outfile.close();
                return true;
            }
            static bool LoadMaterial(Material& material,const std::wstring& name) {
                std::map<std::string, std::string> configDict; 
                char* pathstr = Util::WStringToChar((Util::GetRootPath() + L"Material\\" + name));
                std::ifstream file(pathstr);
                delete[] pathstr;
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