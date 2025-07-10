// Copyright 2025 Pound Emulator Project. All rights reserved.
#pragma once

#include "../Panel.h"
#include <vector>
#include <string>
#include <deque>

namespace Pound::GUI
{

    class ConsolePanel : public Panel
    {
    public:
        ConsolePanel();

        void Render() override;
        void AddLog(const std::string &text);
        void Clear();

    private:
        struct LogEntry
        {
            std::string text;
            ImVec4 color;
        };

        std::deque<LogEntry> log_buffer;
        bool auto_scroll = true;
        bool show_timestamps = true;
        static constexpr size_t MAX_LOG_ENTRIES = 1000;

        ImVec4 GetLogColor(const std::string &text) const;
    };
    
} // namespace Pound::GUI