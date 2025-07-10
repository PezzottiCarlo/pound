// Copyright 2025 Pound Emulator Project. All rights reserved.

#include "ControllerPanel.h"
#include "../Colors.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <cmath>

namespace Pound::GUI
{
    ControllerPanel::ControllerPanel() : Panel("Controller")
    {
        InitializeButtons();
    }
    
    void ControllerPanel::InitializeButtons()
    {
        // Initialize all buttons
        buttons["button-a"] = { "A Button", false, nullptr };
        buttons["button-b"] = { "B Button", false, nullptr };
        buttons["button-x"] = { "X Button", false, nullptr };
        buttons["button-y"] = { "Y Button", false, nullptr };
        buttons["plus-button"] = { "Plus Button (+)", false, nullptr };
        buttons["minus-button"] = { "Minus Button (-)", false, nullptr };
        buttons["home-button"] = { "Home Button", false, nullptr };
        buttons["capture-button"] = { "Capture Button", false, nullptr };
        buttons["left-button"] = { "L Button", false, nullptr };
        buttons["right-button"] = { "R Button", false, nullptr };
        buttons["dpad-up"] = { "D-Pad Up", false, nullptr };
        buttons["dpad-down"] = { "D-Pad Down", false, nullptr };
        buttons["dpad-left"] = { "D-Pad Left", false, nullptr };
        buttons["dpad-right"] = { "D-Pad Right", false, nullptr };
        
        // Initialize sticks
        sticks["l-stick"] = { "Left Stick", 0.0f, 0.0f, nullptr };
        sticks["r-stick"] = { "Right Stick", 0.0f, 0.0f, nullptr };
    }
    
    void ControllerPanel::SetButtonCallback(const std::string& button_id, std::function<void(bool)> callback)
    {
        if (buttons.find(button_id) != buttons.end())
        {
            buttons[button_id].callback = callback;
        }
    }
    
    void ControllerPanel::SetButtonState(const std::string& button_id, bool pressed)
    {
        if (buttons.find(button_id) != buttons.end())
        {
            buttons[button_id].pressed = pressed;
        }
    }
    
    void ControllerPanel::SetStickPosition(const std::string& stick_id, float x, float y)
    {
        if (sticks.find(stick_id) != sticks.end())
        {
            sticks[stick_id].x = x;
            sticks[stick_id].y = y;
        }
    }
    
    void ControllerPanel::Render()
    {
        if (!visible)
        {
            return;
        }
        
        if (!ImGui::Begin(name.c_str(), &visible, ImGuiWindowFlags_NoCollapse))
        {
            ImGui::End();
            return;
        }
        
        // Controller settings
        ImGui::Text("Controller Settings:");
        ImGui::SliderFloat("Scale", &controller_scale, 1.0f, 4.0f);
        ImGui::Checkbox("Show Button Labels", &show_button_labels);
        ImGui::Checkbox("Highlight Pressed Buttons", &highlight_pressed);
        
        ImGui::Separator();
        
        // Render the controller
        RenderController();
        
        // Show button states
        if (ImGui::CollapsingHeader("Button States"))
        {
            ImGui::Columns(2, nullptr, false);
            for (const auto& [id, info] : buttons)
            {
                ImGui::Text("%s: %s", info.name.c_str(), info.pressed ? "Pressed" : "Released");
                ImGui::NextColumn();
            }
            ImGui::Columns(1);
            
            ImGui::Spacing();
            ImGui::Text("Left Stick: (%.2f, %.2f)", sticks["l-stick"].x, sticks["l-stick"].y);
            ImGui::Text("Right Stick: (%.2f, %.2f)", sticks["r-stick"].x, sticks["r-stick"].y);
        }
        
        ImGui::End();
    }
    
    // Helper function to handle button interaction
    void HandleButtonInteraction(const char* id, const ImVec2& pos, const ImVec2& size, Pound::GUI::ControllerPanel::ButtonInfo& button)
    {
        ImGui::SetCursorScreenPos(pos);
        ImGui::InvisibleButton(id, size);

        bool new_state = ImGui::IsItemActive();
        if (new_state != button.pressed)
        {
            button.pressed = new_state;
            if (button.callback)
            {
                button.callback(new_state);
            }
        }
    }

    void ControllerPanel::RenderController()
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
        ImVec2 canvas_size = ImVec2(600.0f * controller_scale, 300.0f * controller_scale);
        
        // Draw controller background
        ImVec2 controller_center = ImVec2(canvas_pos.x + canvas_size.x * 0.5f, canvas_pos.y + canvas_size.y * 0.5f);
        
        // Simplified controller shape (rounded rectangle)
        draw_list->AddRectFilled(
            ImVec2(canvas_pos.x + 50 * controller_scale, canvas_pos.y + 50 * controller_scale),
            ImVec2(canvas_pos.x + 550 * controller_scale, canvas_pos.y + 250 * controller_scale),
            IM_COL32(139, 139, 139, 255),
            30.0f * controller_scale
        );
        
        // --- START D-Pad INTERACTION ---
        ImVec2 dpad_center = ImVec2(canvas_pos.x + 150 * controller_scale, canvas_pos.y + 120 * controller_scale);
        float dpad_size = 30 * controller_scale;

        // D-Pad background
        draw_list->AddRectFilled(
            ImVec2(dpad_center.x - dpad_size, dpad_center.y - dpad_size/3),
            ImVec2(dpad_center.x + dpad_size, dpad_center.y + dpad_size/3),
            IM_COL32(102, 101, 101, 255)
        );
        draw_list->AddRectFilled(
            ImVec2(dpad_center.x - dpad_size/3, dpad_center.y - dpad_size),
            ImVec2(dpad_center.x + dpad_size/3, dpad_center.y + dpad_size),
            IM_COL32(102, 101, 101, 255)
        );
        
        // Highlight and handle interaction for D-Pad buttons
        ImVec2 dpad_up_pos = ImVec2(dpad_center.x - dpad_size/3, dpad_center.y - dpad_size);
        ImVec2 dpad_up_size = ImVec2(dpad_size*2/3, dpad_size*2/3);
        HandleButtonInteraction("dpad-up", dpad_up_pos, dpad_up_size, buttons["dpad-up"]);
        if (highlight_pressed && buttons["dpad-up"].pressed) draw_list->AddRectFilled(dpad_up_pos, ImVec2(dpad_up_pos.x + dpad_up_size.x, dpad_up_pos.y + dpad_up_size.y), IM_COL32(51, 153, 255, 255));

        ImVec2 dpad_down_pos = ImVec2(dpad_center.x - dpad_size/3, dpad_center.y + dpad_size/3);
        ImVec2 dpad_down_size = ImVec2(dpad_size*2/3, dpad_size*2/3);
        HandleButtonInteraction("dpad-down", dpad_down_pos, dpad_down_size, buttons["dpad-down"]);
        if (highlight_pressed && buttons["dpad-down"].pressed) draw_list->AddRectFilled(dpad_down_pos, ImVec2(dpad_down_pos.x + dpad_down_size.x, dpad_down_pos.y + dpad_down_size.y), IM_COL32(51, 153, 255, 255));

        ImVec2 dpad_left_pos = ImVec2(dpad_center.x - dpad_size, dpad_center.y - dpad_size/3);
        ImVec2 dpad_left_size = ImVec2(dpad_size*2/3, dpad_size*2/3);
        HandleButtonInteraction("dpad-left", dpad_left_pos, dpad_left_size, buttons["dpad-left"]);
        if (highlight_pressed && buttons["dpad-left"].pressed) draw_list->AddRectFilled(dpad_left_pos, ImVec2(dpad_left_pos.x + dpad_left_size.x, dpad_left_pos.y + dpad_left_size.y), IM_COL32(51, 153, 255, 255));
        
        ImVec2 dpad_right_pos = ImVec2(dpad_center.x + dpad_size/3, dpad_center.y - dpad_size/3);
        ImVec2 dpad_right_size = ImVec2(dpad_size*2/3, dpad_size*2/3);
        HandleButtonInteraction("dpad-right", dpad_right_pos, dpad_right_size, buttons["dpad-right"]);
        if (highlight_pressed && buttons["dpad-right"].pressed) draw_list->AddRectFilled(dpad_right_pos, ImVec2(dpad_right_pos.x + dpad_right_size.x, dpad_right_pos.y + dpad_right_size.y), IM_COL32(51, 153, 255, 255));
        // --- END D-Pad INTERACTION ---

        // --- START ABXY INTERACTION ---
        ImVec2 abxy_center = ImVec2(canvas_pos.x + 450 * controller_scale, canvas_pos.y + 120 * controller_scale);
        float button_radius = 20 * controller_scale;
        float button_spacing = 35 * controller_scale;
        
        // A button (right)
        ImVec2 a_pos = ImVec2(abxy_center.x + button_spacing, abxy_center.y);
        draw_list->AddCircleFilled(a_pos, button_radius, buttons["button-a"].pressed && highlight_pressed ? IM_COL32(51, 153, 255, 255) : IM_COL32(102, 101, 101, 255));
        if (show_button_labels) draw_list->AddText(ImVec2(a_pos.x - 5 * controller_scale, a_pos.y - 8 * controller_scale), IM_COL32(255, 255, 255, 255), "A");
        HandleButtonInteraction("button-a", ImVec2(a_pos.x - button_radius, a_pos.y - button_radius), ImVec2(button_radius * 2, button_radius * 2), buttons["button-a"]);

        // B button (bottom)
        ImVec2 b_pos = ImVec2(abxy_center.x, abxy_center.y + button_spacing);
        draw_list->AddCircleFilled(b_pos, button_radius, buttons["button-b"].pressed && highlight_pressed ? IM_COL32(51, 153, 255, 255) : IM_COL32(102, 101, 101, 255));
        if (show_button_labels) draw_list->AddText(ImVec2(b_pos.x - 5 * controller_scale, b_pos.y - 8 * controller_scale), IM_COL32(255, 255, 255, 255), "B");
        HandleButtonInteraction("button-b", ImVec2(b_pos.x - button_radius, b_pos.y - button_radius), ImVec2(button_radius * 2, button_radius * 2), buttons["button-b"]);

        // X button (top)
        ImVec2 x_pos = ImVec2(abxy_center.x, abxy_center.y - button_spacing);
        draw_list->AddCircleFilled(x_pos, button_radius, buttons["button-x"].pressed && highlight_pressed ? IM_COL32(51, 153, 255, 255) : IM_COL32(102, 101, 101, 255));
        if (show_button_labels) draw_list->AddText(ImVec2(x_pos.x - 5 * controller_scale, x_pos.y - 8 * controller_scale), IM_COL32(255, 255, 255, 255), "X");
        HandleButtonInteraction("button-x", ImVec2(x_pos.x - button_radius, x_pos.y - button_radius), ImVec2(button_radius * 2, button_radius * 2), buttons["button-x"]);

        // Y button (left)
        ImVec2 y_pos = ImVec2(abxy_center.x - button_spacing, abxy_center.y);
        draw_list->AddCircleFilled(y_pos, button_radius, buttons["button-y"].pressed && highlight_pressed ? IM_COL32(51, 153, 255, 255) : IM_COL32(102, 101, 101, 255));
        if (show_button_labels) draw_list->AddText(ImVec2(y_pos.x - 5 * controller_scale, y_pos.y - 8 * controller_scale), IM_COL32(255, 255, 255, 255), "Y");
        HandleButtonInteraction("button-y", ImVec2(y_pos.x - button_radius, y_pos.y - button_radius), ImVec2(button_radius * 2, button_radius * 2), buttons["button-y"]);
        // --- END ABXY INTERACTION ---

        // --- START ANALOG STICK INTERACTION ---
        float stick_base_radius = 30 * controller_scale;
        float stick_radius = 20 * controller_scale;
        
        // Left stick
        ImVec2 left_stick_pos = ImVec2(canvas_pos.x + 200 * controller_scale, canvas_pos.y + 180 * controller_scale);
        draw_list->AddCircleFilled(left_stick_pos, stick_base_radius, IM_COL32(220, 221, 221, 255));
        
        // Handle dragging for Left Stick
        ImGui::SetCursorScreenPos(ImVec2(left_stick_pos.x - stick_base_radius, left_stick_pos.y - stick_base_radius));
        ImGui::InvisibleButton("l-stick_interaction", ImVec2(stick_base_radius * 2, stick_base_radius * 2));
        if (ImGui::IsItemActive()) {
            ImVec2 mouse_delta = ImGui::GetMouseDragDelta(0, 0.0f);
            float max_dist = stick_base_radius * 0.7f;
            float current_dist = std::sqrt(mouse_delta.x * mouse_delta.x + mouse_delta.y * mouse_delta.y);
            if (current_dist > max_dist) {
                mouse_delta.x = (mouse_delta.x / current_dist) * max_dist;
                mouse_delta.y = (mouse_delta.y / current_dist) * max_dist;
            }
            sticks["l-stick"].x = mouse_delta.x / max_dist;
            sticks["l-stick"].y = -mouse_delta.y / max_dist; // Invert Y-axis
        } else {
             if (sticks["l-stick"].x != 0.0f || sticks["l-stick"].y != 0.0f) {
                sticks["l-stick"].x = 0.0f;
                sticks["l-stick"].y = 0.0f;
             }
        }
        
        ImVec2 left_stick_actual = ImVec2(
            left_stick_pos.x + sticks["l-stick"].x * stick_base_radius * 0.7f,
            left_stick_pos.y - sticks["l-stick"].y * stick_base_radius * 0.7f
        );
        draw_list->AddCircleFilled(left_stick_actual, stick_radius, IM_COL32(102, 101, 101, 255));

        // Right stick
        ImVec2 right_stick_pos = ImVec2(canvas_pos.x + 400 * controller_scale, canvas_pos.y + 180 * controller_scale);
        draw_list->AddCircleFilled(right_stick_pos, stick_base_radius, IM_COL32(220, 221, 221, 255));

        // Handle dragging for Right Stick
        ImGui::SetCursorScreenPos(ImVec2(right_stick_pos.x - stick_base_radius, right_stick_pos.y - stick_base_radius));
        ImGui::InvisibleButton("r-stick_interaction", ImVec2(stick_base_radius * 2, stick_base_radius * 2));
        if (ImGui::IsItemActive()) {
            ImVec2 mouse_delta = ImGui::GetMouseDragDelta(0, 0.0f);
            float max_dist = stick_base_radius * 0.7f;
            float current_dist = std::sqrt(mouse_delta.x * mouse_delta.x + mouse_delta.y * mouse_delta.y);
            if (current_dist > max_dist) {
                mouse_delta.x = (mouse_delta.x / current_dist) * max_dist;
                mouse_delta.y = (mouse_delta.y / current_dist) * max_dist;
            }
            sticks["r-stick"].x = mouse_delta.x / max_dist;
            sticks["r-stick"].y = -mouse_delta.y / max_dist; // Invert Y-axis
        } else {
             if (sticks["r-stick"].x != 0.0f || sticks["r-stick"].y != 0.0f) {
                sticks["r-stick"].x = 0.0f;
                sticks["r-stick"].y = 0.0f;
             }
        }
        
        ImVec2 right_stick_actual = ImVec2(
            right_stick_pos.x + sticks["r-stick"].x * stick_base_radius * 0.7f,
            right_stick_pos.y - sticks["r-stick"].y * stick_base_radius * 0.7f
        );
        draw_list->AddCircleFilled(right_stick_actual, stick_radius, IM_COL32(102, 101, 101, 255));
        // --- END ANALOG STICK INTERACTION ---
        
        // --- START SHOULDER BUTTON INTERACTION ---
        float shoulder_width = 60 * controller_scale;
        float shoulder_height = 20 * controller_scale;
        
        // L button
        ImVec2 l_button_pos = ImVec2(canvas_pos.x + 100 * controller_scale, canvas_pos.y + 30 * controller_scale);
        draw_list->AddRectFilled(l_button_pos, ImVec2(l_button_pos.x + shoulder_width, l_button_pos.y + shoulder_height), buttons["left-button"].pressed && highlight_pressed ? IM_COL32(51, 153, 255, 255) : IM_COL32(102, 101, 101, 255), 5.0f * controller_scale);
        if (show_button_labels) draw_list->AddText(ImVec2(l_button_pos.x + 25 * controller_scale, l_button_pos.y + 2 * controller_scale), IM_COL32(255, 255, 255, 255), "L");
        HandleButtonInteraction("left-button", l_button_pos, ImVec2(shoulder_width, shoulder_height), buttons["left-button"]);
        
        // R button
        ImVec2 r_button_pos = ImVec2(canvas_pos.x + 440 * controller_scale, canvas_pos.y + 30 * controller_scale);
        draw_list->AddRectFilled(r_button_pos, ImVec2(r_button_pos.x + shoulder_width, r_button_pos.y + shoulder_height), buttons["right-button"].pressed && highlight_pressed ? IM_COL32(51, 153, 255, 255) : IM_COL32(102, 101, 101, 255), 5.0f * controller_scale);
        if (show_button_labels) draw_list->AddText(ImVec2(r_button_pos.x + 25 * controller_scale, r_button_pos.y + 2 * controller_scale), IM_COL32(255, 255, 255, 255), "R");
        HandleButtonInteraction("right-button", r_button_pos, ImVec2(shoulder_width, shoulder_height), buttons["right-button"]);
        // --- END SHOULDER BUTTON INTERACTION ---

        // --- START MISC BUTTONS INTERACTION ---
        float small_button_radius = 15 * controller_scale;
        
        // Minus button
        ImVec2 minus_pos = ImVec2(canvas_pos.x + 250 * controller_scale, canvas_pos.y + 80 * controller_scale);
        draw_list->AddCircleFilled(minus_pos, small_button_radius, buttons["minus-button"].pressed && highlight_pressed ? IM_COL32(51, 153, 255, 255) : IM_COL32(102, 101, 101, 255));
        draw_list->AddRectFilled(ImVec2(minus_pos.x - 8 * controller_scale, minus_pos.y - 2 * controller_scale), ImVec2(minus_pos.x + 8 * controller_scale, minus_pos.y + 2 * controller_scale), IM_COL32(35, 25, 22, 255));
        HandleButtonInteraction("minus-button", ImVec2(minus_pos.x - small_button_radius, minus_pos.y - small_button_radius), ImVec2(small_button_radius * 2, small_button_radius * 2), buttons["minus-button"]);
        
        // Plus button
        ImVec2 plus_pos = ImVec2(canvas_pos.x + 350 * controller_scale, canvas_pos.y + 80 * controller_scale);
        draw_list->AddCircleFilled(plus_pos, small_button_radius, buttons["plus-button"].pressed && highlight_pressed ? IM_COL32(51, 153, 255, 255) : IM_COL32(102, 101, 101, 255));
        draw_list->AddRectFilled(ImVec2(plus_pos.x - 8 * controller_scale, plus_pos.y - 2 * controller_scale), ImVec2(plus_pos.x + 8 * controller_scale, plus_pos.y + 2 * controller_scale), IM_COL32(35, 25, 22, 255));
        draw_list->AddRectFilled(ImVec2(plus_pos.x - 2 * controller_scale, plus_pos.y - 8 * controller_scale), ImVec2(plus_pos.x + 2 * controller_scale, plus_pos.y + 8 * controller_scale), IM_COL32(35, 25, 22, 255));
        HandleButtonInteraction("plus-button", ImVec2(plus_pos.x - small_button_radius, plus_pos.y - small_button_radius), ImVec2(small_button_radius * 2, small_button_radius * 2), buttons["plus-button"]);

        float tiny_button_size = 12 * controller_scale;
        
        // Home button
        ImVec2 home_pos = ImVec2(canvas_pos.x + 320 * controller_scale, canvas_pos.y + 120 * controller_scale);
        draw_list->AddCircleFilled(home_pos, tiny_button_size, buttons["home-button"].pressed && highlight_pressed ? IM_COL32(51, 153, 255, 255) : IM_COL32(102, 101, 101, 255));
        HandleButtonInteraction("home-button", ImVec2(home_pos.x - tiny_button_size, home_pos.y - tiny_button_size), ImVec2(tiny_button_size*2, tiny_button_size*2), buttons["home-button"]);

        // Capture button
        ImVec2 capture_pos = ImVec2(canvas_pos.x + 280 * controller_scale, canvas_pos.y + 120 * controller_scale);
        draw_list->AddRectFilled(ImVec2(capture_pos.x - tiny_button_size, capture_pos.y - tiny_button_size), ImVec2(capture_pos.x + tiny_button_size, capture_pos.y + tiny_button_size), buttons["capture-button"].pressed && highlight_pressed ? IM_COL32(51, 153, 255, 255) : IM_COL32(102, 101, 101, 255), 2.0f * controller_scale);
        HandleButtonInteraction("capture-button", ImVec2(capture_pos.x - tiny_button_size, capture_pos.y - tiny_button_size), ImVec2(tiny_button_size*2, tiny_button_size*2), buttons["capture-button"]);
        // --- END MISC BUTTONS INTERACTION ---
        
        // Reserve space for the controller
        ImGui::Dummy(canvas_size);
    }
    
} // namespace Pound::GUI