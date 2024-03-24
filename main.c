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

  while (!glfwWindowShouldClose(w)) {
    glClearColor(0.2f, 0.4f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(w);
    glfwPollEvents();
  }

  return 0;
}
