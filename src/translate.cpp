#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include <math.h>
#include <stdio.h>

static const char *vertex_shader_source = R"(
#version 410 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

uniform float angle;
uniform float scale;
uniform vec2 offset;

out vec3 Color;

void main() {
    Color = color;
    
    mat4 rotation = mat4(
        +cos(angle), -sin(angle), +0.0, +0.0,
        +sin(angle), +cos(angle), +0.0, +0.0,
        +0.0       , +0.0       , +1.0, +0.0,
        +0.0       , +0.0       , +0.0, +1.0
    );

    mat4 scaling = mat4(
        +scale, +0.0  , +0.0, +0.0,
        +0.0  , +scale, +0.0, +0.0,
        +0.0  , +0.0  , +1.0, +0.0,
        +0.0  , +0.0  , +0.0, +1.0
    );

    mat4 translation = mat4(
        +1.0, +0.0, +0.0, +offset.x,
        +0.0, +1.0, +0.0, +offset.y,
        +0.0, +0.0, +1.0, +0.0,
        +0.0, +0.0, +0.0, +1.0
    );

    gl_Position = translation * vec4(position, 1.0);
}
)";

static const char *fragment_shader_source = R"(
#version 410 core

in vec3 Color;
out vec4 FragColor;

void main() {
    FragColor = vec4(Color, 1.0);
}
)";

void check_shader(GLuint shader);
void check_program(GLuint program);

// clang-format off
static GLfloat vertices[] = {
    -0.5f, -0.5f, 0.0f, +1.0f, +0.0f, +0.0f,
    +0.5f, -0.5f, 0.0f, +0.0f, +1.0f, +0.0f,
    +0.5f, +0.5f, 0.0f, +0.0f, +0.0f, +1.0f,
    -0.5f, +0.5f, 0.0f, +1.0f, +1.0f, +0.0f,
};
static GLuint indices[] = {
    0, 1, 2,
    2, 3, 0,
};
// clang-format on

int main(void) {
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(800, 800, "Window", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    glEnable(GL_MULTISAMPLE);

    // 1. setup gpu data
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const void *)(3 * sizeof(GLfloat)));

    // 2. setup gpu program
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    check_shader(vertex_shader);

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    check_shader(fragment_shader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    check_program(program);

    GLint angle_location  = glGetUniformLocation(program, "angle");
    GLint scale_location  = glGetUniformLocation(program, "scale");
    GLint offset_location = glGetUniformLocation(program, "offset");

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);

        float time = glfwGetTime();
        glUniform1f(angle_location, time);

        float scale = 0.7f + 0.5f * sin(glfwGetTime());
        glUniform1f(scale_location, scale);

        float radius = 0.5f;
        float x      = radius * cos(glfwGetTime());
        float y      = radius * sin(glfwGetTime());
        glUniform2f(offset_location, x, y);

        // 3. render
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void check_shader(GLuint shader) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Shader Compilation Error:\n%s\n", infoLog);
    }
}

void check_program(GLuint program) {
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        fprintf(stderr, "Program Linking Error:\n%s\n", infoLog);
    }
}
