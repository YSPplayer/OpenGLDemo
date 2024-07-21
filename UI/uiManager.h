#pragma once
struct GLFWwindow;
namespace GL {
	namespace UI {
		class UiManager {
		public:
			UiManager();
			~UiManager();
			void Init(GLFWwindow* window);
			void Render(int width,int height);
		private:
			void Draw();
		};
	}
}
