# LearnOpenGL notes
* glEnable(<target>) - enables an OpenGL feature

* Called an API, yet a specificition.
  - Specifies the results of each function and how it should perform.
  - Usually implemented by GPU manufacturers, Apple, or Linux hackers.

## Core profile vs Immediate profile
* Immediate mode (fixed function pipeline)
  - Old
  - No flexibility
  - Legacy OpenGL

* Core profile
  - Modern OpenGL
  - Flexible and efficient
  - After 3.3, no major changes, just efficiency-tuning
    * Thats why a lot of developers target 3.3, wider range of GPUs support it

## Extensions
- Specific implementations of certain techniques that allow for more efficiency.
- Have to query the GPU if a certain one is available.
- Not part of the standard.

## State machine
- A collection of variables that define how OpenGL should currently operate.
* State is known as the `context`.
- Rendering is done using the current context.

## Objects
- OpenGL was natively implemented in C.
- But C is old and doesnt have much abstractions.
- Think of them as C-like structs:
```c
struct WindowTargetObject{
  float option1;
  float option2;
  char[] name;
};
```
- Then instantiations of specific objects can bind to specific variables in the context:
```c
struct OpenGLContext{
  ...
  WindowTargetObject* pWindowTarget;
  ...
};
```
- Through a code as this:
```c
unsigned int id = 0;
glGenObjects(1, &id);
glBindObjects(GL_WINDOW_TARGET, id); //Bind into the window target context slot
glSetObjectOption(GL_WINDOW_TARGET, 800);
glSetObjectOption(GL_WINDOW_TARGET, 600);
glBindObject(GL_WINDOW_TARGET, 0); //Unbind
```
- This may seem unnecessary to keep multiple window parameter objects and switch between them.
  * But consider if its 2 large models (of a house and another one of a character), that we prepared before, and we just need to bind them to draw them now.

## GLFW (Creating a window)
- OpenGL prefers to be abstracted from the OS.
- Make a window, create a context (OS attaches the context to the program's memory space), handle inputs (including resizing), through code

### Set version
```c
//Settings
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

//Disables backward compatibility with Legacy OpenGL
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Core profile

//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //For Mac
```

## glViewport
```c
//Set viewport
glViewport(0, 0, 1280, 720);
//How to display coordinates with respect to the window
//left corner, right corner
//Maps normalized device coordinates into window coordinates
//Allows to make a literal viewport within the window and reserve other space for more

//However, viewport has to be adjusted for whenever the window is resized
glfwSetFramebufferSizeCallback(pWindow, [](GLFWwindow* pWindow, int width, int height){
    glViewport(0, 0, width, height);
});
```

## Double buffer
```c
//Swap front and back buffers
glfwSwapBuffers(pWindow);
//Double buffer
//Consider this case that shows the advantage of the double buffer:
//  - A monitor updates at a fixed rate
//  - If the GPU is still writing to the next frame, half of it would be new and the other half would be old
//  - So just display the previous frame, and then once finished swap buffers
//The render loop and monitors refresh rate are desynchronized after all
```

## Input
```c
//If the key is not pressed, GLFW returns GLFW_RELEASE
auto processInput = [](GLFWwindow* pWindow){
  if (glfwGetKey(pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS){
    glfwSetWindowShouldClose(pWindow, true);
  }
};
```

## glClear
* Clears the current buffer drawn to.
* Options are: `GL_COLOR_BUFFER_BIT`, `GL_DEPTH_BUFFER_BIT`, and `GL_STENCIL_BUFFER_BIT`.
* `glClearColor(r, g, b, a)` specifies clear color...
* Call each frame (just wanted to use the new terminology)
* glClear is a state-using function, glClearColor is a state-setting functin.

## Rendering pipeline and Shaders
* Large portion of what OpenGL does is projecting 3D graphics onto 2D screen.
  - Managed by graphics pipeline
### Graphics pipeline
* Can be split into 2 major parts: 3D coordinates into 2D coordinates & 2D coordinates into pixels
* Series of steps that depend upon each other but that are highly "parallizable".
* Shaders are written in GLSL.

#### Stages
  - Take in vertex data
  - Vertex - collection of data per 3D coordinate (vertex attributes for each data type)
  - OpenGL requires knowledge of what shape is drawn: GL_POINTS, GL_TRIANGLES, GL_LINE_STRIP
    - Those hints are called primitives
* Vertex Shader         (Customizable)
  - Single vertex for input.
  - Transform 3D coordinates into different 3D coordinates
  - Basic processing on the vertex attributes
* Geometry Shader       (Customizable)
  - Collection of vertices that form a primitive as input.
  - Optional
  - Can make new vertices (to form other primitives)
  - Ex: Mirrors a triangle to make a quad
* Shape (Primitive) Assembly
  - All vertices of a single primitive as input.
  - And assembles the the shape given.
* Rasterization
  - Map primitives to corresponding pixels on the screen.
  - Results fragments (all data for single pixel) for the fragment shader to use.
  - Clipping is done right after, discard all pixels outside the viewport.
* Fragment Shader       (Customizable)
  - Calculate the final color of a pixel
  - Usually where all the advanced effects are (with data on the 3D scene)
* Tests and Blending
  - Alpha test and blending
  - Check depth and stencil values, and discard pixels behind other objects.
  - Blend objects accordingly with their alpha values.

* Also Tessellation and Transform Feedback Loop stages (later).
* Mostly a vertex and a fragment shaders are used (required by OpenGL).

## Vertex input
* To start drawing - give OpenGL input vertex data.
* OpenGL uses NDC (Normalized Device Coordinates), all 3 axes range between -1 and 1 for positioning
  - NDC has to be used after vertex shader (otherwise clipping)
  - Origin is at the center and positive-y is up, not like usual screen coordinates
* NDC coordinates are then properly transformed, via glViewport's specification, into screen-space coordinates.
  - Called viewport transform.

* Sent to vertex shader via a Vertex Buffer Object (VBO):
  - GPU<->CPU communication is really slow, thats why its good to send as much data in a batch as possible
  - OpenGL object, specifically GL_ARRAY_BUFFER
  - Can have multiple different buffers bound at once
```c
//Vertex buffer
unsigned int vBuffer;
glGenBuffers(1, &vBuffer); //vBuffer is the id of the vertex buffer within the context
//OpenGL is heavy on ids

//Bind vertex buffer
glBindBuffer(GL_ARRAY_BUFFER, vBuffer); 
//target (its type, what variable in the context to assign this buffer to), id
//To unbind, bind 0

std::vector<float> vbo = {
  -0.5f, -0.5f, 0.0f,
   0.5f, -0.0f, 0.0f,
   0.0f, 0.5f,  0.0f
};

//Have to specify size desired, but filling can be done any time later
glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vbo.size(), vbo.data(), GL_STATIC_DRAW);
//Buffer data into the GL_ARRAY_BUFFER "variable" in the context (bound previously, state machine)
//Type, size of buffer (bytes), pointer to data (NULL for none), usage (Docs)
//Usage: static - make once, read a lot
//       dynamic - modify and read a lot
//       stream - just modify and read a couple of times
//       draw - uses as source for drawing
//       read - used to read data from GPU
//       copy - read from GPU and used to draw
//For optimization, tell OpenGL into what memory to place (fast or slow)

//Now have to tell how to interpret the data in the buffer to access in a shader later
```

## Vertex shader
```c
#version 330 core
layout (location = 0) in vec3 iPos; //Vertex array layout 0th index

void main(){
  gl_Position = vec4(iPos.xyz, 1.0);
  //Vectors are handy in representing positions
}
```

## Fragment shader
```c
#version 330 core
out vec4 oColor;

void main(){
  oColor = vec4(1.0f, 0.5f, 0.2f, 1.0f); //RGBA (0-1)
}
```

## Using shaders within C++
```c
unsigned int Shader(unsigned int type, std::string path){
  unsigned int shaderObj = glCreateShader(type);
  std::string src = ReadRel(path);
  const char* cSrc = src.c_str();

  //Attach the source code
  glShaderSource(shaderObj, 1, &cSrc, NULL);
  //ID, # of strings as source code, source, length of each string passed (in case not C-style termination, "null terminator")

  //Compile the shader
  glCompileShader(shaderObj);

  //Check successful compilation
  int success;
  glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success); //success has to be a vector of ints (for function), here, though, 1 int is suffient
  if (!success){
    char infoLog[512];
    glGetShaderInfoLog(shaderObj, 512, NULL, infoLog); //3rd is length of string of infoLog
    LOG_ERROR("Shader failed to compile: ", infoLog);
  }

  return shaderObj;
}

unsigned int ShaderProgram(unsigned int vso, unsigned int fso){
  //Final linked version of multiple shaders combined
  //Links outputs of each shader to the next

  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();

  glAttachShader(shaderProgram, vso);
  glAttachShader(shaderProgram, fso);
  glLinkProgram(shaderProgram);

  //Check failure
  int success;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success){
    char infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    LOG_ERROR("Shader linking error: ", infoLog);
  }

  //Detach shader so that the linker doesnt use them anymore
  glDetachShader(shaderProgram, vso);
  glDetachShader(shaderProgram, fso);

  //Delete the shader objects
  //Rather marks them for deletion after the shader program is deleted
  glDeleteShader(vso);
  glDeleteShader(fso);
}

...

//Compile a shader (Dynamically from source code)
std::string vShaderPath = std::string(SHADERS_DIR) + std::string("v1.glsl");
unsigned int vso = Shader(GL_VERTEX_SHADER, vShaderPath);
std::string fShaderPath = std::string(SHADERS_DIR) + std::string("f1.glsl");
unsigned int fso = Shader(GL_FRAGMENT_SHADER, fShaderPath);
unsigned int shaderProgram = ShaderProgram(vso, fso);
glUseProgram(shaderProgram); //Binding

...

//Delete shaderProgram from memory
glDeleteProgram(shaderProgram);
```

## Vertex Attributes
```c
//Have to specify size desired, but filling can be done any time later
glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vbo.size(), vbo.data(), GL_STATIC_DRAW);
//Buffer data into the GL_ARRAY_BUFFER "variable" in the context (bound previously, state machine)
//Type, size of buffer (bytes), pointer to data (NULL for none), usage (Docs)
//Usage: static - make once, read a lot
//       dynamic - modify and read a lot
//       stream - just modify and read a couple of times
//       draw - uses as source for drawing
//       read - used to read data from GPU
//       copy - read from GPU and used to draw
//For optimization

//Now have to tell how to interpret the data in the buffer to access in a shader later
//Vertex attributes binding to indices for use in vertex shader
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
//Per vertex attribute
//1st: which attrib (index) (location in vertex shader)
//2nd: # of values (3 coordinates)
//3rd: data type of each value (vec in GLSL is floats)
//4th: normalize (if int->float 0 (or -1 for signed) to 1)
//5th: stride - space between vertices (can be more if more elements in each vertex)
//     can be left to 0, if each vertex is tightly packed, OpenGL will figure it out
//6th: offset within the buffer where the data begins of this attribute
glEnableVertexAttribArray(0); //Enable at index 0 (attribute)

//However, since we dont want to repeat this glVertexAttribPointer stuff every time we bind a different VBO,
```

## Vertex Array Object (VAO)
* Since we dont want to repeat this glVertexAttribPointer stuff every time
we bind a different VBO, we use VAOs
* Associates vertex attrib calls and VBOs and stores them (+Index buffer).
  - Dont have to bind VBO if bind VAO!
* OpenGL core requires 1.
* When have multiple objects, 1 VAO per each.
```c
unsigned int vao;
glGenVertexArrays(1, &vao); //If wanted to gen more than one, would use array
glBindVertexArray(vao);
```

## Render loop
```c
//Render loop, we dont want the program to run one frame and terminate
//One iteration is called a frame!
while (!glfwWindowShouldClose(pWindow)){
  //Process input
  processInput(pWindow);

  glBindVertexArray(vao);
  glUseProgram(shaderProgram);

  //Clear
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  //Without index buffer
  //glDrawArrays(GL_TRIANGLES, 0, 3);
  //Mode, starting index (of vertex), # of indices

  //With index buffer
  //glDrawElements(GL_TRIANGLES, rawIbo.size(), GL_UNSIGNED_INT, NULL);
  //Mode, count, type of index buffer, offset (or if none bound, ptr to array of indices)

  //Swap front and back buffers
  glfwSwapBuffers(pWindow);
  //Double buffer
  //Consider this case that shows the advantage of the double buffer:
  //  - A monitor updates at a fixed rate
  //  - If the GPU is still writing to the next frame, half of it would be new and the other half would be old
  //  - So just display the previous frame, and then once finished swap buffers
  //The render loop and monitors refresh rate are desynchronized after all

  //Poll events, call callbacks
  glfwPollEvents();
}
```

## Index Buffer/Element Buffer (IBO/EBO)
* Allows to specify indices of vertices to render.
* No overhead from specifying the same indices.
  - What if a vertex has a billion attributes? (Massive overhead)
* Last to bind is stored as VAO's IBO
  - Note that unbinding also counts (so dont unbind before unbinding VAO)
```c
//Data
std::vector<float> rawVbo = {
  -0.5f, -0.5f,  0.0f,
   0.5f, -0.5f,  0.0f,
  -0.5f,  0.5f,  0.0f,
   0.5f,  0.5f,  0.0f
};
//It would be great if instead of buffering overlapped vertices, we could only store
//unique ones and specify which indices to use
//Called indexed drawing
std::vector<unsigned int> rawIbo = {
  0, 1, 2, //First triangle
  1, 2, 3
};

//Index buffer
unsigned int ibo;
glGenBuffers(1, &ibo);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*rawIbo.size(), rawIbo.data(), GL_STATIC_DRAW);
```

## Wireframe mode
* Can specify how primitives are drawn (for example: triangles as lines)
```c
glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Into line
//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //Back
//More about GL_FRONT and GL_BACK in face culling (later)
```
