// Copyright 2025 Pound Emulator Project. All rights reserved.

#pragma once

#include "../Panel.h"
#include <functional>
#include <unordered_map>
#include <string>

namespace Pound::GUI
{
    class ControllerPanel : public Panel
    {
    public:
        struct ButtonInfo
        {
            std::string name;
            bool pressed = false;
            std::function<void(bool)> callback;
        };
        ControllerPanel();

        void Render() override;

        void SetButtonCallback(const std::string &button_id, std::function<void(bool)> callback);
        void SetButtonState(const std::string &button_id, bool pressed);
        void SetStickPosition(const std::string &stick_id, float x, float y);
        void SetScale(float scale) { controller_scale = scale; }

    private:
        struct StickInfo
        {
            std::string name;
            float x = 0.0f;
            float y = 0.0f;
            std::function<void(float, float)> callback;
        };

        void RenderController();
        void DrawRoundedPath(ImDrawList *draw_list, const std::vector<ImVec2> &points, ImU32 color, float thickness);

        std::unordered_map<std::string, ButtonInfo> buttons;
        std::unordered_map<std::string, StickInfo> sticks;

        float controller_scale = 1.0f;
        bool show_button_labels = true;
        bool highlight_pressed = true;

        void InitializeButtons();
    };

} // namespace Pound::GUI