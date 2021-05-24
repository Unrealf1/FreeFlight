#pragma once

#include <GL/glew.h>
#include <vector>
#include <spdlog/spdlog.h>
#include <fstream>


GLuint createProgram(GLuint vs, GLuint fs) {
    //Создаем шейдерную программу
    GLuint program = glCreateProgram();

    //Прикрепляем шейдерные объекты
    glAttachShader(program, vs);
    glAttachShader(program, fs);

    //Линкуем программу
    glLinkProgram(program);

    //Проверяем ошибки линковки
    int status = -1;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        GLint errorLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &errorLength);

        std::vector<char> errorMessage;
        errorMessage.resize(errorLength);

        glGetProgramInfoLog(program, errorLength, 0, errorMessage.data());

        spdlog::critical("Failed to link the program:\n{}", errorMessage.data());

        exit(1);
    }
    return program;
}

std::string extractShaderText(const std::string& filepath) {
    std::ifstream shaderFile(filepath.c_str());
    if (shaderFile.fail()) {
        spdlog::critical("Failed to load shader file {}", filepath);
        exit(1);
    }
    std::string shaderFileContent(
        (std::istreambuf_iterator<char>(shaderFile)), (std::istreambuf_iterator<char>())
    );
    return shaderFileContent;
}

GLuint createShader(const char* shaderText, GLenum type) {
    //Создаем шейдерный объект
    GLuint shader = glCreateShader(type);

    //Передаем в шейдерный объект текст шейдера
    glShaderSource(shader, 1, &shaderText, nullptr);

    //Компилируем шейдер
    glCompileShader(shader);

    //Проверяем ошибки компиляции
    int status = -1;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        GLint errorLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errorLength);

        std::vector<char> errorMessage;
        errorMessage.resize(errorLength);

        glGetShaderInfoLog(shader, errorLength, 0, errorMessage.data());

        spdlog::critical("Failed to compile the shader:\n {}", errorMessage.data());

        exit(1);
    }
    return shader;
}

GLuint createVertexShader(const char* vertexShaderText) {
    return createShader(vertexShaderText, GL_VERTEX_SHADER);
}

GLuint createFragmentShader(const char* fragmentShaderText) {
    return createShader(fragmentShaderText, GL_FRAGMENT_SHADER);
}
