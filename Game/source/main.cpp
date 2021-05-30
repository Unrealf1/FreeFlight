// #include "application/ConcreteApplication.hpp"
#include "application/TestApplication.hpp"

int main() {
    ApplicationParameters params;
    params.log_level = spdlog::level::debug;
    params.log_location = "log/log.txt";
    params.view_distance = 100.0f;
    params.window_name = "Test application";

    TestApplication app(params);
    app.start();
}