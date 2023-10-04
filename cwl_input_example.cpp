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
#if 0
    // declaring information about this tool in general
    tool.cwlVersion = cwl::CWLVersion::v1_2;
    tool.id         = "Some id";
    tool.label      = "some label";
    tool.doc        = "documentation that is brief";
    tool.stdout_    = "log.txt";

    {
        // Demonstrates a binding to `--first-option <SOMENUMBER>`.
        // It will be exposed as `--firstOption <SOMENUMBER>`.
        auto input = cwl::CommandInputParameter{};
        input.id   = "firstOption";
        input.type = cwl::CWLType::long_;

        auto binding       = cwl::CommandLineBinding{};
        binding.prefix     = "--first-option";
        input.inputBinding = binding;

        tool.inputs->push_back(input);
    }
    {
        // Demonstrates a binding to `--input-file <FILE>`.
        // It will be exposed as `--inputFile <FILE>`
        auto input = cwl::CommandInputParameter{};
        input.id   = "inputFile";
        input.type = cwl::CWLType::File;

        auto binding       = cwl::CommandLineBinding{};
        binding.prefix     = "--input-file";
        input.inputBinding = binding;

        tool.inputs->push_back(input);
    }
    {
        // Demonstrates output binding to `--output-file <FILE>`
        // The exposed parameter is `--outputFile <FILE>`.
        //
        // This works in two steps:
        // 1. We register a option that can be set by the user
        auto input = cwl::CommandInputParameter{};
        input.id   = "outputFile";
        input.type = cwl::CWLType::string;

        auto binding       = cwl::CommandLineBinding{};
        binding.prefix     = "--output-file";
        input.inputBinding = binding;

        tool.inputs->push_back(input);

        // 2. We use the input value to declare that we will
        // write a file there
        auto output = cwl::CommandOutputParameter{};
        output.id   = "outputFile";
        output.type = cwl::CWLType::File;

        auto outputBinding = cwl::CommandOutputBinding{};
        // the 'outupt-file' referes to the id of our CommandInputParameter
        outputBinding.glob = "$(inputs.outputFile)";

        output.outputBinding = outputBinding;

        tool.outputs->push_back(output);
    }
    {
        // Demonstrates a record with a single field `species`
        // the `species` field can have the values `homo_sapiens`,
        // `mus_musculus` or just be null.
        // It will be exposed as `--speciesSelection.species [homo_sapiens|mus_musculus]`
        auto input  = cwl::CommandInputParameter{};
        input.id    = "speciesSelection";
        auto record = cwl::CommandInputRecordSchema{};

        auto fieldEntry  = cwl::CommandInputRecordField{};
        *fieldEntry.name = "species";

        auto species = cwl::CommandInputEnumSchema{};
        species.symbols->push_back("homo_sapiens");
        species.symbols->push_back("mus_musculus");

        using ListType = std::vector<std::variant<cwl::CWLType, cwl::CommandInputRecordSchema, cwl::CommandInputEnumSchema, cwl::CommandInputArraySchema, std::string>>;
        fieldEntry.type = ListType{species, "null"};

        using ListType2 = std::vector<cwl::CommandInputRecordField>;
        record.fields = ListType2{fieldEntry};

        using ListType3 = std::vector<std::variant<cwl::CWLType, cwl::CommandInputRecordSchema, cwl::CommandInputEnumSchema, cwl::CommandInputArraySchema, std::string>>;
        input.type = ListType3{record};

        tool.inputs->push_back(input);
    }


    auto y = toYaml(tool);

    YAML::Emitter out;
    out << y;
    std::cout << out.c_str() << "\n";
#endif
}
