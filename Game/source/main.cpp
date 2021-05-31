// #include "application/ConcreteApplication.hpp"
#include "application/TestApplication.hpp"

int main() {
    spdlog::set_level(spdlog::level::debug);

    ApplicationParameters params;
    params.log_level = spdlog::level::debug;
    params.log_location = "log/log.txt";
    params.view_distance = 1500.0f;
    params.window_name = "Test application";
    params.near_plane = 0.1f;
    params.far_plane = 2000.0f;

    TestApplication app(params);
    app.start();
}