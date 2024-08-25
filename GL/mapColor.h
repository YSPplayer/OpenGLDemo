#pragma once
#include <vector>
#include "../Util/util.h"
namespace GL {
	using namespace GL::Tool;
	struct MColor {
		unsigned char R;
		unsigned char G;
		unsigned char B;
	};
	class MapColor {
	private:
		std::vector<std::vector<unsigned char>> color;
		std::vector<unsigned char> node;
		int nbnode;
		int size;
		void BuildNodes() {
			color[3][0] = 1;
			color[3][size - 1] = 1;
			nbnode = 0;
			for (int i = 0; i < size; i++) {
				if (color[3][i] == 1) {
					node[nbnode] = static_cast<unsigned char>(i);
					nbnode++;
				}
			}
		}

		void Build() {
			int x1, y1, x2, y2;
			double a, b;
			for (int k = 0; k < 3; k++) {
				for (int i = 0; i < nbnode - 1; i++) {
					x1 = static_cast<int>(node[i]);
					x2 = static_cast<int>(node[i + 1]);
					y1 = color[k][x1];
					y2 = color[k][x2];
					a = static_cast<double>(y2 - y1) / static_cast<double>(x2 - x1);
					b = static_cast<double>(y1) - a * static_cast<double>(x1);
					for (int j = x1; j < x2; j++) {
						color[k][j] = static_cast<unsigned char>(a * static_cast<double>(j) + b);
					}
				}
			}
		}
	public:
		MapColor() {
			nbnode = 0;
			size = 256;
			color = std::vector<std::vector<unsigned char>>(4, std::vector<unsigned char>(size, 0));
			node = std::vector<unsigned char>(size, 0);
		}
		MapColor(const std::vector<MColor>& colors, const std::vector<unsigned char>& indexs) {
			nbnode = 0;
			size = 256;
			color = std::vector<std::vector<unsigned char>>(4, std::vector<unsigned char>(size, 0));
			node = std::vector<unsigned char>(size, 0);
			for (int i = 0; i < size; i++)
				color[3][i] = 0;
			color[3][0] = 1;
			color[3][size - 1] = 1;
			BuildNodes();
			for (int index = 0; index < colors.size(); ++index) {
				const MColor& mcolor = colors[index];
				color[0][indexs[index]] = mcolor.R;
				color[1][indexs[index]] = mcolor.G;
				color[2][indexs[index]] = mcolor.B;
				color[3][indexs[index]] = 1;
			}
			BuildNodes();
			Build();
		}
		unsigned char inline ColorR(unsigned char index) { return color[0][index]; }
		unsigned char inline ColorG(unsigned char index) { return color[1][index]; }
		unsigned char inline ColorB(unsigned char index) { return color[2][index]; }
		static void SetColorForZ(float**&colorMaps,float fraction,int index, std::vector<MapColor>& mapColor) {
			// 计算三次方，并将结果缩放到 [0, 255] 范围内
			float scaledValue = pow(fraction, 3) * 255.0;
			// 将结果转换为整数类型，得到灰度值
			int grey = static_cast<int>(scaledValue);
			int ugrey = 0;
			ugrey = grey > 255 ? 255 : static_cast<unsigned char>(grey);
			ugrey = grey < 0 ? 0 : static_cast<unsigned char>(grey);
			//统一化全部颜色
			for (int i = 0; i < mapColor.size(); i++) {
				colorMaps[i][index * 3 + 0] = static_cast<float>(mapColor[i].ColorR(ugrey)) / 255.0f;
				colorMaps[i][index * 3 + 1] = static_cast<float>(mapColor[i].ColorG(ugrey)) / 255.0f;
				colorMaps[i][index * 3 + 2] = static_cast<float>(mapColor[i].ColorB(ugrey)) / 255.0f;
			/*	Util::WriteLog(std::to_wstring(colorMaps[i][index * 3 + 0]));
				Util::WriteLog(std::to_wstring(colorMaps[i][index * 3 + 1]));
				Util::WriteLog(std::to_wstring(colorMaps[i][index * 3 + 2]));
				Util::WriteLog(L"");*/
			}
		}
	};


}