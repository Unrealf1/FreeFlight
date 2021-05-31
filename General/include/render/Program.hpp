#pragma once

#include <GL/glew.h>
#include <vector>
#include <spdlog/spdlog.h>
#include <fstream>


static void inline checkErrorLength(GLint errorLength, const std::string& error_reason) {
    if (errorLength < 0) {
        spdlog::critical("Could not read error message({}): error length is less than 0", error_reason);
        exit(2);
    }
}

inline GLuint createProgram(GLuint vs, GLuint fs) {
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

        std::string reason = "Failed to link the program";
        checkErrorLength(errorLength, reason);

        std::vector<char> errorMessage;
        errorMessage.resize(static_cast<std::size_t>(errorLength));

        glGetProgramInfoLog(program, errorLength, 0, errorMessage.data());

        spdlog::critical("{}:\n{}", reason, errorMessage.data());

        exit(1);
    }
    return program;
}

inline std::string  extractShaderText(const std::string& filepath) {
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

inline GLuint createShader(const char* shaderText, GLenum type) {
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

        std::string reason = "Failed to compile the shader";
        checkErrorLength(errorLength, reason);

        std::vector<char> errorMessage;
        errorMessage.resize(static_cast<std::size_t>(errorLength));

        glGetShaderInfoLog(shader, errorLength, 0, errorMessage.data());

        spdlog::critical("{}:\n {}", reason, errorMessage.data());

        exit(1);
    }
    return shader;
}

inline GLuint createVertexShader(const char* vertexShaderText) {
    return createShader(vertexShaderText, GL_VERTEX_SHADER);
}

inline GLuint createFragmentShader(const char* fragmentShaderText) {
    return createShader(fragmentShaderText, GL_FRAGMENT_SHADER);
}
