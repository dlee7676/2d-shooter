This is a demonstration level of a vertical-scrolling shooter engine made with DirectX. The concept is based on "danmaku", or "bullet curtain" games such as the Touhou Project series. Notable features are pointed out in annotations.

Aside from the green particles, the assets used (player, enemy, and bullet sprites, and background images) are not mine, and are only used to demonstrate the program.

To compile the program in Visual Studio, add the headers and .cpp files to a blank Win32 project and add the Include folder from the Microsoft DirectX SDK to the Visual C++ include directories.  It is also necessary to add the d3d9.lib and d3dx9.lib files from the Lib/x86 folder in the SDK to the additional dependencies in the linker input options.  To run the program, the included image files need to be added to the project directory as well.  Currently, it must be run in Debug mode.

Controls for the game are:

Arrow keys to move the player

Space bar to fire

Hold Shift to reduce movement speed (allows more precise movement)
