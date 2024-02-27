#define GL_SILENCE_DEPRECATION
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

  int res;
  glGetShaderiv(id, GL_COMPILE_STATUS, &res);

  if (res == GL_FALSE) {
    int len;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
    char *message = (char *)alloca(len * sizeof(char));
    glGetShaderInfoLog(id, len, &len, message);
    const char *vt = type == GL_VERTEX_SHADER ? "vertex" : "fragment";
    fprintf(stderr, "[ERROR]: could not compile %s shader: %s\n", vt, message);
    glDeleteShader(id);

    return 0;
  }

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

int main(void) {
  glfwInit();

  GLFWwindow *w = glfwCreateWindow(800, 600, "w", NULL, NULL);

  glfwMakeContextCurrent(w);

  printf("version = %s\n", glGetString(GL_VERSION));

  float pos[6] = {-0.5f, -0.5f, 0.0f, 0.5f, 0.5f, -0.5f};

  unsigned int buff;
  glGenBuffers(1, &buff);
  glBindBuffer(GL_ARRAY_BUFFER, buff);
  glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), pos, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

  const char *vs = read_file("main.vert");
  const char *fs = read_file("main.frag");

  unsigned int shader = create_shader(vs, fs);

  glUseProgram(shader);

  while (!glfwWindowShouldClose(w)) {
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(w);
    glfwPollEvents();
  }

  return 0;
}
