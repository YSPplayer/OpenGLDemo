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
	/// ����glfw����
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="name"></param>
	/// <returns></returns>
	bool CWindow::CreateWindow(int width, int height, Param* args,const std::string& name) {
		OpenGLInit();
		window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
		if(window == nullptr) {
			glfwTerminate(); //�ͷ�glfw��ص��ڴ�
			return false;
		}
		glfwMakeContextCurrent(window); //����glfw���ڵ�ǰ�������� 
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			glfwTerminate(); 
			return false;//����opengl�ĺ���ָ��
		} 
		//��ʼ��������
		if(!glmanager->Init(args)) return false;
		glViewport(0, 0, width, height); //����opengl�Ĵ��ڴ�С����������Ϊ�������ڴ�Сһ��
		BindCallback();
		return true;
	}

	/// <summary>
	/// ���ڵ�������Ⱦ����
	/// </summary>
	/// <returns></returns>
	bool CWindow::Exe() {
		// ���� V-Sync
		glfwSwapInterval(1);
		while(!glfwWindowShouldClose(window)) {
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
		else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) //����ģ��λ��
			data.reset = true;
		else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
			data.drawLine = !data.drawLine;
		else 
			data.moveType = MOVE_NONE;
	}

	/// <summary>
	/// ��glfw�Ļص�����
	/// </summary>
	void CWindow::BindCallback() {
		//ÿ�δ��ڴ�С�仯ʱͬ������opengl���ڵĴ�С
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
				// ʹ�ø�С������������ƽ����ת
				const double scaleFactor = 0.3;  // ��С���ֵ���Լ�С��ת��������
				if (fabs(xoffset) > 3.0) {  // ֻ�е�����ƶ�����2����ʱ�Ÿ���
					data.rotateZ = true;
					data.rotationZ += xoffset * scaleFactor;
				}
				if (fabs(yoffset) > 3.0) {  // ͬ������ֵ������Y��
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
		if (data.aspect >= 0.0f && data.aspect <= 3.0f)
			data.aspect = data.aspect - (yoffset / 10.0f);
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
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwGetWindowSize(window, &data.width, &data.height);//��ȡ����ǰ���ڵĿ��
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
				//mouseDragging = false; // ֹͣ��ק
			}
		}
	}
}
}