#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned int compile_shader(unsigned int type, const char *source) {
  unsigned int id = glCreateShader(type);
  glShaderSource(id, 1, &source, NULL);
  glCompileShader(id);

  int res;
  glGetShaderiv(id, GL_COMPILE_STATUS, &res);

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

  const char *vs = "#version 120\n"
                   "attribute vec4 position;\n"
                   "\n"
                   "void main() {\n"
                   "  gl_Position = position;\n"
                   "}\n";

  const char *fs = "#version 120\n"
                   "\n"
                   "void main() {\n"
                   "  gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0); // Red color with full opacity\n"
                   "}\n";

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
