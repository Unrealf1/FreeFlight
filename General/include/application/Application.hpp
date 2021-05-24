#pragma once

#include "render/DrawableUnit.hpp"
#include "render/Updatable.hpp"
#include "player/PlayerDependable.hpp"
#include "camera/Camera.hpp"
#include "application/ApplicationParameters.hpp"

#include <memory>
#include <vector>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <imgui.h>
#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


//Функция обратного вызова для обработки нажатий на клавиатуре
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseCursosPosCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

static void glfw_error_callback(int error, const char* description) {
    spdlog::error("Glfw Error {}: {}", error, description);
}

class Application {
public:
    Application(const ApplicationParameters& params): _params(params) {
        initLogger();
    }

    virtual ~Application() = default;

    void start() {
        _logger->info("started");
        initContext();
        _logger->debug("context initialized");
        initGL();
        _logger->debug("GL initialized");
        initGUI();

        _logger->debug("GUI initialized");

        prepareScene();
        _logger->debug("scene prepared");
        run();
    }

    virtual void handleKey(int, int, int, int) { }

    virtual void handleMouseMove(double, double) { }

    virtual void handleScroll(double, double) { }

protected:
    GLFWwindow* _window = nullptr;

    ApplicationParameters _params;

    double _last_frame_time = 0.0;

    std::shared_ptr<spdlog::logger> _logger;

    virtual void draw() = 0;

    virtual void drawGUI() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void run() {
        onRun();
        while (!glfwWindowShouldClose(_window)) {
            glfwPollEvents(); //Проверяем события ввода

            updateGUI();

            update(); //Обновляем сцену и положение виртуальной камеры

            draw(); 

            drawGUI();

            glfwSwapBuffers(_window); //Переключаем передний и задний буферы
        }
        onStop();
    }

    void update() {
        double current_time = glfwGetTime();
        double dt = current_time - _last_frame_time;
        _last_frame_time = current_time;
        UpdateInfo info;
        info.dt = dt;
        info.window = _window;
        onUpdate(info);
    }

    virtual void onUpdate(const UpdateInfo&) {}

    virtual void onRun() { }

    virtual void onStop() { }

    void initLogger() {
        _logger = spdlog::stdout_color_mt("application");
        _logger->set_level(_params.log_level);
        if (!_params.log_location.empty()) {
            auto rotating = std::make_shared<spdlog::sinks::rotating_file_sink_mt> (
                _params.log_location, 1024*1024, 5, true
            );
            _logger->sinks().push_back(rotating);
        }
    }

    virtual void prepareScene() { }

    virtual void initGUI() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui_ImplGlfw_InitForOpenGL(_window, false);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    virtual void updateGUI() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void initContext() {
        //Инициализируем библиотеку GLFW
        if (!glfwInit()) {
            _logger->error("could not start GLFW3");
            exit(1);
        }

        //Устанавливаем параметры создания графического контекста
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        //Создаем графический контекст (окно)
        GLFWwindow* window = glfwCreateWindow(800, 600, "Amazing triangle", nullptr, nullptr);
        if (!window) {
            _logger->error("could not open window with GLFW3");
            glfwTerminate();
            exit(1);
        }
        //Делаем этот контекст текущим
        glfwMakeContextCurrent(window);
        _window = window;
        _logger->debug("created window");

        glfwSwapInterval(0); //Отключаем вертикальную синхронизацию
        glfwSetWindowUserPointer(_window, this); //Регистрируем указатель на данный объект, чтобы потом использовать его в функциях обратного вызова}

        glfwSetKeyCallback(_window, keyCallback); //Регистрирует функцию обратного вызова для обработки событий клавиатуры
        //glfwSetWindowSizeCallback(_window, windowSizeChangedCallback);
        //glfwSetMouseButtonCallback(_window, mouseButtonPressedCallback);
        glfwSetCursorPosCallback(_window, mouseCursosPosCallback);
        glfwSetScrollCallback(_window, scrollCallback);
        glfwSetErrorCallback(glfw_error_callback);
    }

    void initGL() {
        glewExperimental = GL_TRUE;
        glewInit();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }  
};

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->handleKey(key, scancode, action, mods);
}

void mouseCursosPosCallback(GLFWwindow* window, double xpos, double ypos) {
    auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->handleMouseMove(xpos, ypos);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->handleScroll(xoffset, yoffset);
}