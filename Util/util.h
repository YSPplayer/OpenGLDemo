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
            /// �����淨��
            /// </summary>
            /// <param name="v1"></param>
            /// <param name="v2"></param>
            /// <param name="v3"></param>
            /// <returns></returns>
            static glm::vec3 CalculateFaceNormal(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
                glm::vec3 edge1 = v2 - v1;
                glm::vec3 edge2 = v3 - v1;
                glm::vec3 normal = glm::cross(edge1, edge2);
                return glm::normalize(normal); // ��һ����������
            }
            static float GetRandomFloat(float min, float max) {
                std::uniform_real_distribution<float> dis(min, max);
                return dis(gen);
            }
            /// <summary>
            /// ��ȡ��ǰ�������е���Ŀ¼
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
                char* charPtr = new char[str.length() + 1]; //char�ڴ���Ҫ�ֶ������ͷ�
                strcpy_s(charPtr, str.length() + 1, str.c_str());
                return charPtr;
            }

            /// <summary>
            /// ��ȡ��ǰ��ʱ��
            /// </summary>
            /// <returns></returns>
            static std::wstring GetCurrentDateTime() {
                auto now = std::chrono::system_clock::now();
                std::time_t now_time = std::chrono::system_clock::to_time_t(now);
                std::tm now_tm;
                localtime_s(&now_tm, &now_time); // ʹ���̰߳�ȫ��localtime_s
                wchar_t buffer[100];
                wcsftime(buffer, sizeof(buffer), L"%Y-%m-%d %H:%M:%S", &now_tm);
                return buffer;
            }

            // ׷��ģʽд���ı�
            static void WriteLog(const std::wstring& text) {
                // ���ļ�����׷��ģʽд��
                std::wofstream ofs((rootPath + L"debug.log").c_str(), std::ios_base::app);
                if (!ofs.is_open()) return;
                // ����������������Ϊ UTF-8
                ofs.imbue(std::locale(ofs.getloc(), new std::codecvt_utf8<wchar_t>));
                // ��ȡ��ǰʱ��
                const std::wstring& currentDateTime = GetCurrentDateTime();
                // д���ļ�
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
            /// ��ʼ����ǰ�������е���Ŀ¼
            /// </summary>
            /// <returns></returns>
            static std::wstring InitPath() {
                wchar_t exePath[MAX_PATH];
                GetModuleFileNameW(NULL, exePath, MAX_PATH);
                // �� exe ·������ȡ��Ŀ¼����
                wchar_t* lastSlash = wcsrchr(exePath, L'\\');
                if (lastSlash) {
                    *lastSlash = L'\0';
                }
                return  std::wstring(exePath) + L"\\";
            }
        };
          
	}
}