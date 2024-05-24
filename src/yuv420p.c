#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

const char *vs = "#version 120\n"
                 "attribute vec4 position;\n"
                 "attribute vec2 texCoord;\n"
                 "varying vec2 texCoordVarying;\n"
                 "void main() {\n"
                 "    gl_Position = position;\n"
                 "    texCoordVarying = texCoord;\n"
                 "}\n";

const char *fs = "#version 120\n"
                 "varying vec2 texCoordVarying;\n"
                 "uniform sampler2D textureY;\n"
                 "uniform sampler2D textureU;\n"
                 "uniform sampler2D textureV;\n"
                 "void main() {\n"
                 "    vec3 yuv;\n"
                 "    vec3 rgb;\n"
                 "    yuv.x = texture2D(textureY, texCoordVarying).r;\n"
                 "    yuv.y = texture2D(textureU, texCoordVarying).r - 0.5;\n"
                 "    yuv.z = texture2D(textureV, texCoordVarying).r - 0.5;\n"
                 "    rgb = mat3(1.0, 1.0, 1.0, 0.0, -0.344, 1.772, 1.402, -0.714, 0.0) * yuv;\n"
                 "    gl_FragColor = vec4(rgb, 1.0);\n"
                 "}\n";

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return -1;
    }

    // Create a window
    GLFWwindow *window = glfwCreateWindow(640, 480, "YUV420p to RGB", NULL, NULL);
    if (!window) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        printf("Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    // Create and compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vs, NULL);
    glCompileShader(vertexShader);

    // Create and compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fs, NULL);
    glCompileShader(fragmentShader);

    // Create shader program and link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Generate YUV420p buffer
    int            width     = 640;
    int            height    = 480;
    unsigned char *yuvBuffer = (unsigned char *) malloc(width * height * 3 / 2);
    // Fill the YUV buffer with test data (e.g., color gradient)
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            yuvBuffer[i * width + j] = (i + j) % 256; // Y
        }
    }
    for (int i = 0; i < height / 2; i++) {
        for (int j = 0; j < width / 2; j++) {
            yuvBuffer[width * height + i * width / 2 + j]                      = (i + j) % 256; // U
            yuvBuffer[width * height + width * height / 4 + i * width / 2 + j] = (i + j) % 256; // V
        }
    }

    // Create textures for Y, U, and V planes
    GLuint textureY, textureU, textureV;
    glGenTextures(1, &textureY);
    glGenTextures(1, &textureU);
    glGenTextures(1, &textureV);

    // Set up Y texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureY);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, yuvBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glUniform1i(glGetUniformLocation(shaderProgram, "textureY"), 0);

    // Set up U texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureU);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width / 2, height / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, yuvBuffer + width * height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glUniform1i(glGetUniformLocation(shaderProgram, "textureU"), 1);

    // Set up V texture
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureV);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width / 2, height / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, yuvBuffer + width * height + width * height / 4);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glUniform1i(glGetUniformLocation(shaderProgram, "textureV"), 2);

    // Set up vertex data
    GLfloat vertices[] = {-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Set vertex attributes
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), vertices);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), vertices + 2);
        glEnableVertexAttribArray(1);

        // Draw quad
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    glDeleteTextures(1, &textureY);
    glDeleteTextures(1, &textureU);
    glDeleteTextures(1, &textureV);
    glDeleteProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    free(yuvBuffer);
    glfwTerminate();

    return 0;
}