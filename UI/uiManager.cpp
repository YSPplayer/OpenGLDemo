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
		UiManager::UiManager(GlManager* glmanager, bool load) :glmanager(glmanager) {
			if (!load) {
				std::string selectedFilePath = " ";//��ʼ������
				strncpy_s(udata.pathBuf, selectedFilePath.c_str(), IM_ARRAYSIZE(udata.pathBuf) - 1);
				udata.pathBuf[IM_ARRAYSIZE(udata.pathBuf) - 1] = '\0'; // ȷ���ַ����Կ��ַ���β 
				udata.modelWidth = 1224;
				udata.modelHeight = 1026;
				udata.modelXOffset = 1.38;
				udata.modelYOffset = 1.38;
				udata.modelRandomZ = true;
				udata.modelRandomRange = 10;
				udata.selectedRadio = DRAW_MODE_SURFACE;//ѡ��ģʽ
			}
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
			//���������ļ�����·��  Util::WStringToChar((Util::GetRootPath() + L"imgui.ini"))
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
			ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
		}

		/// <summary>
		/// ����UI���
		/// </summary>
		void UiManager::Draw(Data& data) {
			SetStyle(data);
			//�˵���//
			{
				ImGui::BeginMainMenuBar();
				{
					if (ImGui::BeginMenu(u8"�ļ�")) {
						if (ImGui::MenuItem(u8"���ļ�")) {
							nfdchar_t* outPath = NULL;
							const nfdchar_t* filterList = "x3p";
							nfdresult_t result = NFD_OpenDialog(filterList, NULL, &outPath);
							if (result == NFD_OKAY) {
								std::string selectedFilePath = outPath;
								X3pData data;
								//����x3pģ��
								bool success = LoadX3p(selectedFilePath.c_str(), &data, false, nullptr);
								glmanager->ClearModels();
								glmanager->CreateX3pModel(data.sizeX, data.sizeY, data.axes[0].increment, data.axes[1].increment, data.pointData, data.minZ, data.maxZ);
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
					if (ImGui::BeginMenu(u8"����")) {
						if (ImGui::MenuItem(u8"��������")) {
							bool success = Util::CreateConfig(data, udata);
							/*		if (success) {
										std::cout << "Create config success!" << std::endl;
									}
									else {
										std::cout << "Create config fail!" << std::endl;
									}*/
						}
						if (ImGui::MenuItem(u8"���ز���")) {
							Material& material = glmanager->GetCurrentModel()->material;
							nfdchar_t* outPath = NULL;
							const nfdchar_t* filterList = "material";
							nfdresult_t result = NFD_OpenDialog(filterList, NULL, &outPath);
							if (result == NFD_OKAY) {
								std::string selectedFilePath = outPath;
								if (!selectedFilePath.empty()) {
									Util::LoadMaterial(material, Util::StringToWString(selectedFilePath), true);
									//�洢ģ����ɫ
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
						if (ImGui::MenuItem(u8"�������")) {
							Material& material = glmanager->GetCurrentModel()->material;
							nfdchar_t* outPath = nullptr;
							const nfdchar_t* filterList = "material"; // �����ļ����͹�����
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

			//�����ƶ�����
			{
				float menuBarHeight = ImGui::GetFrameHeight();
				float menuWidth = static_cast<float>(data.width) / 10.0f;
				ImGui::SetNextWindowPos(ImVec2(float(data.width - menuWidth), float(menuBarHeight)));
				ImGui::SetNextWindowSize(ImVec2(menuWidth, menuWidth));//����ui��Ⱦ����
				ImGui::Begin(u8"##����", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
				LightControl* lcontrol = glmanager->GetLightControl();
				Model* model = glmanager->GetCurrentModel();
				/*
				   ��λ��theta(y) ����phi(z)
				*/
				/*	++data.phi;
					if (data.phi >= 180.0f) data.phi = -180.0f;*/
					/*	data.phi = 10.0f;
						++data.theta;
						if (data.theta >= 180.0f) data.theta = -180.0f;*/
				float maxWidth = ImGui::GetContentRegionAvail().x;
				//��Ϊopengl������ϵ�ͱ�׼����ϵ��һ��
				ImGui::Checkbox(u8"���ù���ģ��", &data.showLightMode);
				ImGui::Text(u8"����:");
				ImGui::SetNextItemWidth(maxWidth / 2.0f);
				ImGui::SliderFloat(u8"##��λ��", &data.phi, -180.0f, 180.0f, "%.8f"); //[0 - ��]
				ImGui::Text(u8"��λ��:");
				ImGui::SetNextItemWidth(maxWidth / 2.0f);
				ImGui::SliderFloat(u8"##����", &data.theta, 0.0f, 180.0f, "%.8f");//[-�� - ��]
				//if (ImGui::Button(u8"��")) {
				//	ff -= 0.1F;
				//	std::cout << ff << std::endl;
				//	spherical = Util::SetSpherical(glm::pi<float>(),0.0f ,spherical); // std::max(0.0f, spherical.z - 0.1f);
				//}
				//ImGui::SameLine();
				//if (ImGui::Button(u8"��")) {
				//	spherical.z = std::min(glm::pi<float>() / 2.0f, spherical.z + 0.1f);
				//}
				//ImGui::Text(u8"");
				//if (ImGui::Button(u8"��")) {
				//	spherical.y = std::max(-glm::pi<float>() / 2.0f, spherical.y - 0.1f);
				//}
				//ImGui::SameLine();
				//if (ImGui::Button(u8"��")) {
				//	spherical.y = std::min(glm::pi<float>() / 2.0f, spherical.y + 0.1f);
				//}
				//��������ת�صѿ������� Util::SphericalToCartesian(model->GetModelCenterPoisition(), spherical);
				lcontrol->lightPos = Util::CalculateNewSphericalCoordinates(model->GetModelCenterPoisition(), lcontrol->lightPos, glm::radians(data.theta), glm::radians(data.phi));

				ImGui::End();
			}
			//���ò˵�����
			{
				// ��ȡ�˵����ĸ߶�
				float menuBarHeight = ImGui::GetFrameHeight();
				ImGui::SetNextWindowPos(ImVec2(float(0), float(menuBarHeight)));
				ImGui::SetNextWindowSize(ImVec2(float(static_cast<float>(data.width) / 5.0f), float(data.height - menuBarHeight)));//����ui��Ⱦ����
				ImGui::Begin(u8"���ò˵�", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
				{
					//ģ��
					if (ImGui::TreeNode(u8"ģ��")) {
						float maxWidth = ImGui::GetContentRegionAvail().x;
						if (ImGui::Button(u8"���ó���")) {
							glmanager->ClearModels();
						}
						ImGui::Checkbox(u8"����ϡ��", &data.sparsePoint);
						ImGui::SetNextItemWidth(maxWidth / 2.0f);
						ImGui::SliderFloat(u8"#Z�ᱶ��", &data.zFactor, 1.0f, 10.0f, "%.5f");
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
						for (int i = 0; i < 3; i++) {

							if (ImGui::Selectable(i == 0 ? point : i == 1 ? grid : surface, udata.selectedRadio == i)) {
								udata.selectedRadio = i;
								data.drawMode = i;
							}
						}
						ImGui::TreePop();
					}
					ImGui::Text(u8"");
					if (ImGui::TreeNode(u8"��ͼ")) {
						ImGui::Checkbox(u8"������ͼ", &data.useTexture);
						if (!data.useTexture) data.gammaCorrection = false;//��������ͼ٤��У��ȡ��
						ImGui::Text(u8"");
						float maxWidth = ImGui::GetContentRegionAvail().x;
						ImGui::SetNextItemWidth(maxWidth / 1.2f);
						ImGui::InputText("##��ͼ·��", udata.pathBuf, IM_ARRAYSIZE(udata.pathBuf));
						ImGui::SameLine();
						//ѡ��·��
						if (ImGui::Button(u8"...")) {
							//���ⲿ�ļ�����
							nfdchar_t* outPath = NULL;
							const nfdchar_t* filterList = "jpg;png;bmp";
							nfdresult_t result = NFD_OpenDialog(filterList, NULL, &outPath);
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
							glmanager->CreateModelTexture(udata.pathBuf, glmanager->GetCurrentModel(), nullptr, 0);
						}
						ImGui::TreePop();
					}
					ImGui::Text(u8"");
					float(*colors)[4] = data.colors;
					static float color[4] = { 255.0f, 255.0f, 255.0f, 255.0f };
					if (ImGui::TreeNode(u8"��ɫ")) {
						float maxWidth = ImGui::GetContentRegionAvail().x;
						if (ImGui::ColorButton("##colorbutton0", ImVec4(colors[0][0], colors[0][1], colors[0][2], colors[0][3]), ImGuiColorEditFlags_NoTooltip, ImVec2(maxWidth / 8.0f, maxWidth / 12.0f))) {
							colors[0][0] = color[0];
							colors[0][1] = color[1];
							colors[0][2] = color[2];
						}
						ImGui::SameLine();
						ImGui::Text(u8"������ɫ");
						if (ImGui::ColorButton("##colorbutton1", ImVec4(colors[1][0], colors[1][1], colors[1][2], colors[1][3]), ImGuiColorEditFlags_NoTooltip, ImVec2(maxWidth / 8.0f, maxWidth / 12.0f))) {
							colors[1][0] = color[0];
							colors[1][1] = color[1];
							colors[1][2] = color[2];
							//���²���
							Material& material = glmanager->GetCurrentModel()->material;
							material.diffuse = glm::vec3(colors[1][0], colors[1][1], colors[1][2]);
						}
						ImGui::SameLine();
						ImGui::Text(u8"ģ����ɫ");
						if (ImGui::ColorButton("##colorbutton2", ImVec4(colors[2][0], colors[2][1], colors[2][2], colors[2][3]), ImGuiColorEditFlags_NoTooltip, ImVec2(maxWidth / 8.0f, maxWidth / 12.0f))) {
							colors[2][0] = color[0];
							colors[2][1] = color[1];
							colors[2][2] = color[2];
						}
						ImGui::SameLine();
						ImGui::Text(u8"������ɫ");
						ImGui::ColorPicker4("##picker", color);
						ImGui::TreePop();
					}
					ImGui::Text(u8"");
					if (ImGui::TreeNode(u8"��������")) {
						ImGui::Checkbox(u8"���ù���", &data.useLight);
						float maxWidth = ImGui::GetContentRegionAvail().x;
						ImGui::Text(u8"1.ģ�Ͳ���");
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						Material& material = glmanager->GetCurrentModel()->material;
						ImGui::InputFloat(u8"##������x", &material.ambient[0], 0.0, 1.0, "%.10f");
						ImGui::SameLine();
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						ImGui::InputFloat(u8"##������y", &material.ambient[1], 0.0, 1.0, "%.10f");
						ImGui::SameLine();
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						ImGui::InputFloat(u8"##������z", &material.ambient[2], 0.0, 1.0, "%.10f");
						ImGui::SameLine();
						ImGui::Text(u8"������");
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						ImGui::InputFloat(u8"##������x", &material.diffuse[0], 0.0, 1.0, "%.10f");
						ImGui::SameLine();
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						ImGui::InputFloat(u8"##������y", &material.diffuse[1], 0.0, 1.0, "%.10f");
						ImGui::SameLine();
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						ImGui::InputFloat(u8"##������z", &material.diffuse[2], 0.0, 1.0, "%.10f");
						ImGui::SameLine();
						ImGui::Text(u8"������");
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						ImGui::InputFloat(u8"##���淴��x", &material.specular[0], 0.0, 1.0, "%.10f");
						ImGui::SameLine();
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						ImGui::InputFloat(u8"##���淴��y", &material.specular[1], 0.0, 1.0, "%.10f");
						ImGui::SameLine();
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						ImGui::InputFloat(u8"##���淴��z", &material.specular[2], 0.0, 1.0, "%.10f");
						ImGui::SameLine();
						ImGui::Text(u8"���淴��");
						ImGui::Text(u8"");
						ImGui::Text(u8"2.��ͼ����");
						ImGui::SetNextItemWidth(maxWidth / 3.0f);
						ImGui::InputDouble(u8"�Աȶ�", &data.alpha, 1.0, 10.0, "%.5f");
						ImGui::SetNextItemWidth(maxWidth / 3.0f);
						ImGui::InputInt(u8"����", &data.beta, 0, 100);
						if (ImGui::Button(u8"���þ�����ͼ")) {
							glmanager->ChangeModelSpecularImage(glmanager->GetCurrentModel(), data.alpha, data.beta);
						}
						ImGui::Text(u8"");
						ImGui::SliderFloat(u8"�����", &material.shininess, 0.00001f, 256.0f);
						ImGui::SetNextItemWidth(maxWidth / 3.0f);
						ImGui::InputFloat(u8"##�����", &material.shininess, 0.00001f, 256.0f, "%.10f");
						ImGui::Text(u8"");
						ImGui::Text(u8"3.�������");
						const char* parallelLight = u8"ƽ�й�";
						const char* linearPointLight = u8"���Ե��Դ";
						const char* nonlinearPointLight = u8"�����Ե��Դ";
						const char* spotlight = u8"�۹�";
						for (int i = 0; i < 4; i++) {
							if (ImGui::Selectable(i == 0 ? parallelLight : i == 1 ? linearPointLight :
								i == 2 ? nonlinearPointLight : spotlight, data.lightType == i)) {
								data.lightType = i;
							}
						}
						ImGui::Text(u8"");
						ImGui::Text(u8"4.����ϸ��");
						ImGui::Checkbox(u8"Blinn��ɫ", &data.blinn);
						ImGui::SameLine();
						ImGui::Checkbox(u8"٤��У��", &data.gammaCorrection);
						ImGui::Text(u8"");
						ImGui::Text(u8"5.��Դλ��");
						LightControl* lightControl = glmanager->GetLightControl();
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						ImGui::InputFloat(u8"##����λ��X", &lightControl->lightPos.x, 0.0, 1.0, "%.5f");
						ImGui::SameLine();
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						ImGui::InputFloat(u8"##����λ��Y", &lightControl->lightPos.y, 0.0, 1.0, "%.5f");
						ImGui::SameLine();
						ImGui::SetNextItemWidth(maxWidth / 6.0f);
						ImGui::InputFloat(u8"##����λ��Z", &lightControl->lightPos.z, 0.0, 1.0, "%.5f");
						ImGui::TreePop();
					}
					ImGui::Text(u8"");
					if (ImGui::TreeNode(u8"ϵͳ")) {
						ImGui::Checkbox(u8"����͸��", &data.transparentBg);
						ImGui::SameLine();
						ImGui::Checkbox(u8"�����޳�", &data.cullBackFace);
						ImGui::Text(u8"");
						ImGui::SliderFloat(u8"����ƶ��ٶ�", &data.moveSpeedUnit, 1.0f, 10.0f);
						ImGui::Text(u8"");
						ImGui::SliderFloat(u8"����ӽ�������", &data.sensitivity, 0.01f, 0.1f);
						ImGui::TreePop();
					}
				}
				ImGui::End();

			}
		}
		/// <summary>
		/// �����������ɫ��ʽ
		/// </summary>
		void UiManager::SetStyle(Data& data) {
			//���������ɫ
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
			//ȡ���߿���ʽ 
			style.Colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // ���ñ߿���ɫΪ͸��
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // ������Ӱ��ɫΪ͸��
			style.FrameBorderSize = 0.0f; // ���ñ߿���Ϊ0
			float transparent = data.transparentBg ? 0.0f : 0.94f;
			//���ñ����Ƿ�͸��
			style.Colors[ImGuiCol_WindowBg].w = transparent;
			style.Colors[ImGuiCol_PopupBg].w = transparent;

		}
	}
}
