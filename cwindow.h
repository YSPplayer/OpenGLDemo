#pragma once
#include "GL/glManager.h"
#include "GL/data.h"
#include <GLFW/glfw3.h> //glfw3要后于glad包含，glad要优先加载Opengl的函数
#include "UI/uiManager.h"
#include <string>
/// <summary>
/// 自定义glfw窗口类
/// </summary>
namespace GL {
namespace Window {
	using namespace GL::UI;
#define DEFAULT_WINDOW_NAME "OpenGL"
	class CWindow {
		friend GlManager;
		public:
			CWindow();
			bool CreateCWindow(int width,int height, Param* args = nullptr,const std::string& name = DEFAULT_WINDOW_NAME);
			bool Exe();
		private:
			static bool shiftPressed;
			static bool shiftControl;
			static bool firstMouse;
			float deltaTime; //当前帧的时间差
			float lastFrame; //上一帧的时间
			static float lastX;
			static float lastY;
			static float lastX2;
			static float lastY2;
			static Data data;//传输的参数data类
			static bool mousePressed;
			static bool mouseRightPressed;
		/*	static bool rightButtonPressed;*/
			~CWindow(); 
			static void MouseButtonClick(GLFWwindow* window, int button, int action, int mods);
			static void MouseMoveUpdatePoint(GLFWwindow* window, double xposIn, double yposIn);
			void UpdateDeltaTime();
			void ProcessInput();
			void Render();
			static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
			void BindCallback();
			void OpenGLInit();
			static void MouseWheelUpdateScroll(GLFWwindow* window, double xoffset, double yoffset);
			GLFWwindow* window;//主窗口
			GlManager* glmanager;//GL管理对象
			UiManager* uimanager;//imggui对象
	};
}
}
