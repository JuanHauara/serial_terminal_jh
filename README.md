# Serial Terminal

Serial Terminal is a simple wxWidgets desktop application for bidirectional
communication with microcontrollers and other devices through serial ports.

The program is intentionally small: the main documentation focus is the build
environment, the libraries used, and the VS Code build tasks.

## Toolchain and Libraries

The current Windows build setup uses:

- C++11, configured in `CMakeLists.txt` with `CMAKE_CXX_STANDARD 11`.
- CMake 3.10 or newer. The current local setup uses CMake 4.3.2.
- TDM-GCC-64 / MinGW, installed at `C:\TDM-GCC-64`.
- GNU C++ compiler `C:\TDM-GCC-64\bin\g++.exe`, version 10.3.0.
- wxWidgets 3.2.10, installed at `C:\wxWidgets-3.2.10`.

The CMake file currently looks for wxWidgets with:

```cmake
set(wxWidgets_ROOT_DIR C:/wxWidgets-3.2.10)
find_package(wxWidgets REQUIRED COMPONENTS core base gl net OPTIONAL_COMPONENTS)
```

The generated wxWidgets library directory in the current local build is:

```text
C:/wxWidgets-3.2.10/lib/gcc_lib
```

Use the same MinGW toolchain for the application and for wxWidgets. Mixing
different MinGW distributions or incompatible compiler versions can cause link
errors.

## Build System

The project is configured with CMake and uses the `MinGW Makefiles` generator on
Windows. The CMake target is:

```text
serial_terminal
```

The Windows executable is generated as:

```text
build/serial_terminal.exe
```

`CMakeLists.txt` also sets `-mwindows` so the application starts without opening
a separate Windows console.

### Build from VS Code tasks on Windows

The repository includes VS Code tasks in:

```text
.vscode/tasks.json
```

The documented build flow assumes Windows with TDM-GCC-64 / MinGW installed at:

```text
C:\TDM-GCC-64
```

To build the project from VS Code on Windows:

1. Open the project folder in VS Code:

```text
C:\prog\cpp\serial_terminal
```

2. Run the default build task with **Terminal > Run Build Task...** or
   `Ctrl+Shift+B`.
3. Select **Configure & Build** if VS Code asks for a task.

The **Configure & Build** task does the following on Windows:

- prepends `C:\TDM-GCC-64\bin` to `PATH`;
- creates the `build` folder if it does not exist;
- runs CMake with the `MinGW Makefiles` generator;
- sets the C compiler to `C:/TDM-GCC-64/bin/gcc.exe`;
- sets the C++ compiler to `C:/TDM-GCC-64/bin/g++.exe`;
- runs `cmake --build .` only if configuration succeeds.

The generated executable is:

```text
build/serial_terminal.exe
```

To remove the generated build directory from VS Code, run **Terminal > Run
Task...** and select **Clean**. This deletes:

```text
build
```

### Manual Windows build

Equivalent Windows build with TDM-GCC-64:

```powershell
$env:Path = 'C:\TDM-GCC-64\bin;' + $env:Path
if (!(Test-Path -Path 'build')) { New-Item -Path 'build' -ItemType Directory | Out-Null }
Set-Location -Path 'build'
cmake -G 'MinGW Makefiles' -DCMAKE_C_COMPILER='C:/TDM-GCC-64/bin/gcc.exe' -DCMAKE_CXX_COMPILER='C:/TDM-GCC-64/bin/g++.exe' ..
cmake --build .
```

The executable is generated as:

```text
build/serial_terminal.exe
```
