#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>

const char *vert_source = "#version 120\n"
                          "attribute vec4 position;\n"
                          "void main() {\n"
                          "    gl_Position = position;\n"
                          "}\n";

const char *frag_source = "#version 120\n"
                          "uniform float time;\n"
                          "void main() {\n"
                          "    vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0);\n"
                          "    float pattern = sin(uv.x * 10.0 + time * 10) * sin(uv.y * 10.0 + time * 5);\n"
                          "    vec3 color = vec3(pattern, pattern * 1.0, 0.5);\n"
                          "    gl_FragColor = vec4(color, 1.0);\n"
                          "}\n";

int main(void) {
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(800, 600, "Window", NULL, NULL);
    glfwMakeContextCurrent(window);

    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vert_source, NULL);
    glCompileShader(vert);

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &frag_source, NULL);
    glCompileShader(frag);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);

    glUseProgram(prog);

    glDeleteShader(vert);
    glDeleteShader(frag);

    float vertices[] = {-0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, 0.0f, 1.0f, -0.5f, 0.5f, 0.0f, 1.0f};

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), vertices, GL_STATIC_DRAW);

    GLuint position_attr = glGetAttribLocation(prog, "position");
    glEnableVertexAttribArray(position_attr);
    glVertexAttribPointer(position_attr, 4, GL_FLOAT, GL_FALSE, 0, 0);

    GLuint time_loc = glGetUniformLocation(prog, "time");

    while (!glfwWindowShouldClose(window)) {
        float time = (float) glfwGetTime();
        glUniform1f(time_loc, time);

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_QUADS, 0, 4);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
