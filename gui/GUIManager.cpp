// Copyright 2025 Pound Emulator Project. All rights reserved.

#include "GUIManager.h"
#include "Colors.h"
#include "Base/Logging/Log.h"
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include <SDL3/SDL_opengl.h>
#include <algorithm>

namespace Pound::GUI
{

    GUIManager::GUIManager() = default;

    GUIManager::~GUIManager()
    {
        Shutdown();
    }
    TabBar *GUIManager::GetTabBar(const std::string &name) const
    {
        auto it = std::find_if(m_tab_bars.begin(), m_tab_bars.end(),
                               [&name](const std::unique_ptr<TabBar>& bar)
                               {
                                   return bar->id == name;
                               });
        return (it != m_tab_bars.end()) ? it->get() : nullptr;
    }

    bool GUIManager::Initialize(const std::string &title, int width, int height)
    {
        window = std::make_unique<Window>();

        if (!window->Initialize(title, width, height))
        {
            LOG_ERROR(Render, "Failed to initialize window");
            return false;
        }

        // Initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // Setup style
        ApplyTheme();

        // Setup platform/renderer backends
        ImGui_ImplSDL3_InitForOpenGL(window->GetSDLWindow(), window->GetGLContext());
        ImGui_ImplOpenGL3_Init("#version 330");

        // Setup default menus
        SetupDefaultMenus();

        running = true;
        return true;
    }

    void GUIManager::Shutdown()
    {
        if (!running)
            return;

        panels.clear();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

        window.reset();
        running = false;
    }

    void GUIManager::SetupDefaultMenus()
    {
        // Create the default menus in order
        AddTabs("File");
        AddTabs("Emulation");
        
        // View menu is special - we keep a reference to it
        view_menu = AddTabs("View");
        
        AddTabs("Help");
        
        menus_initialized = true;
    }

    void GUIManager::RunFrame()
    {
        if (!running)
            return;

        window->ProcessEvents();

        BeginFrame();
        RenderTabBars();

        for (auto &panel : panels)
        {
            if (panel->IsVisible())
            {
                panel->Render();
            }
        }

        if (show_demo_window)
        {
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        EndFrame();
    }

    void GUIManager::BeginFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    void GUIManager::EndFrame()
    {
        ImGui::Render();

        ImGuiIO &io = ImGui::GetIO();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window->SwapBuffers();
    }

    void GUIManager::ApplyTheme()
    {
        ImGuiStyle &style = ImGui::GetStyle();

        // Modern theme with custom colors
        style.WindowRounding = 8.0f;
        style.FrameRounding = 4.0f;
        style.PopupRounding = 4.0f;
        style.ScrollbarRounding = 6.0f;
        style.GrabRounding = 4.0f;
        style.TabRounding = 4.0f;

        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
        style.WindowMenuButtonPosition = ImGuiDir_Right;

        // Apply custom color scheme
        style.Colors[ImGuiCol_Text] = Colors::Text;
        style.Colors[ImGuiCol_TextDisabled] = Colors::TextDisabled;
        style.Colors[ImGuiCol_WindowBg] = Colors::WithAlpha(Colors::Background, 0.95f);
        style.Colors[ImGuiCol_ChildBg] = Colors::BackgroundDark;
        style.Colors[ImGuiCol_PopupBg] = Colors::WithAlpha(Colors::Background, 0.94f);
        style.Colors[ImGuiCol_Border] = Colors::Border;
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg] = Colors::BackgroundLight;
        style.Colors[ImGuiCol_FrameBgHovered] = Colors::Lighten(Colors::BackgroundLight, 0.1f);
        style.Colors[ImGuiCol_FrameBgActive] = Colors::Lighten(Colors::BackgroundLight, 0.2f);
        style.Colors[ImGuiCol_TitleBg] = Colors::BackgroundDark;
        style.Colors[ImGuiCol_TitleBgActive] = Colors::Background;
        style.Colors[ImGuiCol_TitleBgCollapsed] = Colors::WithAlpha(Colors::BackgroundDark, 0.51f);
        style.Colors[ImGuiCol_MenuBarBg] = Colors::BackgroundDark;
        style.Colors[ImGuiCol_ScrollbarBg] = Colors::WithAlpha(Colors::BackgroundDark, 0.53f);
        style.Colors[ImGuiCol_ScrollbarGrab] = Colors::BackgroundLight;
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = Colors::Lighten(Colors::BackgroundLight, 0.1f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = Colors::Lighten(Colors::BackgroundLight, 0.2f);
        style.Colors[ImGuiCol_CheckMark] = Colors::Primary;
        style.Colors[ImGuiCol_SliderGrab] = Colors::Primary;
        style.Colors[ImGuiCol_SliderGrabActive] = Colors::PrimaryActive;
        style.Colors[ImGuiCol_Button] = Colors::WithAlpha(Colors::Primary, 0.40f);
        style.Colors[ImGuiCol_ButtonHovered] = Colors::PrimaryHover;
        style.Colors[ImGuiCol_ButtonActive] = Colors::PrimaryActive;
        style.Colors[ImGuiCol_Header] = Colors::WithAlpha(Colors::Primary, 0.31f);
        style.Colors[ImGuiCol_HeaderHovered] = Colors::WithAlpha(Colors::Primary, 0.80f);
        style.Colors[ImGuiCol_HeaderActive] = Colors::Primary;
        style.Colors[ImGuiCol_Separator] = Colors::Border;
        style.Colors[ImGuiCol_SeparatorHovered] = Colors::WithAlpha(Colors::Primary, 0.78f);
        style.Colors[ImGuiCol_SeparatorActive] = Colors::Primary;
        style.Colors[ImGuiCol_ResizeGrip] = Colors::WithAlpha(Colors::Primary, 0.25f);
        style.Colors[ImGuiCol_ResizeGripHovered] = Colors::WithAlpha(Colors::Primary, 0.67f);
        style.Colors[ImGuiCol_ResizeGripActive] = Colors::WithAlpha(Colors::Primary, 0.95f);
        style.Colors[ImGuiCol_Tab] = Colors::BackgroundLight;
        style.Colors[ImGuiCol_TabHovered] = Colors::WithAlpha(Colors::Primary, 0.80f);
        style.Colors[ImGuiCol_TabActive] = Colors::Primary;
        style.Colors[ImGuiCol_TabUnfocused] = Colors::Background;
        style.Colors[ImGuiCol_TabUnfocusedActive] = Colors::Lighten(Colors::Background, 0.1f);
        style.Colors[ImGuiCol_PlotLines] = Colors::Primary;
        style.Colors[ImGuiCol_PlotLinesHovered] = Colors::PrimaryHover;
        style.Colors[ImGuiCol_PlotHistogram] = Colors::Secondary;
        style.Colors[ImGuiCol_PlotHistogramHovered] = Colors::SecondaryHover;
        style.Colors[ImGuiCol_TextSelectedBg] = Colors::WithAlpha(Colors::Primary, 0.35f);
        style.Colors[ImGuiCol_DragDropTarget] = Colors::WithAlpha(Colors::Secondary, 0.90f);
        style.Colors[ImGuiCol_NavHighlight] = Colors::Primary;
        style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }

    void GUIManager::AddPanel(std::shared_ptr<Panel> panel, const std::string &shortcut)
    {
        // Add the panel to our list
        panels.push_back(panel);
        
        // Initialize visibility state
        panel_visibility_states[panel->GetName()] = panel->IsVisible();
        
        // If menus are initialized, add to View menu
        if (menus_initialized && view_menu)
        {
            UpdateViewMenu();
        }
    }

    void GUIManager::RemovePanel(const std::string &name)
    {
        panels.erase(
            std::remove_if(panels.begin(), panels.end(),
                           [&name](const std::shared_ptr<Panel> &panel)
                           {
                               return panel->GetName() == name;
                           }),
            panels.end());
        
        panel_visibility_states.erase(name);
        
        // Update the View menu
        if (menus_initialized && view_menu)
        {
            UpdateViewMenu();
        }
    }

    std::shared_ptr<Panel> GUIManager::GetPanel(const std::string &name) const
    {
        auto it = std::find_if(panels.begin(), panels.end(),
                               [&name](const std::shared_ptr<Panel> &panel)
                               {
                                   return panel->GetName() == name;
                               });
        return (it != panels.end()) ? *it : nullptr;
    }

    void GUIManager::UpdateViewMenu()
    {
        if (!view_menu)
            return;
        
        // Clear existing items
        view_menu->items.clear();
        
        // Add menu items for each panel
        for (const auto& panel : panels)
        {
            TabItem new_item;
            new_item.name = panel->GetName();
            new_item.p_selected = &panel_visibility_states[panel->GetName()];
            
            // Capture panel by value in the lambda
            std::weak_ptr<Panel> weak_panel = panel;
            new_item.checked_callback = [this, weak_panel](bool checked) {
                if (auto panel = weak_panel.lock())
                {
                    panel->SetVisible(checked);
                    panel_visibility_states[panel->GetName()] = checked;
                }
            };
            
            // Add shortcut if needed (you can pass this as parameter or generate automatically)
            // For now, we'll leave it empty
            new_item.shortcut = "";
            
            view_menu->items.push_back(std::move(new_item));
        }
    }

    TabBar *GUIManager::AddTabs(const std::string &name)
    {
        // Ensure View is always in third position
        if (menus_initialized && name != "View")
        {
            // Find the position to insert
            size_t insert_pos = m_tab_bars.size();
            
            // If we have 2 or more tabs and this isn't View, make sure View stays in position 2 (index 2)
            if (m_tab_bars.size() >= 2)
            {
                // Check if View exists
                auto view_it = std::find_if(m_tab_bars.begin(), m_tab_bars.end(),
                    [](const std::unique_ptr<TabBar>& bar) { return bar->id == "View"; });
                
                if (view_it != m_tab_bars.end() && name != "Help")
                {
                    // If adding before View position, insert normally
                    if (m_tab_bars.size() < 3)
                    {
                        insert_pos = 2; // Put View at position 2
                    }
                }
            }
        }
        
        auto new_bar = std::make_unique<TabBar>();
        new_bar->id = name;
        m_tab_bars.push_back(std::move(new_bar));
        return m_tab_bars.back().get();
    }

    void GUIManager::AddPanelTab(TabBar *parent_bar, const std::string &tab_name, 
                                 const std::string &panel_name, const std::string &shortcut)
    {
        if (!parent_bar)
        {
            LOG_WARNING(Render, "Trying to add a panel-tab to a null TabBar.");
            return;
        }

        auto panel = GetPanel(panel_name);
        if (!panel)
        {
            LOG_WARNING(Render, "Panel '%s' not found.", panel_name.c_str());
            return;
        }

        TabItem new_item;
        new_item.name = tab_name;
        new_item.shortcut = shortcut;
        new_item.linked_panel_name = panel_name;
        new_item.p_selected = &panel_visibility_states[panel_name];
        new_item.checked_callback = [this, panel_name](bool checked) {
            HandlePanelToggle(panel_name);
        };
        parent_bar->items.push_back(std::move(new_item));
    }

    void GUIManager::AddPanelTab(TabBar *parent_bar, const std::string &tab_name, 
                                 std::shared_ptr<Panel> panel, const std::string &shortcut)
    {
        if (!parent_bar || !panel)
        {
            LOG_WARNING(Render, "Invalid parameters for AddPanelTab.");
            return;
        }

        // Assicurati che il panel sia stato aggiunto
        auto existing = GetPanel(panel->GetName());
        if (!existing)
        {
            AddPanel(panel);
        }

        AddPanelTab(parent_bar, tab_name, panel->GetName(), shortcut);
    }

    void GUIManager::HandlePanelToggle(const std::string &panel_name)
    {
        auto panel = GetPanel(panel_name);
        if (panel)
        {
            bool new_state = panel_visibility_states[panel_name];
            panel->SetVisible(new_state);
        }
    }

    void GUIManager::AddSubTab(TabBar *parent_bar, const std::string &name, std::function<void()> callback)
    {
        if (!parent_bar)
        {
            LOG_WARNING(Render, "Trying to add a sub-tab to a null TabBar.");
            return;
        }

        TabItem new_item;
        new_item.name = name;
        new_item.render_callback = std::move(callback);
        parent_bar->items.push_back(std::move(new_item));
    }

    void GUIManager::AddSubTab(TabBar *parent_bar, const std::string &name, const std::string &shortcut, std::function<void()> callback)
    {
        if (!parent_bar)
        {
            LOG_WARNING(Render, "Trying to add a sub-tab to a null TabBar.");
            return;
        }

        TabItem new_item;
        new_item.name = name;
        new_item.shortcut = shortcut;
        new_item.render_callback = std::move(callback);
        parent_bar->items.push_back(std::move(new_item));
    }

    void GUIManager::AddSubTab(TabBar *parent_bar, const std::string &name, bool *p_selected, std::function<void(bool)> callback)
    {
        if (!parent_bar)
        {
            LOG_WARNING(Render, "Trying to add a sub-tab to a null TabBar.");
            return;
        }

        TabItem new_item;
        new_item.name = name;
        new_item.p_selected = p_selected;
        new_item.checked_callback  = std::move(callback);
        parent_bar->items.push_back(std::move(new_item));
    }

    void GUIManager::AddSubTab(TabBar *parent_bar, const std::string &name, bool *p_selected, const std::string &shortcut, std::function<void(bool)> callback)
    {
        if (!parent_bar)
        {
            LOG_WARNING(Render, "Trying to add a sub-tab to a null TabBar.");
            return;
        }

        TabItem new_item;
        new_item.name = name;
        new_item.shortcut = shortcut;
        new_item.p_selected = p_selected;
        new_item.checked_callback  = std::move(callback);
        parent_bar->items.push_back(std::move(new_item));
    }

    void GUIManager::RenderTabBarContents(TabBar &bar)
    {
        for (auto &item : bar.items)
        {
            if (item.nested_tabs)
            {
                if (ImGui::BeginMenu(item.name.c_str()))
                {
                    RenderTabBarContents(*item.nested_tabs);
                    ImGui::EndMenu();
                }
            }
            else
            {
                const char *shortcut = item.shortcut.empty() ? nullptr : item.shortcut.c_str();
                
                // Se è collegato a un panel, usa il checkbox
                if (!item.linked_panel_name.empty() || item.p_selected)
                {
                    if (ImGui::MenuItem(item.name.c_str(), shortcut, item.p_selected))
                    {
                        if (item.checked_callback)
                        {
                            item.checked_callback(*item.p_selected);
                        }
                    }
                }
                else
                {
                    // Tab normale senza panel collegato
                    if (ImGui::MenuItem(item.name.c_str(), shortcut))
                    {
                        if (item.render_callback)
                        {
                            item.render_callback();
                        }
                    }
                }
            }
        }
    }

    void GUIManager::RenderTabBars()
    {
        if (ImGui::BeginMainMenuBar())
        {
            for (const auto &bar_ptr : m_tab_bars)
            {
                if (ImGui::BeginMenu(bar_ptr->id.c_str()))
                {
                    RenderTabBarContents(*bar_ptr);
                    ImGui::EndMenu();
                }
            }
            ImGui::EndMainMenuBar();
        }
    }
}