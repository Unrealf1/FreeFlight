#include "application/ConcreteApplication.hpp"

int main() {
    ApplicationParameters params;
    params.log_level = spdlog::level::debug;
    params.log_location = "log/log.txt";
    params.view_distance = 100.0f;

    ConcreteApplication app(params);
    app.start();
}