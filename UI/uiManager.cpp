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
			udata.selectedRadio = DRAW_MODE_SURFACE;//ѡ��ģʽ
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
			//���������ļ�����·�� 
		    io.IniFilename = Util::WStringToChar((Util::GetRootPath() + L"imgui.ini"));
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
		/// ����UI���
		/// </summary>
		void UiManager::Draw(Data& data) {
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
				ImGui::SetNextWindowSize(ImVec2(float(static_cast<float>(data.width) / 6.0f), float(data.height)));//����ui��Ⱦ����
				ImGui::Begin(u8"���ò˵�", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
				{
					//ģ��
					if (ImGui::TreeNode(u8"ģ��")) {
						float maxWidth = ImGui::GetContentRegionAvail().x;
						if (ImGui::Button(u8"���ó���")) {
							glmanager->ClearModels();
						}
						//��׼��ģ��
						ImGui::Text(u8"");
						ImGui::Text(u8"��׼��ģ��");
						ImGui::SetNextItemWidth(maxWidth / 3.0f);
						//���
						ImGui::InputDouble(u8"���", &udata.modelWidth, 0.1, 1.0, "%.2f");
						ImGui::SameLine();
						//�߶�
						ImGui::SetNextItemWidth(maxWidth / 3.0f);
						ImGui::InputDouble(u8"�߶�", &udata.modelHeight, 0.1, 1.0, "%.2f");
						//X�ֱ���
						ImGui::SetNextItemWidth(maxWidth / 3.0f);
						ImGui::InputDouble(u8"Xƫ��", &udata.modelXOffset, 0.1, 1.0, "%.2f");
						ImGui::SameLine();
						//Y�ֱ���
						ImGui::SetNextItemWidth(maxWidth / 3.0f);
						ImGui::InputDouble(u8"Yƫ��", &udata.modelYOffset, 0.1, 1.0, "%.2f");
						//���Z��
						ImGui::Checkbox(u8"���Z��", &udata.modelRandomZ);
						//�������
						ImGui::SameLine();
						ImGui::SetNextItemWidth(maxWidth / 3.0f);
						ImGui::InputDouble(u8"�������", &udata.modelRandomRange, 0.1, 1.0, "%.2f");
						//����ģ��
						if (ImGui::Button(u8"����ģ��")) {
							glmanager->ClearModels();
							glmanager->CreateRandomModel(udata);
						}
						//���ӻ�ģʽ
						ImGui::Text(u8"");
						ImGui::Text(u8"��ʾģʽ");
						const char* point = u8"����";
						const char* grid = u8"����";
						const char* surface = u8"��";
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
					if (ImGui::TreeNode(u8"��ͼ")) {
						float maxWidth = ImGui::GetContentRegionAvail().x;
						ImGui::SetNextItemWidth(maxWidth / 1.2f);
						ImGui::InputText("", udata.pathBuf, IM_ARRAYSIZE(udata.pathBuf));
						ImGui::SameLine();
						//ѡ��·��
						if (ImGui::Button(u8"...")) {
							//���ⲿ�ļ�����
							nfdchar_t* outPath = NULL;
							nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outPath);
							if (result == NFD_OKAY) {
								std::string selectedFilePath = outPath;
								if (!selectedFilePath.empty()) {
									strncpy_s(udata.pathBuf, selectedFilePath.c_str(), IM_ARRAYSIZE(udata.pathBuf) - 1);
									udata.pathBuf[IM_ARRAYSIZE(udata.pathBuf) - 1] = '\0'; // ȷ���ַ����Կ��ַ���β
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
						if (ImGui::Button(u8"������ͼ")) {
							glmanager->CreateModelTexture(udata.pathBuf,glmanager->GetCurrentModel(),nullptr,0);
						}
						ImGui::TreePop();
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
