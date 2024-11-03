#include <OpenGL/gl3.h>

#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
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

    float points[] = {-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f};

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

    const char *vert_shader = "#version 400\n"
                              "uniform float rotation;"
                              "in vec2 vp;"
                              "void main() {"
                              "    float x = vp.x * cos(rotation) - vp.y * sin(rotation);"
                              "    float y = vp.x * sin(rotation) + vp.y * cos(rotation);"
                              "    gl_Position = vec4(x, y, 0, 1.0);"
                              "}";

    const char *frag_shader = "#version 400\n"
                              "out vec4 frag_colour;"
                              "void main() {"
                              "    frag_colour = vec4(1.0, 0.5, 0.5, 1.0);"
                              "}";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vert_shader, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &frag_shader, NULL);
    glCompileShader(fs);

    GLuint program = glCreateProgram();
    glAttachShader(program, fs);
    glAttachShader(program, vs);
    glLinkProgram(program);
    glUseProgram(program);

    GLint rotation_loc = glGetUniformLocation(program, "rotation");

    while (!glfwWindowShouldClose(w)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float rotation_angle = (float)glfwGetTime();
        glUniform1f(rotation_loc, rotation_angle);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

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
