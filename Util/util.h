#pragma once
#include <cmath>
#include <iostream>
#include <random>
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

            static float GetRandomFloat(float min, float max) {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_real_distribution<float> dis(min, max);
                return dis(gen);
            }
		};
	}
}