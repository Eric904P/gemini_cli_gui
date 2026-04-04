# Gemini Native Agent (C++ / Qt6)

![C++](https://img.shields.io/badge/C++-17-blue.svg) ![Qt6](https://img.shields.io/badge/Qt-6.x-41CD52.svg) ![CMake](https://img.shields.io/badge/CMake-3.16+-red.svg) ![License](https://img.shields.io/badge/License-MIT-green.svg)

**Gemini Native Agent** is an air-gapped, stateful, multi-modal autonomous coding assistant. Built entirely in native C++ using the Qt6 framework, it bypasses traditional Node.js or Python wrappers to communicate directly with Google's Gemini 2.5 Flash API. 

The agent features native function calling, local OS execution, computer vision, and secure SQLite session persistence.

## ✨ What's New in v1.1.0: The Autonomous Loop
v1.1.0 is a major milestone: **The agent can now autonomously write, compile, execute, and visually verify its own C++ GUI code from a single command.** * **The Autonomous Coding Loop:** You can now issue a single prompt (e.g., "Write a C++ GUI that says 'Hello World' in red, compile it, run it, and confirm the text is red"). The agent will write the file, use the shell to compile it via `g++`, launch the resulting `.exe`, and use the screenshot tool to visually verify the output.
* **Recursive Process Tree Vision:** The `take_screenshot` tool now uses WinAPI's `Toolhelp32Snapshot` to map child-process trees (e.g., `cmd.exe` -> `hello_world.exe`), guaranteeing perfectly cropped bounding boxes of the generated GUI windows instead of grabbing the whole desktop.
* **Execution Barrier (Anti-Stutter):** Rebuilt the internal data pipeline to perfectly interleave LLM Tool Call states with API memory. A C++ execution barrier intercepts and discards duplicate tool calls, forcing sequential, verified reasoning and preventing infinite API loops.
* **Modular Architecture:** The codebase has been completely refactored into domain-specific subdirectories (`/agent`, `/tools`, `/data`, `/settings`) for easier scaling and maintenance.

---

## 🛠️ Toolset & Capabilities

The agent utilizes strict JSON schemas to natively understand and execute functions on the local machine.

### Core Tools
* **File System Operations:** `write_file`, `read_file`, and `list_directory` allow the agent to understand and manipulate the workspace codebase.
* **Sub-Process Execution (`execute_shell_command`):** Spawns asynchronous, memory-safe `QProcess` instances to compile code, run scripts, or manage local servers completely in the background.
* **Computer Vision (`take_screenshot`):** The agent can capture a cropped screenshot of the active GUI window to visually verify if the code it just compiled is rendering correctly.
* **Network & Deployment:** `upload_ftp` for remote deployments, `fetch_webpage` to read external documentation or verify live DOMs, and `git_manager` for batched version control workflows.

### Zero-Setup Runtimes
The agent dynamically injects portable toolchains (like MinGW, Python, Node) directly into its isolated execution environment. This allows it to compile C++ and run scripts out-of-the-box without relying on the host OS `PATH` variables.

### Stateful Memory
Implements Google's stateful interactions endpoint, offloading context management to the server. Chat history is cached server-side and managed locally via SQLite (`QSqlDatabase`), allowing instantaneous hot-swapping between isolated project sessions with near-zero input token costs for prolonged debugging.

---

## 🛡️ Security & Sandboxing

The agent operates in a highly restricted environment to ensure host system safety:
* **Human-in-the-Loop Intercepts:** While reads are silent, *any* destructive or modifying action (`write_file`, shell execution, network pushes) triggers a mandatory UI intercept. The action is halted until the user physically verifies the target and payload.
* **Path Traversal Protection:** The workspace path is strictly locked. The agent cannot traverse up the directory tree or access restricted OS files.
* **Secure Credential Vault:** API keys and environment variables (like `GITHUB_PAT`) are encrypted and stored via `QSettings` in the native OS registry.

---

## 💻 Usage & Configuration

1. **Launch the Application:** Run the compiled executable (or install via the standalone release).
2. **Secure Key Entry:** On first launch, the application will prompt you for your Google Gemini API Key.
3. **Session Management:** Create a new SQLite session (which creates an isolated sandbox directory) or load an existing one from the startup modal.
4. **Agentic Commands:** Speak to the agent naturally. If you ask it to "create a python GUI that prints hello world, run it, and verify it", the agent will natively route function calls to the C++ backend, trigger the security intercepts, write the file, execute it, and visually verify the result.

---

## 📦 Build Instructions

If you are building from source rather than using the packaged installer:

### Prerequisites
* **C++ Compiler:** MSVC (Windows) or MinGW supporting C++17.
* **CMake:** Version 3.16 or higher.
* **Qt6:** Required modules: `Core`, `Gui`, `Widgets`, `Network`, `Sql`.

### Compilation
1. Clone the repository:
   ```bash
   git clone [https://github.com/yourusername/gemini_cli_gui.git](https://github.com/yourusername/gemini_cli_gui.git)
   cd gemini_cli_gui
   ```
2. Generate the build files using CMake:
   ```bash
   mkdir build && cd build
   cmake ..
   ```
3. Compile the executable in Release mode:
   ```bash
   cmake --build . --config Release
   ```
   *(Note: CMake is configured to automatically run `windeployqt` post-build to package necessary DLLs).*

---

## 🧠 System Architecture

```text
[User UI] <---> [MainWindow Router] <---> [SQLite Database]
                        |
                        v
        [AgentActionManager (Security Gatekeeper)]
                        |
                        v
    [GeminiApiClient (Asynchronous QNetworkAccessManager)]
                        |
                        v
          [Google Gemini REST API (gemini-2.5-flash)]
```

---

## 📝 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.