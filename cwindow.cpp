#include "Util/util.h"
#include "cwindow.h"
#include <iostream>
namespace GL {
	namespace Window {
		using namespace Tool;
		float CWindow::lastX = 400.0f;
		float CWindow::lastY = 300.0f;
		float CWindow::lastX2 = 400.0f;
		float CWindow::lastY2 = 300.0f;
		bool CWindow::mousePressed = false;
		bool CWindow::shiftPressed = false;
		bool CWindow::firstMouse = false;
		Data CWindow::data;
		CWindow::CWindow() {
			window = nullptr;
			glmanager = new GlManager;
			deltaTime = 0.0f;
			lastFrame = 0.0f;
			data.aspect = DEFAULT_ASPECT;
			data.ambientStrength = 0.5f;
			data.specularStrength = 0.5f;
			data.lastRotationZ = 0.0f;
			data.lastRotationX = 0.0f;
			data.reflectivity = 3.0f;
			data.alpha = 2.0;
			data.lightType = NON_LINEAR_POINT_LIGHT;
			data.theta = 0.0f;
			data.zFactor = 1.0f;
			data.blinn = false;
			data.phi = 0.0f;
			data.showLightMode = false;
			data.beta = 50;
			data.useLight = true;
			data.useTexture = true;
			data.rotateZ = false;
			data.rotateX = false;
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

		void CWindow::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
			if (UiManager::MouseButtonCallback(window, button, action, mods)) return;
			CWindow* self = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				if (action == GLFW_PRESS) {
					self->mousePressed = true;
				}
				else if (action == GLFW_RELEASE) {
					self->mousePressed = false;
				}
			}
			else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
				/*	if (action == GLFW_PRESS) {
						self->rightButtonPressed = true;
					}
					else if (action == GLFW_RELEASE) {
						self->rightButtonPressed = false;
					}*/
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
			glfwSetMouseButtonCallback(window, MouseButtonCallback);
			glfwSetCursorPosCallback(window, UpdatePoint);
			glfwSetScrollCallback(window, UpdateScroll);
			glfwSetKeyCallback(window, KeyCallback);
		}

		void CWindow::UpdatePoint(GLFWwindow* window, double xposIn, double yposIn) {
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
				if (self->mousePressed) {
					if (self->lastX == 0.0 && self->lastY == 0.0) {
						self->lastX = xpos;
						self->lastY = ypos;
						return;
					}
					double xoffset = xpos - self->lastX;
					double yoffset = ypos - self->lastY;
					// ʹ�ø�С������������ƽ����ת
					const double zscaleFactor = 0.3;  // ��С���ֵ���Լ�С��ת��������
					const double xscaleFactor = 0.08;
					if (fabs(xoffset) > 8.0) {  // ֻ�е�����ƶ�����2����ʱ�Ÿ���
						data.rotateZ = true;
						data.rotationZ += xoffset * zscaleFactor;
					}
					if (fabs(yoffset) > 8.0) {  // ͬ������ֵ������Y��
						data.rotateX = true;
						data.rotationX += yoffset * xscaleFactor;
					}
					self->lastX = xpos;
					self->lastY = ypos;
				}
				else {
					self->lastX = 0.0;
					self->lastY = 0.0;
					data.lastRotationZ += data.rotationZ;
					data.lastRotationX += data.rotationX;
					data.rotationZ = 0.0f;
					data.rotationX = 0.0f;
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

		void CWindow::UpdateScroll(GLFWwindow* window, double xoffset, double yoffset) {
			if (data.aspect >= 0.0f && data.aspect <= 3.0f) data.aspect = data.aspect - (yoffset / ((20 - GlManager::aspectUnit) * 10.0f));
			if (data.aspect <= 0.0f)
				data.aspect = 0.0f;
			if (data.aspect >= 3.0f)
				data.aspect = 3.0f;
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
			uimanager->Render(data);//�Ȼ���ģ�ͣ�����Ⱦui��ui�㼶��ģ��֮��
			data.rotateZ = false;
			data.rotateX = false;
			data.isYaw = false;
			data.reset = false;
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
			UiManager::KeyCallback(window, key, scancode, action, mods);
		}
	}
}