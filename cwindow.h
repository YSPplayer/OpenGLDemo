#pragma once
#include "glManager.h"
#include <glad/glad.h> 
#include <GLFW/glfw3.h> //glfw3Ҫ����glad������gladҪ���ȼ���Opengl�ĺ���
#include <string>
/// <summary>
/// �Զ���glfw������
/// </summary>
namespace GL {
namespace Window {
#define DEFAULT_WINDOW_NAME "OpenGL"
	class CWindow {
		public:
			CWindow();
			bool CreateWindow(int width,int height,const std::string& name = DEFAULT_WINDOW_NAME);
			bool Exe();
		private:
			static bool shiftPressed;
			static bool firstMouse;
			float deltaTime; //��ǰ֡��ʱ���
			float lastFrame; //��һ֡��ʱ��
			static float lastX;
			static float lastY;
			static float lastX2;
			static float lastY2;
			static float yaw;
			static float pitch;
			static Data data;//����Ĳ���data��
			static bool mousePressed;
		/*	static bool rightButtonPressed;*/
			~CWindow(); 
			static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
			static void UpdatePoint(GLFWwindow* window, double xpos, double ypos);
			void UpdateDeltaTime();
			void ProcessInput();
			void Render();
			static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
			void BindCallback();
			void OpenGLInit();
			void SetRenderData();
			static void UpdateScroll(GLFWwindow* window, double xoffset, double yoffset);
			GLFWwindow* window;//������
			GlManager* glmanager;//GL�������
	};
}
}
