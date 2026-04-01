#pragma once
#include "Game/IGameModule.h"
#include "Testbench/Game/TestbenchGameState.h"
#include <memory>

class TestbenchModule final : public Ryu::Game::IGameModule
{
	static constexpr auto AppName = "Testbench App";

public:
	std::shared_ptr<Ryu::App::App> CreateApplication(std::shared_ptr<Ryu::Window::Window> window) override;
	constexpr const char* GetName() const override { return AppName; }
};
