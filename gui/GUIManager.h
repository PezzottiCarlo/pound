// Copyright 2025 Pound Emulator Project. All rights reserved.

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include "Window.h"
#include "Panel.h"

namespace Pound::GUI
{
    struct TabItem
    {
        std::string name;
        std::string shortcut;
        bool *p_selected = nullptr;
        std::function<void()> render_callback;
        std::function<void(bool)> checked_callback;
        struct TabBar *nested_tabs = nullptr;
        std::string linked_panel_name; // Nome del panel collegato
    };

    struct TabBar
    {
        std::string id;
        std::vector<TabItem> items;
    };

    class GUIManager
    {
    public:
        GUIManager();
        ~GUIManager();

        bool Initialize(const std::string &title, int width, int height);
        void Shutdown();
        void RunFrame();

        void AddPanel(std::shared_ptr<Panel> panel, const std::string &shortcut = "");
        void RemovePanel(const std::string &name);
        std::shared_ptr<Panel> GetPanel(const std::string &name) const;

        // Tab management
        TabBar *AddTabs(const std::string &name);
        TabBar *GetTabBar(const std::string &name) const;
        
        // Nuovi metodi per collegare panel alle tab
        void AddPanelTab(TabBar *parent_bar, const std::string &tab_name, 
                        const std::string &panel_name, const std::string &shortcut = "");
        void AddPanelTab(TabBar *parent_bar, const std::string &tab_name, 
                        std::shared_ptr<Panel> panel, const std::string &shortcut = "");
        
        // Metodi esistenti per tab generiche
        void AddSubTab(TabBar *parent_bar, const std::string &name, std::function<void()> callback);
        void AddSubTab(TabBar *parent_bar, const std::string &name, const std::string &shortcut, std::function<void()> callback);
        void AddSubTab(TabBar *parent_bar, const std::string &name, bool *p_selected, std::function<void(bool)> callback);
        void AddSubTab(TabBar *parent_bar, const std::string &name, bool *p_selected, const std::string &shortcut, std::function<void(bool)> callback);

        bool IsRunning() const { return running; }
        Window *GetWindow() { return window.get(); }

        void SetShowDemoWindow(bool show) { show_demo_window = show; }

    private:
        void BeginFrame();
        void EndFrame();
        void ApplyTheme();
        void RenderTabBars();
        void RenderTabBarContents(TabBar &bar);
        void HandlePanelToggle(const std::string &panel_name);
        void SetupDefaultMenus();
        void UpdateViewMenu();

        std::unique_ptr<Window> window;
        std::vector<std::shared_ptr<Panel>> panels;
        std::vector<std::unique_ptr<TabBar>> m_tab_bars;
        std::unordered_map<std::string, bool> panel_visibility_states;
        
        TabBar* view_menu = nullptr;
        
        bool running = false;
        bool show_demo_window = false;
        bool menus_initialized = false;
    };
}