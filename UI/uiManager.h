#pragma once
#include "../GL/glManager.h"
struct GLFWwindow;
namespace GL {
	namespace UI {
		class UiManager {
		public:
			UiManager(GlManager* glmanager);
			~UiManager();
			void Init(GLFWwindow* window);
			void Render(Data& data);
			static bool MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
			static bool CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
			static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		private:
			void Draw(Data& data);
			void SetStyle();
			GlManager* glmanager;//该对象的内存在主窗口中释放
			static UData udata;
		};
	}
}
