#include "uiManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "../Util/util.h"
#include <nfd/nfd.h>
#include <x3pFormat.h>
namespace GL {
	namespace UI {
		using namespace GL::Tool;
		using namespace CPlusSDK::X3pFormatDll;
		UData UiManager::udata;
		UiManager::UiManager(GlManager* glmanager,bool load) :glmanager(glmanager) {
			if (!load) {
				std::string selectedFilePath = " ";//初始化数据
				strncpy_s(udata.pathBuf, selectedFilePath.c_str(), IM_ARRAYSIZE(udata.pathBuf) - 1);
				udata.pathBuf[IM_ARRAYSIZE(udata.pathBuf) - 1] = '\0'; // 确保字符串以空字符结尾 
				udata.modelWidth = 1224;
				udata.modelHeight = 1026;
				udata.modelXOffset = 1.38;
				udata.modelYOffset = 1.38;
				udata.modelRandomZ = true;
				udata.modelRandomRange = 10;
				udata.selectedRadio = DRAW_MODE_SURFACE;//选择模式
			}
		}
		/// <summary>
		/// 清理imggui
		/// </summary>
		UiManager::~UiManager() {
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
		/// <summary>
		/// 初始化imgui
		/// </summary>
		void UiManager::Init(GLFWwindow* window) {
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			//设置配置文件保存路径  Util::WStringToChar((Util::GetRootPath() + L"imgui.ini"))
		    io.IniFilename = nullptr;
			ImFontConfig fontCfg;//支持中文显示，获取字体路径
			io.Fonts->AddFontFromFileTTF(Util::WStringToString((Util::GetRootPath() + L"Font/YoungRound_CN.TTF")).c_str(), 14.0f, &fontCfg, io.Fonts->GetGlyphRangesChineseFull());
			ImGui::StyleColorsDark();
			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init("#version 330 core");

		}

		/// <summary>
		/// 绘制imggui代码
		/// </summary>
		/// <param name="width"></param>
		/// <param name="height"></param>
		void UiManager::Render(Data& data) {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			Draw(data);
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		bool UiManager::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
			ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
			return ImGui::GetIO().WantCaptureMouse;
		}

		bool UiManager::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
			ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
			return ImGui::GetIO().WantCaptureMouse;
		}

		void UiManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			ImGui_ImplGlfw_KeyCallback(window,key,scancode, action, mods);
		}

		/// <summary>
		/// 绘制UI组件
		/// </summary>
		void UiManager::Draw(Data& data) {
			SetStyle(data);
			//菜单栏//
			{
				ImGui::BeginMainMenuBar();
				{
					if (ImGui::BeginMenu(u8"文件")) {
						if (ImGui::MenuItem(u8"打开文件")) {
							nfdchar_t* outPath = NULL;
							const nfdchar_t* filterList = "x3p";
							nfdresult_t result = NFD_OpenDialog(filterList, NULL, &outPath);
							if (result == NFD_OKAY) {
								std::string selectedFilePath = outPath;
								X3pData data;
								//加载x3p模型
								bool success = LoadX3p(selectedFilePath.c_str(),&data,false,nullptr);
								glmanager->ClearModels();
								glmanager->CreateX3pModel(data.sizeX,data.sizeY, data.axes[0].increment, data.axes[1].increment,data.pointData,data.minZ,data.maxZ);
							}
							else if (result == NFD_CANCEL) {
								std::cout << "User pressed cancel." << std::endl;
							}
							else {
								std::cout << "Error: " << NFD_GetError() << std::endl;
							}
						}
						ImGui::EndMenu();
					}
					if (ImGui::BeginMenu(u8"配置")) {
						if (ImGui::MenuItem(u8"保存配置")) {
							bool success = Util::CreateConfig(data,udata);
					/*		if (success) {
								std::cout << "Create config success!" << std::endl;
							}
							else {
								std::cout << "Create config fail!" << std::endl;
							}*/
						}
						if (ImGui::MenuItem(u8"加载材质")) {
							Material& material = glmanager->GetCurrentModel()->material;
							nfdchar_t* outPath = NULL;
							const nfdchar_t* filterList = "material";
							nfdresult_t result = NFD_OpenDialog(filterList, NULL, &outPath);
							if (result == NFD_OKAY) {
								std::string selectedFilePath = outPath;
								if (!selectedFilePath.empty()) {
									Util::LoadMaterial(material, Util::StringToWString(selectedFilePath),true);
									//存储模型颜色
									data.colors[1][0] = material.diffuse[0];
									data.colors[1][1] = material.diffuse[1];
									data.colors[1][2] = material.diffuse[2];
								}
								free(outPath);
							}
							else if (result == NFD_CANCEL) {
								std::cout << "[UiManager::Draw]User pressed cancel." << std::endl;
							}
							else {
								std::cout << "[UiManager::Draw]Error: " << NFD_GetError() << std::endl;
							}
						}
						if (ImGui::MenuItem(u8"保存材质")) {
							Material& material = glmanager->GetCurrentModel()->material;
							nfdchar_t* outPath = nullptr;
							const nfdchar_t* filterList = "material"; // 设置文件类型过滤器
							nfdresult_t result = NFD_SaveDialog(filterList, nullptr, &outPath);
							if (result == NFD_OKAY) {
								std::string selectedFilePath = outPath;
								bool success = Util::SaveMaterial(material, Util::StringToWString(selectedFilePath), true);
								/*if (success) {
									std::cout << "Save material success!" << std::endl;
								}
								else {
									std::cout << "Save material fail!" << std::endl;
								}*/
							}
							else if (result == NFD_CANCEL) {
								std::cout << "User pressed cancel." << std::endl;
							}
							else {
								std::cout << "Error: " << NFD_GetError() << std::endl;
							}
				
						}
						ImGui::EndMenu();
					}
				}
				ImGui::EndMainMenuBar();
			}

			//设置菜单容器
			{
				// 获取菜单栏的高度
				float menuBarHeight = ImGui::GetFrameHeight();
				ImGui::SetNextWindowPos(ImVec2(float(0), float(menuBarHeight)));
				ImGui::SetNextWindowSize(ImVec2(float(static_cast<float>(data.width) / 5.0f), float(data.height - menuBarHeight)));//设置ui渲染区域
				ImGui::Begin(u8"设置菜单", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
				{
					//模型
					if (ImGui::TreeNode(u8"模型")) {
						float maxWidth = ImGui::GetContentRegionAvail().x;
						if (ImGui::Button(u8"重置场景")) {
							glmanager->ClearModels();
						}
						//标准化模型
						ImGui::Text(u8"");
						ImGui::Text(u8"标准化模型");
						ImGui::SetNextItemWidth(maxWidth / 3.0f);
						//宽度
						ImGui::InputDouble(u8"宽度", &udata.modelWidth, 0.1, 1.0, "%.2f");
						ImGui::SameLine();
						//高度
						ImGui::SetNextItemWidth(maxWidth / 3.0f);
						ImGui::InputDouble(u8"高度", &udata.modelHeight, 0.1, 1.0, "%.2f");
						//X分辨率
						ImGui::SetNextItemWidth(maxWidth / 3.0f);
						ImGui::InputDouble(u8"X偏移", &udata.modelXOffset, 0.1, 1.0, "%.2f");
						ImGui::SameLine();
						//Y分辨率
						ImGui::SetNextItemWidth(maxWidth / 3.0f);
						ImGui::InputDouble(u8"Y偏移", &udata.modelYOffset, 0.1, 1.0, "%.2f");
						//随机Z轴
						ImGui::Checkbox(u8"随机Z轴", &udata.modelRandomZ);
						//随机区间
						ImGui::SameLine();
						ImGui::SetNextItemWidth(maxWidth / 3.0f);
						ImGui::InputDouble(u8"随机区间", &udata.modelRandomRange, 0.1, 1.0, "%.2f");
						//生成模型
						if (ImGui::Button(u8"生成模型")) {
							glmanager->ClearModels();
							glmanager->CreateRandomModel(udata);
						}
						//可视化模式
						ImGui::Text(u8"");
						ImGui::Text(u8"显示模式");
						const char* point = u8"点云";
						const char* grid = u8"网格";
						const char* surface = u8"面";
						for (int i = 0; i < 3; i++) {
							
							if (ImGui::Selectable(i == 0 ? point : i == 1 ? grid : surface, udata.selectedRadio == i)) {
								udata.selectedRadio = i;
								data.drawMode = i;
							}
						}
						ImGui::TreePop();
					}
					ImGui::Text(u8"");
					if (ImGui::TreeNode(u8"贴图")) {
						ImGui::Checkbox(u8"启用贴图", &data.useTexture);
						ImGui::Text(u8"");
						float maxWidth = ImGui::GetContentRegionAvail().x;
						ImGui::SetNextItemWidth(maxWidth / 1.2f);
						ImGui::InputText("##贴图路径", udata.pathBuf, IM_ARRAYSIZE(udata.pathBuf));
						ImGui::SameLine();
						//选择路径
						if (ImGui::Button(u8"...")) {
							//打开外部文件窗口
							nfdchar_t* outPath = NULL;
							const nfdchar_t* filterList = "jpg;png;bmp";
							nfdresult_t result = NFD_OpenDialog(filterList, NULL, &outPath);
							if (result == NFD_OKAY) {
								std::string selectedFilePath = outPath;
								if (!selectedFilePath.empty()) {
									strncpy_s(udata.pathBuf, selectedFilePath.c_str(), IM_ARRAYSIZE(udata.pathBuf) - 1);
									udata.pathBuf[IM_ARRAYSIZE(udata.pathBuf) - 1] = '\0'; // 确保字符串以空字符结尾
								}
								free(outPath);
							}
							else if (result == NFD_CANCEL) {
								std::cout << "[UiManager::Draw]User pressed cancel." << std::endl;
							}
							else {
								std::cout << "[UiManager::Draw]Error: " << NFD_GetError() << std::endl;
							}
						}
						if (ImGui::Button(u8"加载贴图")) {
							glmanager->CreateModelTexture(udata.pathBuf,glmanager->GetCurrentModel(),nullptr,0);
						}
						ImGui::TreePop();
					}
					ImGui::Text(u8"");
					float (*colors)[4] = data.colors;
					static float color[4] = { 255.0f, 255.0f, 255.0f, 255.0f };
					if (ImGui::TreeNode(u8"颜色")) {
						float maxWidth = ImGui::GetContentRegionAvail().x;
						if (ImGui::ColorButton("##colorbutton0", ImVec4(colors[0][0], colors[0][1], colors[0][2], colors[0][3]), ImGuiColorEditFlags_NoTooltip, ImVec2(maxWidth / 8.0f, maxWidth / 12.0f))) {
							colors[0][0] = color[0];
							colors[0][1] = color[1];
							colors[0][2] = color[2];
						}
						ImGui::SameLine();
						ImGui::Text(u8"背景颜色");
						if (ImGui::ColorButton("##colorbutton1", ImVec4(colors[1][0], colors[1][1], colors[1][2], colors[1][3]), ImGuiColorEditFlags_NoTooltip, ImVec2(maxWidth / 8.0f, maxWidth / 12.0f))) {
							colors[1][0] = color[0];
							colors[1][1] = color[1];
							colors[1][2] = color[2];
							//更新材质
							Material& material = glmanager->GetCurrentModel()->material;
							material.diffuse = glm::vec3(colors[1][0], colors[1][1], colors[1][2]);
						}
						ImGui::SameLine();
						ImGui::Text(u8"模型颜色");
						if (ImGui::ColorButton("##colorbutton2", ImVec4(colors[2][0], colors[2][1], colors[2][2], colors[2][3]), ImGuiColorEditFlags_NoTooltip, ImVec2(maxWidth / 8.0f, maxWidth / 12.0f))) {
							colors[2][0] = color[0];
							colors[2][1] = color[1];
							colors[2][2] = color[2];
						}
						ImGui::SameLine();
						ImGui::Text(u8"光照颜色");
						ImGui::ColorPicker4("##picker", color);
						ImGui::TreePop();
					}
					ImGui::Text(u8"");
					if (ImGui::TreeNode(u8"光照属性")) {
						ImGui::Checkbox(u8"启用光照", &data.useLight);
						float maxWidth = ImGui::GetContentRegionAvail().x;
						ImGui::Text(u8"1.模型材质");
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						Material& material = glmanager->GetCurrentModel()->material;
						ImGui::InputFloat(u8"##环境光x", &material.ambient[0], 0.0, 1.0, "%.10f");
						ImGui::SameLine();
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						ImGui::InputFloat(u8"##环境光y", &material.ambient[1], 0.0, 1.0, "%.10f");
						ImGui::SameLine();
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						ImGui::InputFloat(u8"##环境光z", &material.ambient[2], 0.0, 1.0, "%.10f");
						ImGui::SameLine();
						ImGui::Text(u8"环境光");
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						ImGui::InputFloat(u8"##漫反射x", &material.diffuse[0], 0.0, 1.0, "%.10f");
						ImGui::SameLine();
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						ImGui::InputFloat(u8"##漫反射y", &material.diffuse[1], 0.0, 1.0, "%.10f");
						ImGui::SameLine();
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						ImGui::InputFloat(u8"##漫反射z", &material.diffuse[2], 0.0, 1.0, "%.10f");
						ImGui::SameLine();
						ImGui::Text(u8"漫反射");
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						ImGui::InputFloat(u8"##镜面反射x", &material.specular[0], 0.0, 1.0, "%.10f");
						ImGui::SameLine();
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						ImGui::InputFloat(u8"##镜面反射y", &material.specular[1], 0.0, 1.0, "%.10f");
						ImGui::SameLine();
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						ImGui::InputFloat(u8"##镜面反射z", &material.specular[2], 0.0, 1.0, "%.10f");
						ImGui::SameLine();
						ImGui::Text(u8"镜面反射");
						ImGui::Text(u8"");
						ImGui::Text(u8"2.贴图材质");
						ImGui::SetNextItemWidth(maxWidth / 3.0f);
						ImGui::InputDouble(u8"对比度", &data.alpha, 1.0, 10.0, "%.5f");
						ImGui::SetNextItemWidth(maxWidth / 3.0f);
						ImGui::InputInt(u8"亮度", &data.beta, 0, 100);
						if (ImGui::Button(u8"重置镜面贴图")) {
							glmanager->ChangeModelSpecularImage(glmanager->GetCurrentModel(),data.alpha,data.beta);
						}
						ImGui::Text(u8"");
						ImGui::SliderFloat(u8"光泽度", &material.shininess, 0.00001f, 256.0f);
						ImGui::SetNextItemWidth(maxWidth / 3.0f);
						ImGui::InputFloat(u8"##光泽度", &material.shininess, 0.00001f, 256.0f, "%.10f");
						ImGui::TreePop();
					}
					ImGui::Text(u8"");
					if (ImGui::TreeNode(u8"系统")) {
						ImGui::Checkbox(u8"背景透明", &data.transparentBg);
						ImGui::SameLine();
						ImGui::Checkbox(u8"背面剔除", &data.cullBackFace);
						ImGui::Text(u8"");
						ImGui::SliderFloat(u8"相机移动速度", &data.moveSpeedUnit, 1.0f, 10.0f);
						ImGui::Text(u8"");
						ImGui::SliderFloat(u8"相机视角灵敏度", &data.sensitivity, 0.01f, 0.1f);
						ImGui::TreePop();
					}
				}
				ImGui::End();

			}
		}
		/// <summary>
		/// 设置组件的颜色样式
		/// </summary>
		void UiManager::SetStyle(Data& data) {
			//设置组件颜色
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
			//取消边框样式 
			style.Colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // 设置边框颜色为透明
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // 设置阴影颜色为透明
			style.FrameBorderSize = 0.0f; // 设置边框宽度为0
			float transparent = data.transparentBg ? 0.0f : 0.94f;
			//设置背景是否透明
			style.Colors[ImGuiCol_WindowBg].w = transparent;
			style.Colors[ImGuiCol_PopupBg].w = transparent;
			
		}
	}
}
