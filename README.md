# Gemini Native Agent (C++ / Qt6)

![C++](https://img.shields.io/badge/C++-17-blue.svg) ![Qt6](https://img.shields.io/badge/Qt-6.x-41CD52.svg) ![CMake](https://img.shields.io/badge/CMake-3.16+-red.svg) ![License](https://img.shields.io/badge/License-MIT-green.svg)

**Gemini Native Agent** is an air-gapped, stateful, multi-modal End User Computing (EUC) tool and autonomous coding assistant. Built entirely in native C++ using the Qt6 framework, it bypasses traditional Node.js/Python wrappers to communicate directly with Google's Gemini REST API. 

Designed with a strict **Zero-Trust Architecture**, the agent features native function calling, local OS execution, computer vision, and secure SQLite session persistence.

## 🚀 Enterprise-Grade Features

### Core Architecture
* **Native REST Pipeline:** Utilizes `QNetworkAccessManager` for lightning-fast, asynchronous HTTP requests directly to the `gemini-2.5-flash` endpoints, eliminating heavy external runtime dependencies.
* **Stateful Interactions:** Implements Google's `previous_interaction_id` architecture, offloading context management to the server. This results in ultra-low latency and near-zero input token costs for prolonged debugging sessions.
* **Secure Credential Vault:** API keys and environment variables (like `GITHUB_PAT`) are encrypted and stored via `QSettings` in the native OS registry, preventing hardcoded vulnerabilities.
* **SQLite Session Hot-Swapping:** Uses `QSqlDatabase` to persist multi-turn conversations and agent states, allowing instantaneous switching between isolated development environments.

### Autonomous "Eyes and Hands" (Native Function Calling)
The agent utilizes strict JSON schemas to natively understand and execute C++ functions on the local machine:
* **Silent Reconnaissance:** The agent can autonomously use `read_file`, `list_directory`, and `fetch_webpage` to understand the workspace DOM/codebase without interrupting the user.
* **Computer Vision (Windows API):** Integrates with `EnumWindowsProc` to literally "see" the desktop. The agent can compile GUI code, launch the application, target the specific window HWND, take a screenshot (`take_screenshot`), and self-correct its UI layout based on the pixels.
* **Sub-Process Execution:** Spawns asynchronous, memory-safe `QProcess` instances to compile C++, run Python scripts, or execute complex Git workflows completely in the background.

### 🛡️ Zero-Trust Security Model
Security is paramount in EUC environments. The agent operates in a highly restricted sandbox:
* **Human-in-the-Loop Intercepts:** While reads are silent, *any* destructive or modifying action (`write_file`, shell execution, network pushes) triggers a mandatory `QMessageBox` thread-blocking modal. The action is halted until the user physically verifies the target and payload.
* **Path Traversal Protection:** The `currentWorkspacePath` is strictly locked. The agent cannot traverse up the directory tree or access restricted OS files.
* **Binary Token Filter:** A custom multi-modal drag-and-drop filter automatically blocks `.exe` or compiled binaries from being uploaded to the context window, protecting the API token budget and preventing payload corruption.

---

## 🛠️ Build & Installation Instructions

This application is built to be a standalone, portable executable.

### Prerequisites
* **C++ Compiler:** MSVC (Windows), GCC (Linux), or Clang (macOS) supporting C++17.
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
3. Compile the executable:
   ```bash
   cmake --build . --config Release
   ```

---

## 💻 Usage & Configuration

1. **Launch the Application:** Run the compiled `GeminiCliGui` executable.
2. **Secure Key Entry:** On first launch, the application will prompt you for your Google Gemini API Key. This is safely encrypted into your local system registry.
3. **Session Management:** Create a new SQLite session or load an existing one from the startup modal.
4. **Agentic Commands:** You can speak to the agent naturally. If you ask it to "create a python script that prints hello world", the agent will natively route a function call to the C++ backend, trigger the security intercept, and write the file directly to your disk upon approval.

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

---
*Developed by Lewis Curl as a demonstration of native C++ architecture, memory management, and modern End User Computing standards.*