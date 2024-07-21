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
		/// ����imggui
		/// </summary>
		UiManager::~UiManager() {
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
		/// <summary>
		/// ��ʼ��imgui
		/// </summary>
		void UiManager::Init(GLFWwindow* window) {
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			//���������ļ�����·�� Util::WStringToChar((Util::GetRootPath() + L"imgui.ini"))
		    io.IniFilename = nullptr;
			ImFontConfig fontCfg;//֧��������ʾ����ȡ����·��
			io.Fonts->AddFontFromFileTTF(Util::WStringToString((Util::GetRootPath() + L"Font/YoungRound_CN.TTF")).c_str(), 14.0f, &fontCfg, io.Fonts->GetGlyphRangesChineseFull());
			ImGui::StyleColorsDark();
			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init("#version 330 core");

		}

		/// <summary>
		/// ����imggui����
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
		/// ����UI���
		/// </summary>
		void UiManager::Draw(int width, int height) {
			SetStyle();
			//�˵���//
			{
				ImGui::BeginMainMenuBar();
				{
					if (ImGui::BeginMenu(u8"�ļ�")) {
						if (ImGui::MenuItem(u8"���ļ�")) {}
						ImGui::EndMenu();
					}
				}
				ImGui::EndMainMenuBar();
			}

			//���ò˵�����
			{
				// ��ȡ�˵����ĸ߶�
				float menuBarHeight = ImGui::GetFrameHeight();
				ImGui::SetNextWindowPos(ImVec2(float(0), float(menuBarHeight + 10)));
				ImGui::SetNextWindowSize(ImVec2(float(static_cast<float>(width) / 6.0f), float(height)));//����ui��Ⱦ����
				ImGui::Begin(u8"���ò˵�", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
				{
					//ģ��
					if (ImGui::TreeNode(u8"ģ��")) {
						if (ImGui::Button(u8"����ģ��")) {
							glmanager->ClearModels();
						}
						//��׼��ģ��
						ImGui::Text(u8"");
						ImGui::Text(u8"��׼��ģ��");
						ImGui::TreePop();
						//
					}
				}
				ImGui::End();

			}
			//ImGui::SetNextWindowPos(ImVec2(float(0), float(0)));
			//ImGui::SetNextWindowSize(ImVec2(float(static_cast<float>(width) / 6.0f), float(height)));//����ui��Ⱦ����
//			ImGui::Begin(u8"���ò˵�", nullptr,ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);                          // Create a window called "Hello, world!" and append into it.
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
		/// �����������ɫ��ʽ
		/// </summary>
		void UiManager::SetStyle() {
			//���������ɫ
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
			//ȡ���߿���ʽ 
			style.Colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // ���ñ߿���ɫΪ͸��
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // ������Ӱ��ɫΪ͸��
			style.FrameBorderSize = 0.0f; // ���ñ߿���Ϊ0
			
		}
	}
}
