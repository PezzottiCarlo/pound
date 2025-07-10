# Copyright 2025 Pound Emulator Project. All rights reserved.

# GUI sources
set(GUI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GUIManager.cpp
)

set(GUI_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Window.h
    ${CMAKE_CURRENT_SOURCE_DIR}/GUIManager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Panel.h
)

# Panel sources
set(PANEL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/panels/ConsolePanel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/panels/CPUPanel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/panels/PerformancePanel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/panels/ControllerPanel.cpp
)

set(PANEL_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/panels/ConsolePanel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/panels/CPUPanel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/panels/PerformancePanel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/panels/ControllerPanel.h
)

# Add all GUI sources to the main target
target_sources(Pound PRIVATE
    ${GUI_SOURCES}
    ${GUI_HEADERS}
    ${PANEL_SOURCES}
    ${PANEL_HEADERS}
)

# Include directories for GUI
target_include_directories(Pound PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)