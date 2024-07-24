#pragma once
#include <cmath>
#include <iostream>
#include <fstream>
#include <random>
#include <codecvt>
#include <Windows.h>
#include <chrono>
#include <string>
#include <glm/glm.hpp>
namespace GL {
	namespace Tool {
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
            static const char* WStringToChar(const std::wstring& wstr) {
                const std::string& str = WStringToString(wstr);
                char* charPtr = new char[str.length() + 1]; //char内存需要手动管理释放
                strcpy_s(charPtr, str.length() + 1, str.c_str());
                return charPtr;
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
        };
          
	}
}