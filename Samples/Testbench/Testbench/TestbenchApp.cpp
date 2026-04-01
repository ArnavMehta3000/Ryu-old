#include "Testbench/TestbenchApp.h"

#include "Application/App/Utils/PathManager.h"
#include "Core/Profiling/Profiling.h"
#include "Engine/Engine.h"
#include "Game/Components/TransformComponent.h"
#include "Game/World/Entity.h"
#include "Testbench/Game/TestbenchWorld.h"

using namespace Ryu;
using namespace Ryu::Window;

TestbenchApp::TestbenchApp(const std::shared_ptr<Ryu::Window::Window>& window)
	: App::App(window)
{
}

bool TestbenchApp::OnInit()
{
	if (!App::OnInit())  // This sets `m_isRunning` to true
	{
		return false;
	}

	RYU_PROFILE_SCOPE();
	RYU_LOG_INFO("Initializing Testbench App");

	if (!m_gameInput.Init())
	{
		RYU_LOG_WARN("Failed to initialize GameInput");
	}

	// Bind input stuff
	Game::InputManager* input = Engine::Engine::Get().GetInputManager();
	input->BindAction("Quit", KeyCode::Escape, [this]{ RequestQuit(); });
	input->BindAction("ToggleStats", KeyCode::F, [this]{ RYU_LOG_INFO("DT: {} | FPS: {}", m_timer.DeltaTimeF(), m_timer.FPS()); });

	m_worldManager.CreateWorld<TestbenchWorld>();

	//TestSerialization();

	return true;
}

void TestbenchApp::OnShutdown()
{
	RYU_PROFILE_SCOPE();
	RYU_LOG_INFO("Shutting down Testbench App");

	m_gameInput.Shutdown();
}

void TestbenchApp::OnTick(const Ryu::Utils::FrameTimer& timer)
{
	RYU_PROFILE_SCOPE();
	m_gameInput.PollKeyboard();
	m_gameInput.PollMouse();

	m_timer = timer;

	m_worldManager.OnTick(timer);
}

void TestbenchApp::TestSerialization()
{
	// Serialization removed - no longer needed
}
