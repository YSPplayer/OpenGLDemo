#include "cwindow.h"
using namespace GL::Window;
int main() {
	CWindow* window = new CWindow;
	if(window->CreateWindow(800,600)) return window->Exe();
	return 0;
}