#pragma one 

#include <GL/glfw3.h>


class Scene {
public:
    void init() {

    }

    void initContext() {
        //Инициализируем библиотеку GLFW
        if (!glfwInit())
        {
            std::cerr << "ERROR: could not start GLFW3\n";
            exit(1);
        }

        //Устанавливаем параметры создания графического контекста
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        //Создаем графический контекст (окно)
        GLFWwindow* window = glfwCreateWindow(800, 600, "Amazing triangle", nullptr, nullptr);
        if (!window)
        {
            std::cerr << "ERROR: could not open window with GLFW3\n";
            glfwTerminate();
            exit(1);
        }

        //Делаем этот контекст текущим
        glfwMakeContextCurrent(window);
        _window = window;

        glfwSwapInterval(0); //Отключаем вертикальную синхронизацию
        glfwSetWindowUserPointer(_window, this); //Регистрируем указатель на данный объект, чтобы потом использовать его в функциях обратного вызова}

        glfwSetKeyCallback(_window, keyCallback); //Регистрирует функцию обратного вызова для обработки событий клавиатуры
        //glfwSetWindowSizeCallback(_window, windowSizeChangedCallback);
        //glfwSetMouseButtonCallback(_window, mouseButtonPressedCallback);
        glfwSetCursorPosCallback(_window, mouseCursosPosCallback);
        glfwSetScrollCallback(_window, scrollCallback);
    }

    void initGL() {
        glewExperimental = GL_TRUE;
        glewInit();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }

    GLFWwindow* _window = nullptr;
};
