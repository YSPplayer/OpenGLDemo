#include "Util/util.h"
#include "cwindow.h"
#include <iostream>
#ifndef M_PI
#define M_PI 3.1415926535897931
#endif
namespace GL {
	namespace Window {
		using namespace Tool;
		float CWindow::lastX = 400.0f;
		float CWindow::lastY = 300.0f;
		float CWindow::lastX2 = 400.0f;
		float CWindow::lastY2 = 300.0f;
		bool CWindow::mousePressed = false;
		bool CWindow::mouseRightPressed = false;
		bool CWindow::shiftPressed = false;
		bool CWindow::shiftControl = false;
		bool CWindow::firstMouse = false;
		Data CWindow::data;
		CWindow::CWindow() {
			window = nullptr;
			glmanager = new GlManager;
			deltaTime = 0.0f;
			lastFrame = 0.0f;
			data.aspect = DEFAULT_ASPECT;
			data.parallel = DEFAULT_PARALLEL;
			data.isParallel = false;
			data.ambientStrength = 0.5f;
			data.specularStrength = 0.5f;
			data.lastRotationZ = 0.0f;
			data.lastRotationX = 0.0f;
			data.lastMoveX = 0.0f;
			data.lastMoveY = 0.0f;
			data.reflectivity = 3.0f;
			data.alpha = 2.0;
			data.lightType = NON_LINEAR_POINT_LIGHT;
			data.theta = 0.0f;
			data.zFactor = 1.0f;
			data.zScalingMutiple = 1.0f;
			data.blinn = false;
			data.isZScalingMutiple = false;
			data.phi = 0.0f;
			data.modelSensitivity = 1.0;//��ֵԽС,������Խ��
			data.showLightMode = false;
			data.beta = 50;
			data.useLight = true;
			data.useTexture = true;
			data.angleLimite = false;
			data.rotateXZ = false;
			data.moveXY = false;
			data.useColorMap = true;
			data.gammaCorrection = true;
			data.yaw = -90.0f;
			data.pitch = 0.0f;
			data.transparentBg = true;
			data.sparsePoint = true;
			data.useNormalTexture = true;
			data.textureFlip = false;
			bool load = false;
			if (!(load = Util::LoadConfig(data, UiManager::udata))) {
				data.sensitivity = 0.1f;
				data.moveSpeedUnit = 1.0f;
				data.cullBackFace = false;
				data.drawMode = DRAW_MODE_SURFACE;
				//��ʼ����ɫ
				for (int i = 0; i < 3; ++i) {
					data.colors[i][0] = 0.0f;
					data.colors[i][1] = 0.0f;
					data.colors[i][2] = 0.0f;
					data.colors[i][3] = 1.0f;
				}
			}
			uimanager = new UiManager(glmanager, load);

		}

		/// <summary>
		/// ����glfw����
		/// </summary>
		/// <param name="width"></param>
		/// <param name="height"></param>
		/// <param name="name"></param>
		/// <returns></returns>
		bool CWindow::CreateCWindow(int width, int height, Param* args, const std::string& name) {
			OpenGLInit();
			window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
			if (window == nullptr) {
				glfwTerminate(); //�ͷ�glfw��ص��ڴ�
				return false;
			}
			glfwMakeContextCurrent(window); //����glfw���ڵ�ǰ�������� 
			glfwSwapInterval(1); // Enable vsync
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
				glfwTerminate();
				return false;//����opengl�ĺ���ָ��
			}
			//��ʼ��������
			if (!glmanager->Init(args)) return false;
			glViewport(0, 0, width, height); //����opengl�Ĵ��ڴ�С����������Ϊ�������ڴ�Сһ��
			glDisable(GL_CULL_FACE); //Ĭ�Ͻ������޳�����
			glCullFace(GL_BACK); // �޳�����
			glFrontFace(GL_CCW); // ��ʱ��Ϊ����
			glEnable(GL_DEPTH_TEST);//������Ȳ��ԣ���Ȼģ����Ⱦ����ȷ
			//��ʼ��imggui
			uimanager->Init(window);
			BindCallback();
			return true;
		}

		/// <summary>
		/// ���ڵ�������Ⱦ����
		/// </summary>
		/// <returns></returns>
		bool CWindow::Exe() {
			while (!glfwWindowShouldClose(window)) {
				UpdateDeltaTime();//����֡���ٶ�
				ProcessInput();//���������¼�
				glfwPollEvents(); //�����¼��������¼��Ĵ���
				Render();
				glfwSwapBuffers(window);//˫�����������Ⱦͼ�Σ�ǰ����������ʾͼ�񣬺󻺳�����ͼ����ƣ���ֹͼ����˸��ʾ
			}
			glmanager->ClearModels();
			glfwTerminate();
			return true;
		}
		CWindow::~CWindow() {}

		void CWindow::MouseButtonClick(GLFWwindow* window, int button, int action, int mods) {
			if (UiManager::MouseButtonCallback(window, button, action, mods)) return;
			CWindow* self = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				if (action == GLFW_PRESS) {
					self->mousePressed = true;
					double xpos, ypos;
					glfwGetCursorPos(window, &xpos, &ypos);
					self->lastX = xpos;
					self->lastY = ypos;
				}   
				else if (action == GLFW_RELEASE) {
					self->mousePressed = false;
					self->lastX = 0.0;
					self->lastY = 0.0;
				}
			}
			else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
				if (action == GLFW_PRESS) {
					self->mouseRightPressed = true;
					double xpos, ypos;
					glfwGetCursorPos(window, &xpos, &ypos);
					self->lastX = xpos;
					self->lastY = ypos;
				}
				else if (action == GLFW_RELEASE) {
					self->mouseRightPressed = false;
					self->lastX = 0.0;
					self->lastY = 0.0;
				}
			}
		}

		/// <summary>
		/// ����¼��ļ���
		/// </summary>
		void CWindow::ProcessInput() {
			data.moveSpeed = data.moveSpeedUnit * deltaTime;
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				data.moveType = MOVE_FORWARD;
			else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				data.moveType = MOVE_BACK;
			else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				data.moveType = MOVE_LEFT;
			else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				data.moveType = MOVE_RIGHT;
			else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) //����ģ��λ��
				data.reset = true;
			else
				data.moveType = MOVE_NONE;
		}

		/// <summary>
		/// ��glfw�Ļص�����
		/// </summary>
		void CWindow::BindCallback() {
			//ÿ�δ��ڴ�С�仯ʱͬ������opengl���ڵĴ�С
			glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
				glViewport(0, 0, width, height);
				});
			glfwSetMouseButtonCallback(window, MouseButtonClick);
			glfwSetCursorPosCallback(window, MouseMoveUpdatePoint);
			glfwSetScrollCallback(window, MouseWheelUpdateScroll);
			glfwSetKeyCallback(window, KeyCallback);
		}

		void CWindow::MouseMoveUpdatePoint(GLFWwindow* window, double xposIn, double yposIn) {
			if (UiManager::CursorPosCallback(window, xposIn, yposIn)) return;
			CWindow* self = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
			//��������ӽ�
			float xpos = static_cast<float>(xposIn);
			float ypos = static_cast<float>(yposIn);
			if (shiftPressed) {
				if (firstMouse)
				{
					lastX2 = xpos;
					lastY2 = ypos;
					firstMouse = false;
				}
				float xoffset = xpos - lastX2;
				float yoffset = lastY2 - ypos;
				lastX2 = xpos;
				lastY2 = ypos;
				xoffset *= data.sensitivity;
				yoffset *= data.sensitivity;
				data.yaw += xoffset;
				data.pitch += yoffset;
				if (data.pitch > 89.0f)
					data.pitch = 89.0f;
				if (data.pitch < -89.0f)
					data.pitch = -89.0f;
				data.isYaw = true;
			}
			else {
				firstMouse = true;//��������ӽǵ��ƶ�����
				if (self->mousePressed) {//�������
					if (data.angleLimite) {
						float increment_z;
						data.lastRotationZ = data.lastRotationZ + ((xpos - self->lastX) * data.modelSensitivity / 3.0f);
						int axle = 0;
						if (data.lastRotationZ > 0.0f) increment_z = static_cast<int>(data.lastRotationZ) % 360;
						else increment_z = static_cast<float>(static_cast<int>(data.lastRotationZ) % 360) + 360.0f; // �������ǶȽ���ģ������
						if (increment_z > 90.0f && increment_z <= 180.0f) axle = 1;
						else if (increment_z > 180.0f && increment_z <= 270.0f) axle = 2;
						else if (increment_z > 270.0f && increment_z <= 360.0f) axle = 3;
						else axle = 0;
						if (data.lastRotationX > 0 && data.lastRotationX < 90) axle = (axle + 2) % 4; 
						self->lastX = xpos;
						float increment_x = ((ypos - self->lastY) * data.modelSensitivity / 3.0f);
						if ((data.lastRotationX + increment_x <= 90.0f && data.lastRotationX + increment_x >= 0.0f)
							|| (data.lastRotationX + increment_x < 0.0f && data.lastRotationX + increment_x >= -90.0f)) {
							data.lastRotationX += (ypos - self->lastY) / 3.0f;
							self->lastY = ypos;
						}
						data.rotateXZ = true;
					}
					else {
						data.lastRotationZ += (xpos - self->lastX) * data.modelSensitivity / 3.0f;
						data.lastRotationZ = Util::NormalizeAngle(data.lastRotationZ, 360.0f); // �淶���Ƕȵ�0-360��
						self->lastX = xpos;
						float increment_x = (ypos - self->lastY) * data.modelSensitivity / 3.0f;
						data.lastRotationX += increment_x;
						data.lastRotationX = Util::NormalizeAngle(data.lastRotationX, 360.0f); // ͬ���淶��X��Ƕ�
						data.rotateXZ = true;
						self->lastY = ypos;
					}
				}
				else if (self->mouseRightPressed) {
					//DEFAULT_ASPECT
					float offset_X = (xpos - self->lastX) / 1000.0f * (data.aspect / DEFAULT_ASPECT);
					float offset_Y = (self->lastY - ypos) / 1000.0f * (data.aspect / DEFAULT_ASPECT);
					data.lastMoveX += offset_X;
					data.lastMoveY += offset_Y;
					self->lastX = xpos;
					self->lastY = ypos;
					data.moveXY = true;
				}
			}
			data.enable = self->mousePressed;
		}

		void CWindow::UpdateDeltaTime() {
			float currentFrameTime = static_cast<float>(glfwGetTime());
			deltaTime = currentFrameTime - lastFrame;
			lastFrame = currentFrameTime;
		}

		/// <summary>
		/// ��ʼ��opengl���������
		/// </summary>
		void CWindow::OpenGLInit() {
			glfwInit(); //��ʼ��glfw��
			//����opengl�İ汾��Ϊ3.3
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			//ָ��ʹ��Opengl�ĺ��������ļ�����ʹ�þɵĹ���
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}

		void CWindow::MouseWheelUpdateScroll(GLFWwindow* window, double xoffset, double yoffset) {
			/*if (data.aspect >= 0.0f && data.aspect <= 3.0f) data.aspect = data.aspect - (yoffset / ((20 - GlManager::aspectUnit) * 10.0f));
			if (data.aspect <= 0.0f)
				data.aspect = 0.0f;
			if (data.aspect >= 3.0f)
				data.aspect = 3.0f;*/
			if (shiftControl) { //�Ŵ�-��СZ��
				data.isZScalingMutiple = true;
				if (yoffset < 0 && (data.zScalingMutiple / 2.0f) >= 1e-5) data.zScalingMutiple /= 1.5f;
				else if (yoffset > 0 && (data.zScalingMutiple + 0.5f) <= 40) data.zScalingMutiple += 0.5f;
			}
			else {
				if (data.isParallel) { //ƽ���ӿ�
					if (yoffset > 0) data.parallel /= 1.30f;
					else data.parallel *= 1.30f;
				}
				else { //͸���ӿ�
					//data.aspect -= yoffset / 60.0f * static_cast<float>(M_PI) / (6 * 180.0f);
					//data.aspect = data.aspect < M_PI / 360 ? static_cast<float>(M_PI) / 360 : data.aspect;
					//data.aspect = data.aspect > 3.14f ? 3.14f : data.aspect;
					if (data.aspect >= 0.0f && data.aspect <= 3.0f) data.aspect = data.aspect - (yoffset / ((20 - GlManager::aspectUnit) * 10.0f));
					if (data.aspect <= 0.0f)
						data.aspect = 0.0f;
					if (data.aspect >= 3.0f)
						data.aspect = 3.0f;
				}
			}
		}

		/// <summary>
		/// opengl��Ⱦ����
		/// </summary>
		void CWindow::Render() {
			//��Ⱦָ��
			glClearColor(data.colors[0][0], data.colors[0][1], data.colors[0][2], data.colors[0][3]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//������Ȳ�����ҪGL_DEPTH_BUFFER_BIT
			glfwGetWindowSize(window, &data.width, &data.height);//��ȡ����ǰ���ڵĿ��
			glmanager->Render(data);
			data.reset = false;
			uimanager->Render(data);//�Ȼ���ģ�ͣ�����Ⱦui��ui�㼶��ģ��֮��
			data.rotateXZ = false;
			data.isZScalingMutiple = false;
			data.moveXY = false;
			data.isYaw = false;
		}

		void CWindow::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
			if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
				if (action == GLFW_PRESS) {
					shiftPressed = true;
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//����shiftʱ���������
				}
				else if (action == GLFW_RELEASE) {
					shiftPressed = false;
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); //����ģʽ����ʾ���
				}
			}
			else if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) {
				if (action == GLFW_PRESS) {
					shiftControl = true;
				}
				else if (action == GLFW_RELEASE) {
					shiftControl = false;
				}
			}
			UiManager::KeyCallback(window, key, scancode, action, mods);
		}
	}
}