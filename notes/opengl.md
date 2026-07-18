# OpenGL

## What is OpenGL?
* OpenGL - Graphics API
* It allows to access GPU

* Its not a library.
* Its just a specification.

* GPU manufacturer implements the code that is then accessed by OpenGL's API.
* Its quite clearly not open-source, but theres mesa.

* Its cross-platform.
* Its not superior because of that, game engines dont implement a single graphics API.
* Usually specific APIs, such as Metal or Direct3D, are superior to general ones like OpenGL.

* Really simple and easy.
* Great for learning.

## Modern vs Legacy OpenGL 
* Modern - manual shaders
* Legacy - no shader control (can be used for debugging)

* Shader - program that runs on GPU

## GLFW
* Handles window-making and inputs (OS-specific)
* The initial window position was handled by GLFW. Though, loading the binaries is done by OS, GLFW makes sure it will happen when the context is made current.
* Creates OpenGL context, which is an instance of an OpenGL class within drivers' binaries, it has all necessary information for rendering.
* Then the drivers and/or opengl32.lib kick in and handle the rendering after querying (linking) their function implementations via glew to the program.

## 64-bit vs 32-bit
* Only the destination
* It doesnt matter that much
* Just use 32-bit because 64-bit is backwards compatible with it

## Front and Back buffers
* There are 2 buffers, one is on the screen and one is back.
* Back buffer is drawn to, the front is displayed.

## OpenGL32.lib
* A static library full of references to opengl32.dll.
* Its GLEW but for OpenGL 1.1 and below.
* `gl.h` is its header.

## Window's headers (^)
* Windows has headers for querying drivers' implementation up to 1.1
* Then, they started to make Direct3D

## GLEW
* Through OS-specific dynamic linking API, link the headers with OpenGL functions to the drivers' implementation.
* Drivers' implementation lives on the GPU, once runtime, its binaries are loaded into the application's memory by OS and specified by GLFW.
* The memory addresses are then handled by OS and GLEW.

### Usage
* Need a valid OpenGL context.
* And call `glewInit();`.
* Note that `GL/glew.h` has to be included before `GLFW/glfw3.h`.
This is because `gl.h` is included automatically in GLFW, unless GLEW defines a macro against that.
GLEW would need to do that because it redefines the exact functions within `gl.h`.
* However, gl.h contains necessary functions to communicate with the OS, so opengl32.lib must be linked to (even though the functions are redefined by GLEW).
* And, GLEW also doesnt dynamically link to older 1.1 and below functions, it links to OpenGL32.lib.

## Vertex Buffer
* Just a buffer on GPU (on VRAM), usually stores vertices.
* Have to tell GPU how to interpret it and then can issue the draw call that is customizable via shaders.

## State machine
* Set states.
* Commands are contextual, they depend on previous commands.
