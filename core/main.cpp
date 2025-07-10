// Copyright 2025 Pound Emulator Project. All rights reserved.

#include <thread>
#include <memory>
#include <chrono>

#include "Base/Logging/Backend.h"
#include "Base/Config.h"
#include "ARM/cpu.h"
#include "JIT/jit.h"

#include "gui/GUIManager.h"
#include "gui/panels/ConsolePanel.h"
#include "gui/panels/CPUPanel.h"
#include "gui/panels/PerformancePanel.h"
#include "gui/panels/ControllerPanel.h"

std::shared_ptr<Pound::GUI::ConsolePanel> console_panel;
std::shared_ptr<Pound::GUI::CPUPanel> cpu_panel;
std::shared_ptr<Pound::GUI::PerformancePanel> performance_panel;
std::shared_ptr<Pound::GUI::ControllerPanel> controller_panel;
// CPU test function
void cpuTest()
{
    CPU cpu;
    cpu.pc = 0;

    cpu.write_byte(0, 0x05); // MOVZ placeholder
    cpu.write_byte(4, 0x03); // ADD placeholder
    cpu.write_byte(8, 0xFF); // RET placeholder
    LOG_INFO(ARM, "{}", cpu.read_byte(0));
    JIT jit;
    jit.translate_and_run(cpu);
    cpu.print_debug_information();
    LOG_INFO(ARM, "X0 = {}", cpu.x(0));

    if (cpu_panel)
        cpu_panel->UpdateState(cpu);
}

void initGUI(Pound::GUI::GUIManager *gui_manager)
{
    gui_manager->AddPanel(console_panel);
    gui_manager->AddPanel(cpu_panel);
    gui_manager->AddPanel(performance_panel);
    gui_manager->AddPanel(controller_panel);

    Pound::GUI::TabBar *file_menu = gui_manager->AddTabs("File");
    Pound::GUI::TabBar *emulation_menu = gui_manager->AddTabs("Emulation");
    Pound::GUI::TabBar *help_menu = gui_manager->AddTabs("Help");

    gui_manager->AddSubTab(file_menu, "Open ROM...", []() {});
    gui_manager->AddSubTab(file_menu, "Exit", []()
                           {
        LOG_INFO(Render, "Exiting Pound Emulator");
        std::exit(0); });

    gui_manager->AddSubTab(emulation_menu, "Run CPU Test", []()
                           { cpuTest(); });
    gui_manager->AddSubTab(emulation_menu, "Pause", []()
                           { LOG_INFO(Render, "Pausing emulation (not implemented yet)"); });
    gui_manager->AddSubTab(emulation_menu, "Resume", []()
                           { LOG_INFO(Render, "Resuming emulation (not implemented yet)"); });
                           
    gui_manager->AddSubTab(help_menu, "About", []()
                           { LOG_INFO(Render, "Pound Emulator is a pre-alpha project. Visit our GitHub for more information."); });

    cpu_panel->SetCPUTestCallback(cpuTest);
    console_panel->AddLog("[INFO] Pound Emulator started");
    console_panel->AddLog("[INFO] Version: Pre-Alpha");
}

int main()
{
    Base::Log::Initialize();
    Base::Log::Start();

    const auto config_dir = Base::FS::GetUserPath(Base::FS::PathType::BinaryDir);
    Config::Load(config_dir / "config.toml");

    auto gui_manager = std::make_unique<Pound::GUI::GUIManager>();
    if (!gui_manager->Initialize("Pound Emulator", Config::windowWidth(), Config::windowHeight()))
    {
        LOG_ERROR(Render, "Failed to initialize GUI");
        return -1;
    }

    console_panel = std::make_shared<Pound::GUI::ConsolePanel>();
    cpu_panel = std::make_shared<Pound::GUI::CPUPanel>();
    performance_panel = std::make_shared<Pound::GUI::PerformancePanel>();
    controller_panel = std::make_shared<Pound::GUI::ControllerPanel>();

    initGUI(gui_manager.get());

    while (gui_manager->IsRunning())
    {
        gui_manager->RunFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    gui_manager->Shutdown();

    return 0;
}
