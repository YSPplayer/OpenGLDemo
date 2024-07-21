#include "uiManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "../Util/util.h"
#include <nfd/nfd.h>
namespace GL {
	namespace UI {
		using namespace GL::Tool;
		UData UiManager::udata;
		UiManager::UiManager(GlManager* glmanager):glmanager(glmanager) {
			udata.modelWidth = 1224;
			udata.modelHeight = 1026;
			udata.modelXOffset = 1.38;
			udata.modelYOffset = 1.38;
			udata.modelRandomZ = true;
			udata.modelRandomRange = 10;
			udata.selectedRadio = DRAW_MODE_SURFACE;//选择模式
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
			//设置配置文件保存路径 
		    io.IniFilename = Util::WStringToChar((Util::GetRootPath() + L"imgui.ini"));
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

		void UiManager::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
			ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
		}

		void UiManager::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
			ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
		}

		void UiManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			ImGui_ImplGlfw_KeyCallback(window,key,scancode, action, mods);
		}

		/// <summary>
		/// 绘制UI组件
		/// </summary>
		void UiManager::Draw(Data& data) {
			SetStyle();
			//菜单栏//
			{
				ImGui::BeginMainMenuBar();
				{
					if (ImGui::BeginMenu(u8"文件")) {
						if (ImGui::MenuItem(u8"打开文件")) {}
						ImGui::EndMenu();
					}
				}
				ImGui::EndMainMenuBar();
			}

			//设置菜单容器
			{
				// 获取菜单栏的高度
				float menuBarHeight = ImGui::GetFrameHeight();
				ImGui::SetNextWindowPos(ImVec2(float(0), float(menuBarHeight + 10)));
				ImGui::SetNextWindowSize(ImVec2(float(static_cast<float>(data.width) / 6.0f), float(data.height)));//设置ui渲染区域
				ImGui::Begin(u8"设置菜单", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
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
						for (int i = 0; i < 3; i++)
						{
							
							if (ImGui::Selectable(i == 0 ? point : i == 1 ? grid : surface, udata.selectedRadio == i)) {
								udata.selectedRadio = i;
								data.drawMode = i;
							}
						}
						ImGui::TreePop();
					}
					ImGui::Text(u8"");
					if (ImGui::TreeNode(u8"贴图")) {
						float maxWidth = ImGui::GetContentRegionAvail().x;
						ImGui::SetNextItemWidth(maxWidth / 1.2f);
						ImGui::InputText("", udata.pathBuf, IM_ARRAYSIZE(udata.pathBuf));
						ImGui::SameLine();
						//选择路径
						if (ImGui::Button(u8"...")) {
							//打开外部文件窗口
							nfdchar_t* outPath = NULL;
							nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outPath);
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
				}
				ImGui::End();

			}
			//ImGui::SetNextWindowPos(ImVec2(float(0), float(0)));
			//ImGui::SetNextWindowSize(ImVec2(float(static_cast<float>(width) / 6.0f), float(height)));//设置ui渲染区域
//			ImGui::Begin(u8"设置菜单", nullptr,ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);                          // Create a window called "Hello, world!" and append into it.
//			
////			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
////			//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
////			//ImGui::Checkbox("Another Window", &show_another_window);
////
/////*			ImGui::SliderFloat("float",, 0.0f, 1.0f);        */    // Edit 1 float using a slider from 0.0f to 1.0f
////			//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
////
////			//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
////			//	counter++;
////			ImGui::SameLine();
////			ImGui::Text("counter = %d", 1);
//		/*	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);*/
//			ImGui::End();

		}
		/// <summary>
		/// 设置组件的颜色样式
		/// </summary>
		void UiManager::SetStyle() {
			//设置组件颜色
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
			//取消边框样式 
			style.Colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // 设置边框颜色为透明
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // 设置阴影颜色为透明
			style.FrameBorderSize = 0.0f; // 设置边框宽度为0
			
		}
	}
}
