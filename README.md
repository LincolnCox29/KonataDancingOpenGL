# KonataDancingOpenGL

**KonataDancingOpenGL** is an implementation of Konata's dance from the anime **Lucky Star**. The program is written in C using OpenGL and shows a sequence of images (frames) in a window with a transparent background, allowing interaction by dragging the window when clicking on opaque pixels.

![image](https://github.com/LincolnCox29/KonataDancingOpenGL/blob/master/frames/frame_0135.png)

## Features

- Displays a sequence of textures loaded from PNG files.
- Support for a transparent window.
- Ability to drag the window when clicking on opaque pixels.

## Dependencies

The following libraries are already included in the source files:

- [GLEW](http://glew.sourceforge.net/): A library for managing OpenGL extensions.
- [GLFW](https://www.glfw.org/): A library for creating windows and handling input in OpenGL.
- [STB Image](https://github.com/nothings/stb): A single-file library for loading images.

## Requirements

Make sure you have the following components installed:

- C compiler (e.g., GCC, MSVC)
- CMake (for building the project)

## Build Instructions

1. Clone the repository:

   ```bash
   git clone <repository-url>
   cd KonataDancingOpenGL
   ```

2. Configure the project with CMake:

   ```bash
   mkdir build
   cd build
   cmake ..
   ```

3. Build the project:

   ```bash
   make
   ```

   For Windows users with Visual Studio, open the generated `.sln` file and build the project.
    

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.

## Acknowledgements
- Thanks to the creators of GLEW, GLFW, and STB Image for their excellent libraries.
