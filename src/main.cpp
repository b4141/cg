#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

//__global_settings__globals_are_prefixed_with_[g_]_in_this_app
const unsigned int g_SCR_WIDTH = 640;
const unsigned int g_SCR_HEIGHT = 400;
//__VAO
GLuint g_VertexArrayObject = 0;
//__VBO
GLuint g_VertexBufferObject = 0;
//__program_object_for_the_shaders
GLuint g_GraphicsPipelineShaderProgram = 0;

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
  //__if_the_ESCAPE_key_is_pressed_quite_the_app
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

std::string LoadShaderAsString(const std::string &filename) {
  //__resulting_shader_program_loaded_as_a_single_string
  std::string result = "";
  std::string line = "";
  std::ifstream file(filename.c_str());
  if (file.is_open()) {
    while (std::getline(file, line)) {
      result += line + '\n';
    }
    file.close();
  } else {
    HandleLog(true, "failed to load the shader from source (" + filename + ")");
  }

  return result;
}

GLuint Compileshader(GLuint type, const std::string &source) {
  GLuint shaderObject = 0;
  if (type == GL_VERTEX_SHADER) {
    shaderObject = glCreateShader(GL_VERTEX_SHADER);
  } else if (type == GL_FRAGMENT_SHADER) {
    shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
  }
  //__getting_shader_source_code_as_c_format_string
  const char *src = source.c_str();
  glShaderSource(shaderObject, 1, &src, nullptr);
  //__compiling_the_shader
  glCompileShader(shaderObject);

  //__retriving_the_result_of_the_compilation
  int compResult;
  //__retriving_the_compilation_status_and_storing_it_in_the_compResult
  glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &compResult);
  //__checking_the_compResult
  if (compResult == GL_FALSE) {
    int length;
    glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
    char *errorMessage = new char[length];
    glGetShaderInfoLog(shaderObject, length, &length, errorMessage);
    if (type == GL_VERTEX_SHADER) {
      HandleLog(true, "GL_VERTEX_SHADER compilation failed!");
      HandleLog(true, (std::string)errorMessage);
    } else if (type == GL_FRAGMENT_SHADER) {
      HandleLog(true, "GL_FRAGMENT_SHADER compilation failed!");
      HandleLog(true, (std::string)errorMessage);
    }
    //__reclaiming_memory
    delete[] errorMessage;
    //__deleting_the_broken_shader
    glDeleteShader(shaderObject);
    return 0;
  }

  return shaderObject;
}

GLuint CreateShaderProgram(const std::string &vertexShader, const std::string &fragmentShader) {
  GLuint programObject = glCreateProgram();
  //__compiling_both_vertex_and_fragment_shaders
  GLuint vertexShaderProgram = Compileshader(GL_VERTEX_SHADER, vertexShader);
  GLuint fragmentShaderProgram = Compileshader(GL_FRAGMENT_SHADER, fragmentShader);
  //__attaching_the_compiled_shaders
  glAttachShader(programObject, vertexShaderProgram);
  glAttachShader(programObject, fragmentShaderProgram);
  //__linking_the_two_programs_together
  glLinkProgram(programObject);
  //__validating_the_program
  glValidateProgram(programObject);

  //__detaching_the_individual_shaders
  glDetachShader(programObject, vertexShaderProgram);
  glDetachShader(programObject, fragmentShaderProgram);
  //__deleting_the_individual_shaders
  glDeleteShader(vertexShaderProgram);
  glDeleteShader(fragmentShaderProgram);

  return programObject;
}

void CreateGraphicsPipeline() {
  //__loading_the_vertex_and_fragment_shaders_source_code
  std::string vertexShaderSource = LoadShaderAsString("./shaders/vertexShader.glsl");
  std::string fragmentShaderSource = LoadShaderAsString("./shaders/fragmentShader.glsl");
  //__this_function_takes_the_vertex_and_fragment_shaders_and_compiles_them
  g_GraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void VertexSpecification() {
  //__lives_on_the_CPU
  const std::vector<GLfloat> vertexPosition{
      -0.8f, -0.8f, 0.0f,
      0.8f, -0.8f, 0.0f,
      0.0f, 0.8f, 0.0f};

  //__VBO_are_buffers_on_the_GPU_vram_to_store_the_data
  //__VAO_are_the_way_we_can_access_attributes_to_the_data_stored_in_the_VBO_using_shaders

  //__setting_things_on_the_GPU
  //__generating_VAO__VAO_is_kinda_a_wrapper_around_the_VBO
  //__the_VAO_is_responsible_for_accessing_the_attributes_of_each_vertex
  glGenVertexArrays(1, &g_VertexArrayObject);
  //__selecting_the_VAO_to_use
  glBindVertexArray(g_VertexArrayObject);

  //__generating_VBO
  //__the_VBO_will_store_the_data_on_the_GPU_vram
  glGenBuffers(1, &g_VertexBufferObject);
  //__selecting_the_buffer_object_to_work_within
  //__this_is_like_giving_the_created_buffer_the_label_GL_ARRAY_BUFFER
  glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject);
  //__sending_the_data_to_the_currently_binded_buffer__sending_data_to_GPU
  //__when_sending_the_data_to_the_target_GL_ARRAY_BUFFER_it_store_it_in_the_bounded_buffer
  glBufferData(GL_ARRAY_BUFFER, vertexPosition.size() * sizeof(GLfloat), vertexPosition.data(), GL_STATIC_DRAW);

  //__enabling_the_vertex_attrib_at_index_0__in_this_case_position_which_will_be_accessed_by_the_vertex_shader
  glEnableVertexAttribArray(0);
  //__structuring_the_vertex_data_at_index_0
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
  //__unbinding_the_current_VAO
  glBindVertexArray(0);
  //__disableing_the_vertex_attrib_at_index_0
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

  //__request_a_double_buffer_for_smooth_updating
  glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
  glfwWindowHint(GLFW_DEPTH_BITS, 24);

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
  //__disabling_the_depth_test_and_the_face_culling
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  //__setting_the_color_for_clearing_the_colorbuffer
  glClearColor(0.2f, 0.0f, 0.0f, 1.0f);
  //__clearing_the_color_buffer_and_the_depth_buffer
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void Draw() {
  //__using_the_graphics_pipeline_created_earlier
  glUseProgram(g_GraphicsPipelineShaderProgram);
  //__enabling_the_attributes
  glBindVertexArray(g_VertexArrayObject);
  //__selecting_the_vertex_buffer_object_to_enable
  glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject);
  //__rendering_the_data__from_0_to_3_vertices
  glDrawArrays(GL_TRIANGLES, 0, 3);
  //__stop_using_the_current_graphics_pipeline
  //__this_is_not_necessary_if_there_is_only_one_graphics_pipeline
  glUseProgram(0);
}

void MainLoop(GLFWwindow *window) {
  //__while_the_app_is_running
  while (!glfwWindowShouldClose(window)) {
    //__handling_input
    ProcessInput(window);
    //__setup_for_things_before_drawing__OpenGl_State
    PreDraw();
    //__drawing_call
    Draw();
    //__pollevents_and_swapping_buffers
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

  //__setting_the_geometry
  VertexSpecification();
  //__creating_the_graphics_pipeline__the_vertex_and_fragment_shader
  CreateGraphicsPipeline();
  //__calling_the_main_loop
  MainLoop(window);
  //__cleanup_and_terminating_the_app
  CleanUp(window);

  return 0;
}
