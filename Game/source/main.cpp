// #include "application/ConcreteApplication.hpp"
#include "application/TestApplication.hpp"

int main() {
    spdlog::set_level(spdlog::level::debug);

    auto params = ApplicationParametersReader::read("parameters.json");

    TestApplication app(params);
    app.start();
}