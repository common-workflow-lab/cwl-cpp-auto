#include "cwl_v1_2.h"

#include <iostream>

/**
 * This test program creates loads and prints a CWL description.
 *
 * It assumes that printing to stdout works (see cwl_output_example).
 * It loads a CWL description from a file and populates C++ classes.
 */

// using shortened cwl:: namespace instead of w3id_org::cwl
namespace cwl = w3id_org::cwl;

int main(int argc, char** argv) {
    if (argc != 2) return 1;

    auto tool = cwl::load_document(argv[1]);
    cwl::store_document(tool, std::cout);
    return 0;
}
