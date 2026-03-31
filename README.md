# Gemini Native Agent (Qt C++)

A lightweight, native desktop application built with C++ and Qt6 that interfaces directly with Google's Gemini `v1beta/interactions` API. 

Unlike standard chat wrappers, this application features an **Agentic Loop** that grants the LLM the ability to execute local file operations on your machine, fully governed by a local GUI security intercept.

## 🚀 Key Features

* **Stateful Interactions API:** Utilizes the new Gemini v1beta endpoint to maintain server-side conversational state, drastically reducing input token payload sizes and bandwidth.
* **Local UI Persistence:** Uses a lightweight SQLite database (`QtSql`) to locally store and render chat history across sessions without needing to re-send context to the LLM.
* **Agentic Tool Calling:** The LLM is equipped with a flattened OpenAPI schema toolset, allowing it to natively request file system operations (e.g., `write_file`).
* **Human-in-the-Loop Security:** All local OS actions requested by the agent are intercepted by a Qt modal dialog. The user must explicitly approve the `git diff` or file changes before the C++ backend executes them.
* **Multi-Modal Ready:** Foundation laid for attaching and sending local files/code snippets directly into the context window.

## 🛠️ Prerequisites

To build and run this project, you will need:
* **C++17** compatible compiler (MSVC, GCC, or Clang)
* **Qt 6.x** (specifically the `Core`, `Gui`, `Widgets`, `Network`, and `Sql` modules)
* **CMake** (version 3.16 or higher)
* **Ninja** (Recommended build system)
* A **Google Gemini API Key** (Accessible via Google AI Studio)

## 🏗️ Build Instructions

This project uses CMake. It is recommended to perform an out-of-source build.

1. **Clone the repository:**
   ```bash
   git clone [https://github.com/yourusername/gemini_cli_gui.git](https://github.com/yourusername/gemini_cli_gui.git)
   cd gemini_cli_gui
   ```

2. **Generate the build files:**
   ```bash
   mkdir build
   cd build
   cmake -G Ninja -DCMAKE_PREFIX_PATH="/path/to/your/Qt/6.x.x/compiler" ..
   ```
   *(Note: If Qt is in your system PATH, you may omit the `CMAKE_PREFIX_PATH` flag).*

3. **Compile the application:**
   ```bash
   ninja
   ```

4. **Run the executable:**
   ```bash
   ./GeminiCliGui
   ```

## ⚙️ Usage & Configuration

1. **First Launch:** Upon launching the application for the first time, you will be prompted to enter your Gemini API Key. This key is securely saved in your OS's native credentials/settings manager via `QSettings`.
2. **Chat Interface:** Use the bottom input field to send prompts. The UI will render your text and the model's responses.
3. **Agentic Actions:** If you ask the assistant to create or modify a file (e.g., *"Create a python script that calculates the Fibonacci sequence"*), the LLM will trigger a function call.
4. **Approval Loop:** A system dialog will appear detailing the requested action, target file, and payload. Click **Yes** to allow the C++ backend to write the file, or **No** to deny the request and feed the denial back to the LLM.

## 🧠 Architecture Overview

* `main_window.cpp`: Handles the core Qt UI logic, intercepts agent requests to trigger security modals, and manages the local SQLite history mapping.
* `gemini_api_client.cpp`: The networking engine. It manages the `QNetworkAccessManager`, constructs the strict JSON/OpenAPI payloads required by the Interactions API, tracks the stateful `interaction_id`, and parses the flattened LLM responses.
* `agent_manager.cpp`: (Controller) Executes the local OS actions once approved by the user.

## 🛡️ Security Disclaimer

This application grants an LLM the capability to write files to your local system. **Never bypass the human-in-the-loop security prompt.** Always review the target file path and the proposed payload before clicking "Yes". 

The application is configured to strictly enforce the `write_file` schema, but users are responsible for the code they allow the agent to execute on their machines.
