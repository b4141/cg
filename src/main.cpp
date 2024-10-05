#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>

//__global_settings
const unsigned int g_SCR_WIDTH = 640;
const unsigned int g_SCR_HEIGHT = 400;
//__VAO
GLuint g_VertexArrayObject = 0;
//__VBO
GLuint g_VertexBufferObject = 0;
//__program_object_for_the_shaders
GLuint g_GraphicsPipelineShaderProgram = 0;
//__shaders_source_code
const std::string g_VertexShaderSource =
    "#version 330 core\n"
    "in vec4 position;\n"
    "void main(){\n"
    "    gl_Position = vec4(position.x, position.y, position.z, position.w);\n"
    "}\n";
const std::string g_FragmentShaderSource =
    "#version 330 core\n"
    "out vec4 color;\n"
    "void main(){\n"
    "    color = vec4(1.0f, 0.5f, 0.0f, 1.0f);\n"
    "}\n";

void HandleLog(bool err, const std::string &message) {
  if (err) {
    std::cout << "---Error: " << message << std::endl;
  } else {
    std::cout << message << std::endl;
  }
}

//__resizing_the_viewport_when_changing_the_indow_size
void FrameBufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

GLuint Compileshader(GLuint type, const std::string &source) {
  GLuint shaderObject = 0;
  if (type == GL_VERTEX_SHADER) {
    shaderObject = glCreateShader(GL_VERTEX_SHADER);
  } else if (type == GL_FRAGMENT_SHADER) {
    shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
  }
  const char *src = source.c_str();
  glShaderSource(shaderObject, 1, &src, nullptr);
  glCompileShader(shaderObject);
  return shaderObject;
}

GLuint CreateShaderProgram(const std::string &vertexShader, const std::string &fragmentShader) {
  GLuint programObject = glCreateProgram();
  GLuint vertexShaderProgram = Compileshader(GL_VERTEX_SHADER, vertexShader);
  GLuint fragmentShaderProgram = Compileshader(GL_FRAGMENT_SHADER, fragmentShader);
  glAttachShader(programObject, vertexShaderProgram);
  glAttachShader(programObject, fragmentShaderProgram);
  glLinkProgram(programObject);
  glValidateProgram(programObject);
  return programObject;
}

void CreateGraphicsPipeline() {
  g_GraphicsPipelineShaderProgram = CreateShaderProgram(g_VertexShaderSource, g_FragmentShaderSource);
}

void VertexSpecification() {
  //__generating_VAO
  //__lives_on_the_cpu
  const std::vector<GLfloat> vertexPosition{
      -0.8f, -0.8f, 0.0f,
      0.8f, -0.8f, 0.0f,
      0.0f, 0.8f, 0.0f};
  //__setting_things_on_the_GPU
  glGenVertexArrays(1, &g_VertexArrayObject);
  glBindVertexArray(g_VertexArrayObject);
  //__generating_VBO
  glGenBuffers(1, &g_VertexBufferObject);
  glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, vertexPosition.size() * sizeof(GLfloat), vertexPosition.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glBindVertexArray(0);
  glDisableVertexAttribArray(0);
}

GLFWwindow *InitializeProgram() {
  //__initializing_glfw
  if (!glfwInit()) {
    HandleLog(true, "failed to initialize GLFW");
    return nullptr;
  }

  //__using_OpenGl_version_3_3
  glfwWindowHint(GLFW_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_VERSION_MINOR, 3);
  //__using_OpenGl_core_profile_no_backwards_compatibility
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  //__creating_a_GLFW_window_object
  GLFWwindow *window = glfwCreateWindow(g_SCR_WIDTH, g_SCR_HEIGHT, "OpenGl", NULL, NULL);
  if (window == NULL) {
    HandleLog(true, "failed to create GLFW window");
    glfwTerminate();
    return nullptr;
  }

  //__making_the_window_context_the_main_context_on_the_current_thread
  glfwMakeContextCurrent(window);
  //__handling_window_resize
  glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

  //__initializing_GLAD_to_get_the_OpenGl_functions_pointers
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    HandleLog(true, "Failed to initialize GLAD");
    return nullptr;
  }
  return window;
}

void PreDraw() {
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  //__setting_the_color_for_clearing_the_colorbuffer
  glClearColor(0.2f, 0.0f, 0.0f, 1.0f);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  //__setting_the_graphics_pipeline
  glUseProgram(g_GraphicsPipelineShaderProgram);
}

void Draw() {
  glBindVertexArray(g_VertexArrayObject);
  glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

void MainLoop(GLFWwindow *window) {
  while (!glfwWindowShouldClose(window)) {
    ProcessInput(window);
    PreDraw();
    Draw();
    glfwPollEvents();
    glfwSwapBuffers(window);
  }
}

void CleanUp(GLFWwindow *window) {
  glfwDestroyWindow(window);
  glfwTerminate();
  HandleLog(false, "quiting the app...");
}

int main() {
  HandleLog(false, "running app...");

  GLFWwindow *window = InitializeProgram();
  if (window == nullptr) {
    HandleLog(false, "quiting the app due to an error");
    return -1;
  }

  VertexSpecification();
  CreateGraphicsPipeline();
  MainLoop(window);
  CleanUp(window);

  return 0;
}
