#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>

#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    glfwInit();

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *w = glfwCreateWindow(600, 600, "Rotating Rectangle", NULL, NULL);
    glfwMakeContextCurrent(w);

    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version  = glGetString(GL_VERSION);

    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    float points[] = {-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f};

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    const char *vertex_shader = "#version 400\n"
                                "uniform float rotation;"
                                "in vec2 vp;"
                                "void main() {"
                                "    float x = vp.x * cos(rotation) - vp.y * sin(rotation);"
                                "    float y = vp.x * sin(rotation) + vp.y * cos(rotation);"
                                "    gl_Position = vec4(x, y, 0.0, 1.0);"
                                "}";

    const char *fragment_shader = "#version 400\n"
                                  "uniform vec4 in_colour;"
                                  "out vec4 frag_colour;"
                                  "void main() {"
                                  "    frag_colour = in_colour;"
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
    glUseProgram(program);

    GLint rotation_loc = glGetUniformLocation(program, "rotation");
    GLint in_col_loc   = glGetUniformLocation(program, "in_colour");

    while (!glfwWindowShouldClose(w)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, 600, 600);

        float angle = (float) glfwGetTime();
        glUniform1f(rotation_loc, angle);
        glUniform4f(in_col_loc, angle / 10, angle / 2, angle / 2, 1.0f);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glfwPollEvents();
        glfwSwapBuffers(w);
    }

    glDeleteProgram(program);
    glDeleteShader(fs);
    glDeleteShader(vs);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    glfwTerminate();
    return 0;
}
