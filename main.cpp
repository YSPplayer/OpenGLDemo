#include "cwindow.h"
#include <sstream>
using namespace GL::Window;
int main(int argc, char* argv[]) {
	GL::Param* args = nullptr;
	if (argc == 6) {
		args = new GL::Param;
		std::istringstream(argv[1]) >> args->w;
		std::istringstream(argv[2]) >> args->h;
		std::istringstream(argv[3]) >> args->x;
		std::istringstream(argv[4]) >> args->y;
		std::istringstream(argv[5]) >> args->random;
	}
	CWindow* window = new CWindow;
	if(window->CreateWindow(800,600, args)) return window->Exe();
	return 0;
}