#include "util.h"
namespace GL {
	namespace Tool {
		std::random_device Util::rd;
		std::mt19937 Util::gen(rd());
		std::wstring Util::rootPath = Util::InitPath();
	}
}