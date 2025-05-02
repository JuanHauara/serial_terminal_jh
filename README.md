Serial terminal software designed to establish bidirectional communication between a PC and microcontrollers (or other devices) through serial ports. Useful for development and testing.

## Building (Windows with MinGW)

To compile the project on Windows using MinGW (like TDM-GCC):

1.  **Open a terminal** (like Git Bash, Command Prompt, or PowerShell).

2.  **Create a `build` directory** inside the project's root folder and navigate into it:
    ```bash
    mkdir build
    cd build
    ```

3.  **Configure the project using CMake:**
    Run CMake from within the `build` directory, telling it where the source (`CMakeLists.txt`) is (in the parent directory `..`) and specifying the MinGW generator:
    ```bash
    cmake -G "MinGW Makefiles" ..
    ```
    * **Note:** On Windows, the `-G "MinGW Makefiles"` generator must be specified to instruct CMake to use the MinGW GCC compiler. Otherwise, CMake might default to the Visual C++ compiler (MSVC) if detected.

4.  **Compile the project:**
    Execute the make command provided by your MinGW installation.
    ```bash
    C:/TDM-GCC-64/bin/mingw32-make.exe
    ```
    * **Important:** This command assumes you have TDM-GCC 64-bit installed in `C:/TDM-GCC-64`. **Adjust this path** if your MinGW installation (e.g., TDM-GCC, MSYS2 MinGW) is located elsewhere.
    * Alternatively, if you have added the `bin` directory of your MinGW installation to your system's PATH environment variable, you might be able to simply run:
        ```bash
        mingw32-make
        ```

## Single Command Build

You can also configure and build using a single command line, which can be useful for setting up build commands in editors like Geany:

```bash
cmake -G "MinGW Makefiles" .. && C:/TDM-GCC-64/bin/mingw32-make.exe