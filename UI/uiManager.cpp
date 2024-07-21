#include "uiManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "../Util/util.h"
namespace GL {
	namespace UI {
		using namespace GL::Tool;
		UiManager::UiManager(GlManager* glmanager):glmanager(glmanager) {
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
			//设置配置文件保存路径 Util::WStringToChar((Util::GetRootPath() + L"imgui.ini"))
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
		void UiManager::Render(int width, int height) {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			Draw(width,height);
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		void UiManager::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
			ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
		}

		void UiManager::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
			ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
		}

		/// <summary>
		/// 绘制UI组件
		/// </summary>
		void UiManager::Draw(int width, int height) {
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
				ImGui::SetNextWindowSize(ImVec2(float(static_cast<float>(width) / 6.0f), float(height)));//设置ui渲染区域
				ImGui::Begin(u8"设置菜单", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
				{
					//模型
					if (ImGui::TreeNode(u8"模型")) {
						if (ImGui::Button(u8"销毁模型")) {
							glmanager->ClearModels();
						}
						//标准化模型
						ImGui::Text(u8"");
						ImGui::Text(u8"标准化模型");
						ImGui::TreePop();
						//
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
