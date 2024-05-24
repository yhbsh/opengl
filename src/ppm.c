#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

int            width, height;
unsigned char *image = NULL;

void read_ppm(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        exit(1);
    }

    char magic[3];
    fscanf(file, "%s", magic);
    if (magic[0] != 'P' || magic[1] != '6') {
        printf("Unsupported file format: %s\n", filename);
        exit(1);
    }

    fscanf(file, "%d %d", &width, &height);
    int maxValue;
    fscanf(file, "%d", &maxValue);
    if (maxValue != 255) {
        printf("Unsupported max value: %d\n", maxValue);
        exit(1);
    }

    image = (unsigned char *) malloc(width * height * 3);
    fread(image, width * height * 3, 1, file);
    fclose(file);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <ppm_file>\n", argv[0]);
        return 1;
    }

    read_ppm(argv[1]);

    glfwInit();
    GLFWwindow *window = glfwCreateWindow(width, height, "PPM Viewer", NULL, NULL);
    glfwMakeContextCurrent(window);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, image);
        glFlush();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    free(image);

    return 0;
}
