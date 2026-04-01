#include "Testbench/TestbenchModule.h"
#include "Testbench/TestbenchApp.h"

using namespace Ryu;
using namespace Ryu::Game;

RYU_IMPLEMENT_GAME_MODULE(TestbenchModule);

std::shared_ptr<Ryu::App::App> TestbenchModule::CreateApplication(std::shared_ptr<Ryu::Window::Window> window)
{
	return std::make_shared<TestbenchApp>(window);
}
