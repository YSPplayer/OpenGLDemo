#include "cwindow.h"
#include <iostream>
namespace GL {
namespace Window {
	float CWindow::lastX = 400.0f;
	float CWindow::lastY = 300.0f;
	float CWindow::lastX2 = 400.0f;
	float CWindow::lastY2 = 300.0f;
	bool CWindow::mousePressed = false;
	bool CWindow::shiftPressed = false;
	bool CWindow::firstMouse = false;
	float CWindow::yaw = -90.0f;
	float CWindow::pitch = 0.0f;
	Data CWindow::data;
	CWindow::CWindow() {
		window = nullptr;
		glmanager = new GlManager;
		deltaTime = 0.0f;
		lastFrame = 0.0f;
		data.rotateZ = false;
		data.rotateX = false;
		data.lastRotationZ = 0.0f;
		data.lastRotationX = 0.0f;
		data.aspect = 0.3f;
		data.drawLine = false;
	}

	/// <summary>
	/// 创建glfw窗口
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="name"></param>
	/// <returns></returns>
	bool CWindow::CreateWindow(int width, int height, Param* args,const std::string& name) {
		OpenGLInit();
		window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
		if(window == nullptr) {
			glfwTerminate(); //释放glfw相关的内存
			return false;
		}
		glfwMakeContextCurrent(window); //设置glfw窗口当前的上下文 
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			glfwTerminate(); 
			return false;//加载opengl的函数指针
		} 
		//初始化管理器
		if(!glmanager->Init(args)) return false;
		glViewport(0, 0, width, height); //设置opengl的窗口大小，这里设置为和主窗口大小一样
		BindCallback();
		return true;
	}

	/// <summary>
	/// 窗口的阻塞渲染函数
	/// </summary>
	/// <returns></returns>
	bool CWindow::Exe() {
		// 启用 V-Sync
		glfwSwapInterval(1);
		while(!glfwWindowShouldClose(window)) {
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
		UpdateDeltaTime();
		data.moveSpeed =  1.0f * deltaTime;
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
		else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
			data.drawLine = !data.drawLine;
		else 
			data.moveType = MOVE_NONE;
	}

	/// <summary>
	/// 绑定glfw的回调函数
	/// </summary>
	void CWindow::BindCallback() {
		//每次窗口大小变化时同步更新opengl窗口的大小
		glfwSetFramebufferSizeCallback(window,[](GLFWwindow* window, int width, int height) {
			glViewport(0, 0, width, height);
		});
		glfwSetMouseButtonCallback(window, MouseButtonCallback);
		glfwSetCursorPosCallback(window, UpdatePoint);
		glfwSetScrollCallback(window, UpdateScroll);
		glfwSetKeyCallback(window, KeyCallback);
	}

	void CWindow::UpdatePoint(GLFWwindow* window, double xpos, double ypos) {
		CWindow* self = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
		if (shiftPressed) {
			if (self->mousePressed) {
				if (firstMouse) {
					lastX2 = xpos;
					lastY2 = ypos;
					firstMouse = false;
				}
				double xoffset = xpos - lastX2;
				double yoffset = lastY2 - ypos;
				lastX2 = xpos;
				lastY2 = ypos;
				GLfloat sensitivity = 0.05;
				xoffset *= sensitivity;
				yoffset *= sensitivity;
				yaw += xoffset;
				pitch += yoffset;
				if (pitch > 89.0f)
					pitch = 89.0f;
				if (pitch < -89.0f)
					pitch = -89.0f;
				data.yaw = yaw;
				data.pitch = pitch;
				data.isYaw = true;
			}
			else {
			
			}
		}	
		else {
			if (self->mousePressed) {
				if (self->lastX == 0.0 && self->lastY == 0.0) {
					self->lastX = xpos;
					self->lastY = ypos;
					return;
				}
				double xoffset = xpos - self->lastX;
				double yoffset = ypos - self->lastY;
				// 使用更小的缩放因子以平滑旋转
				const double scaleFactor = 0.3;  // 减小这个值可以减小旋转的灵敏度
				if (fabs(xoffset) > 3.0) {  // 只有当鼠标移动超过2像素时才更新
					data.rotateZ = true;
					data.rotationZ += xoffset * scaleFactor;
				}
				if (fabs(yoffset) > 3.0) {  // 同样的阈值适用于Y轴
					data.rotateX = true;
					data.rotationX += yoffset * scaleFactor;
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
		double currentFrameTime = glfwGetTime();
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
		if (data.aspect >= 0.0f && data.aspect <= 3.0f)
			data.aspect = data.aspect - (yoffset / 10.0f);
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
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwGetWindowSize(window, &data.width, &data.height);//获取到当前窗口的宽高
		glmanager->Render(data);
		data.rotateZ = false;
		data.rotateX = false;
		data.isYaw = false;
		data.reset = false;
	}

	void CWindow::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
			if (action == GLFW_PRESS) {
				shiftPressed = true;
			}
			else if (action == GLFW_RELEASE) {
				shiftPressed = false;
				//mouseDragging = false; // 停止拖拽
			}
		}
	}
}
}