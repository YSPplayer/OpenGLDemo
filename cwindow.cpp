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
				//初始化颜色
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
		/// 创建glfw窗口
		/// </summary>
		/// <param name="width"></param>
		/// <param name="height"></param>
		/// <param name="name"></param>
		/// <returns></returns>
		bool CWindow::CreateCWindow(int width, int height, Param* args, const std::string& name) {
			OpenGLInit();
			window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
			if (window == nullptr) {
				glfwTerminate(); //释放glfw相关的内存
				return false;
			}
			glfwMakeContextCurrent(window); //设置glfw窗口当前的上下文 
			glfwSwapInterval(1); // Enable vsync
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
				glfwTerminate();
				return false;//加载opengl的函数指针
			}
			//初始化管理器
			if (!glmanager->Init(args)) return false;
			glViewport(0, 0, width, height); //设置opengl的窗口大小，这里设置为和主窗口大小一样
			glDisable(GL_CULL_FACE); //默认禁用面剔除功能
			glCullFace(GL_BACK); // 剔除背面
			glFrontFace(GL_CCW); // 逆时针为正面
			glEnable(GL_DEPTH_TEST);//启用深度测试，不然模型渲染不正确
			//初始化imggui
			uimanager->Init(window);
			BindCallback();
			return true;
		}

		/// <summary>
		/// 窗口的阻塞渲染函数
		/// </summary>
		/// <returns></returns>
		bool CWindow::Exe() {
			while (!glfwWindowShouldClose(window)) {
				UpdateDeltaTime();//更新帧率速度
				ProcessInput();//监听按键事件
				glfwPollEvents(); //接收事件，用于事件的触发
				Render();
				glfwSwapBuffers(window);//双缓冲机制来渲染图形，前缓冲用于显示图像，后缓冲用于图像绘制，防止图像闪烁显示
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
		/// 鼠标事件的监听
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
			else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) //重置模型位置
				data.reset = true;
			else
				data.moveType = MOVE_NONE;
		}

		/// <summary>
		/// 绑定glfw的回调函数
		/// </summary>
		void CWindow::BindCallback() {
			//每次窗口大小变化时同步更新opengl窗口的大小
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
			//更换相机视角
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
				firstMouse = true;//重置相机视角的移动变量
				if (self->mousePressed) {
					if (self->lastX == 0.0 && self->lastY == 0.0) {
						self->lastX = xpos;
						self->lastY = ypos;
						return;
					}
					double xoffset = xpos - self->lastX;
					double yoffset = ypos - self->lastY;
					// 使用更小的缩放因子以平滑旋转
					const double zscaleFactor = 0.3;  // 减小这个值可以减小旋转的灵敏度
					const double xscaleFactor = 0.08;
					if (fabs(xoffset) > 8.0) {  // 只有当鼠标移动超过2像素时才更新
						data.rotateZ = true;
						data.rotationZ += xoffset * zscaleFactor;
					}
					if (fabs(yoffset) > 8.0) {  // 同样的阈值适用于Y轴
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
		/// 初始化opengl的相关配置
		/// </summary>
		void CWindow::OpenGLInit() {
			glfwInit(); //初始化glfw库
			//设置opengl的版本号为3.3
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			//指定使用Opengl的核心配置文件，不使用旧的功能
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
		/// opengl渲染代码
		/// </summary>
		void CWindow::Render() {
			//渲染指令
			glClearColor(data.colors[0][0], data.colors[0][1], data.colors[0][2], data.colors[0][3]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//启用深度测试需要GL_DEPTH_BUFFER_BIT
			glfwGetWindowSize(window, &data.width, &data.height);//获取到当前窗口的宽高
			glmanager->Render(data);
			uimanager->Render(data);//先绘制模型，后渲染ui，ui层级在模型之上
			data.rotateZ = false;
			data.rotateX = false;
			data.isYaw = false;
			data.reset = false;
		}

		void CWindow::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
			if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
				if (action == GLFW_PRESS) {
					shiftPressed = true;
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//按下shift时候隐藏鼠标
				}
				else if (action == GLFW_RELEASE) {
					shiftPressed = false;
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); //正常模式下显示鼠标
				}
			}
			UiManager::KeyCallback(window, key, scancode, action, mods);
		}
	}
}