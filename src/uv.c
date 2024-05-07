#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <math.h>
#include <stdio.h>

void printOpenGLInfo() {
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *vendor = glGetString(GL_VENDOR);
    const GLubyte *version = glGetString(GL_VERSION);
    const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("OpenGL Renderer: %s\n", renderer);
    printf("OpenGL Vendor: %s\n", vendor);
    printf("OpenGL Version: %s\n", version);
    printf("GLSL Version: %s\n", glslVersion);
}

int main() {
    GLFWwindow *window;
    const int width = 800;
    const int height = 600;
    unsigned char color_buffer[width * height * 3];

    glfwInit();
    window = glfwCreateWindow(width, height, "Simple Window", NULL, NULL);
    glfwMakeContextCurrent(window);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            color_buffer[index] = (unsigned char)((255 * x) / width);
            color_buffer[index + 1] = (unsigned char)((255 * y) / height);
            color_buffer[index + 2] = 0;
        }
    }

    printOpenGLInfo();

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, color_buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    float time = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int index = (y * width + x) * 3;
                color_buffer[index] = (unsigned char)(127.5 * (1.0 + sin(x * 0.01f + time)));
                color_buffer[index + 1] = (unsigned char)(127.5 * (1.0 + sin(y * 0.01f + time)));
                color_buffer[index + 2] = (unsigned char)(127.5 * (1.0 + sin((x + y) * 0.01f + time)));
            }
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, color_buffer);

        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);
        glEnd();

        glDisable(GL_TEXTURE_2D);

        glfwSwapBuffers(window);
        glfwPollEvents();

        time += 0.05f;
    }

    glDeleteTextures(1, &texture);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
