id: Some id
label: some label
doc: documentation that is brief
inputs:
  firstOption:
    type: long
    inputBinding:
      prefix: --first-option
  inputFile:
    type: File
    inputBinding:
      prefix: --input-file
  outputFile:
    type: string
    inputBinding:
      prefix: --output-file
  speciesSelection:
    - fields:
        species:
          - symbols:
              - homo_sapiens
              - mus_musculus
            type: enum
          - "null"
      type: record
outputs:
  outputFile:
    type: File
    outputBinding:
      glob: $(inputs.outputFile)
cwlVersion: v1.2
class: CommandLineTool
stdout: log.txt
