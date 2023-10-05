#include "cwl_v1_2.h"

#include <iostream>

/**
 * This test program creates loads and prints a CWL description.
 *
 * It assumes that printing to stdout works (see cwl_output_example).
 * It loads a CWL description from a file and populates C++ classes.
 */


// using shortened cwl:: namespace instead of https___w3id_org_cwl_cwl
namespace cwl = https___w3id_org_cwl_cwl;

int main(int argc, char** argv) {
    if (argc != 2) return 1;

    auto yaml = YAML::LoadFile(argv[1]);
    auto tool = cwl::CommandLineTool{};
    fromYaml(yaml, tool);

    auto y = toYaml(tool);

    YAML::Emitter out;
    out << y;
    std::cout << out.c_str() << "\n";

    return 0;
}
