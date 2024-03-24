#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <stdio.h>
#include <stdlib.h>

const char *read_file(const char *file_name) {
  FILE *f = fopen(file_name, "rb");
  fseek(f, 0, SEEK_END);
  size_t fsize = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buff = malloc(fsize + 1);
  fread(buff, fsize, 1, f);

  buff[fsize] = '\0';
  fclose(f);

  return buff;
}

static unsigned int compile_shader(unsigned int type, const char *source) {
  unsigned int id = glCreateShader(type);
  glShaderSource(id, 1, &source, NULL);
  glCompileShader(id);

  return id;
}

static unsigned int create_shader(const char *vert, const char *frag) {
  unsigned int prog = glCreateProgram();
  unsigned int vs = compile_shader(GL_VERTEX_SHADER, vert);
  unsigned int fs = compile_shader(GL_FRAGMENT_SHADER, frag);

  glAttachShader(prog, vs);
  glAttachShader(prog, fs);
  glLinkProgram(prog);
  glValidateProgram(prog);

  glDeleteShader(vs);
  glDeleteShader(fs);

  return prog;
}

static void error_callback(int error, const char *description) { fprintf(stderr, "[ERROR]: %s\n", description); }

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

int main(void) {
  glfwSetErrorCallback(error_callback);
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *w = glfwCreateWindow(800, 600, "w", NULL, NULL);

  glfwMakeContextCurrent(w);
  glfwSetKeyCallback(w, key_callback);

  glewExperimental = GL_TRUE;
  GLenum error = glewInit();
  if (GLEW_OK != error) {
    fprintf(stderr, "[ERROR]: %s\n", glewGetErrorString(error));
    return 1;
  }

  const GLubyte *renderer = glGetString(GL_RENDERER);
  const GLubyte *version = glGetString(GL_VERSION);
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", version);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  float points[] = {0.0f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f, -0.5f, -0.5f, 0.0f};
  GLuint vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  const char *vertex_shader = "#version 400\n"
                              "in vec3 vp;"
                              "void main() {"
                              "  gl_Position = vec4(vp, 1.0);"
                              "}";

  const char *fragment_shader = "#version 400\n"
                                "out vec4 frag_colour;"
                                "void main() {"
                                "  frag_colour = vec4(1.0, 1.0, 1.0, 1.0);"
                                "}";

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_shader, NULL);
  glCompileShader(vs);
  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_shader, NULL);
  glCompileShader(fs);

  GLuint program = glCreateProgram();
  glAttachShader(program, fs);
  glAttachShader(program, vs);
  glLinkProgram(program);

  while (!glfwWindowShouldClose(w)) {
    // wipe the drawing surface clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
    glBindVertexArray(vao);
    // draw points 0-3 from the currently bound VAO with current in-use shader
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwPollEvents();
    glfwSwapBuffers(w);
  }
  return 0;
}
