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
			void Render(int width,int height);
			static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
			static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
		private:
			void Draw(int width,int height);
			void SetStyle();
			GlManager* glmanager;//该对象的内存在主窗口中释放
		};
	}
}
