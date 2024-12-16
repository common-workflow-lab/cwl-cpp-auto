#include "cwl_v1_2.h"

#include <iostream>

/**
 * This test program creates loads and prints a CWL description.
 *
 * It assumes that printing to stdout works (see cwl_output_example).
 * It loads a CWL description from a file and populates C++ classes.
 */


int main(int argc, char** argv) {
    if (argc != 2) return 1;

    auto tool = cpp_gen::load_document(argv[1]);
    cpp_gen::store_document(tool, std::cout);
    return 0;
}
