

# Elevate

**Elevate** is a web-based launcher that combines an interactive UI (built with HTML and CSS) with native Windows application launching capabilities via C++. The project bridges the web and desktop worlds, enabling users to launch Windows applications (like Calculator) directly from a modern web interface.

## Features

- **Interactive Web UI:**  
  Built with HTML and CSS for a responsive, user-friendly experience.

- **Native Windows App Launcher:**  
  Uses C++ functions to launch Windows applications (e.g., Calculator, Notepad) from the web interface.

- **Web-OS Interaction:**  
  Seamlessly connects web technologies with OS-level operations for enhanced productivity.

## Getting Started

### Prerequisites

- Windows OS
- [CMake](https://cmake.org/) (for building C/C++ code)

### Installation

1. **Clone the repository:**

    ```bash
    git clone https://github.com/Fyonietz/Elevate.git
    cd Elevate
    ```

2. **Build the Native Launcher:**

    ```bash
    run ".\build.cmd"
    check ports (default is 9090)
    ```

3. **Run the Web UI:**

    - Open `(http://localhost:9090/)` in your browser  

4. **Launching Apps:**

    - Use the web interface to select and launch available Windows applications.

## How It Works

- The front-end (HTML/CSS/JS) provides an interactive launcher UI.
- When a user selects an app to launch, the request is sent to the native C++ backend.
- The backend executes the appropriate Windows system command to launch the desired application.

## Example

Clicking on the "Calculator" icon in the web launcher UI will start the Windows Calculator app via a C++ function.

## Route Lists

**Info**
- `(sys_info/time)` For Getting Time

**Action**
- `(sys_act/save)` For Save JSON File In Taskbar
- `(sys_act/add)` For Adding App To JSON(Taskbar)
- `(sys_act/run)` For Running App In Elevate(Universal)
- `(sys_act/delete)` For Deleting App In JSON Lists

## Contributing

Pull requests are welcome! Please open an issue first to discuss your ideas.

## License

[MIT](LICENSE)

---

Feel free to modify or expand this README to better fit your project's specific details! If you want the README.md as a file, let me know.
