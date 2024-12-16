#include "cwl_v1_2.h"

#include <iostream>

/**
 * This test program creates loads and prints a CWL description.
 *
 * It assumes that printing to stdout works (see cwl_output_example).
 * It loads a CWL description from a file and populates C++ classes.
 */


int main(int argc, char** argv) {
    if (argc < 2) return 1;

    auto tool = cpp_gen::load_document(argv[1]);

    // parse command line
    auto config = cpp_gen::store_config{};
    for (int i{2}; i < argc; ++i) {
        auto sv = std::string_view{argv[i]};
        if (sv == "no_simplification") {
            config.simplifyTypes = false;
        } else if (sv == "no_list_to_map") {
            config.transformListsToMaps = false;
        } else if (sv == "tags") {
            config.generateTags = true;
        }
    }
    cpp_gen::store_document(tool, std::cout, config);
    return 0;
}
