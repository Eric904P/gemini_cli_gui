# minimum cmake version required
cmake_minimum_required(VERSION 3.16)

# define the project name and language
project(GeminiNativeAgent VERSION 1.0 LANGUAGES CXX)

# set project output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# enforce c++17 standard for modern architecture
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# enable qt's automated build tools
set(CMAKE_AUTOMOC ON) # processes Q_OBJECT macros
set(CMAKE_AUTORCC ON) # processes resource files (.qrc)
set(CMAKE_AUTOUIC ON) # processes designer files (.ui)

# locate the qt6 libraries on the local system
# note: if cmake fails to find qt, you will need to set CMAKE_PREFIX_PATH
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Sql)

# define the executable and its source files
add_executable(GeminiNativeAgent WIN32
    main.cpp
    main_window.cpp
    gemini_api_client.cpp
    agent_manager.cpp
    settings_dialog.cpp
    session_dialog.cpp
    database_manager.cpp
    tool_schema_provider.cpp
    gemini_payload_builder.cpp
    write_file_action.cpp
    ftp_upload_action.cpp
    execute_shell_action.cpp
    take_screenshot_action.cpp
    agent_prompt_builder.cpp
    chat_formatter.cpp
    git_manager_action.cpp

    base_agent_action.h
    theme_manager.h
)

# --- AUTOMATIC QT DLL DEPLOYMENT ---

# 1. Find the path to Qt's built-in deployment tool
# We grab the directory of the qmake executable so CMake knows exactly where to look
if(TARGET Qt6::qmake)
    get_target_property(QMAKE_EXE Qt6::qmake IMPORTED_LOCATION)
elseif(TARGET Qt5::qmake)
    get_target_property(QMAKE_EXE Qt5::qmake IMPORTED_LOCATION)
endif()

get_filename_component(QT_BIN_DIR "${QMAKE_EXE}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_BIN_DIR}")

# 2. Tell CMake to run windeployqt on the compiled .exe immediately after a successful build
if(WINDEPLOYQT_EXECUTABLE)
    add_custom_command(TARGET GeminiNativeAgent POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}"
        # We add flags to prevent it from copying unnecessary bloat (like foreign language translations)
        ARGS --no-translations --no-opengl-sw "$<TARGET_FILE:GeminiNativeAgent>"
        COMMENT "Running windeployqt to automatically copy required Qt DLLs..."
    )
else()
    message(WARNING "windeployqt tool not found! Qt DLLs will not be copied automatically.")
endif()

# link the required qt modules to the executable
target_link_libraries(GeminiNativeAgent PRIVATE 
    Qt6::Core 
    Qt6::Gui 
    Qt6::Widgets
    Qt6::Network
    Qt6::Sql
)