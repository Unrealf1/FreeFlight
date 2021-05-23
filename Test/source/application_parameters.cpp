#include "test_main.hpp"

#include <application/ApplicationParameters.hpp>


TEST(ApplicationParameters, Serialization) {
    ApplicationParameters params;
    params.view_distance = 1.0;

    const char* const filename = "test_params.json";

    ApplicationParametersWriter::write(params, filename);
    auto read = ApplicationParametersReader::read(filename);
    std::remove(filename);

    ASSERT_EQ(params.view_distance, read.view_distance);
}
