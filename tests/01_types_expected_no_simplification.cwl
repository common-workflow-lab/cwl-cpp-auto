id: Some id
label: some label
doc: documentation that is brief
inputs:
  firstOption: long
  secondOption: int
  thirdOption:
    type: string
    inputBinding:
      prefix: --third_option
  fourthOption:
    - fields:
        species:
          - symbols:
              - homo_sapiens
              - mus_musculus
            type: enum
          - "null"
      type: record
  fifthOption: array
  sixthOption:
    - fields:
        species:
          - symbols:
              - homo_sapiens
              - mus_musculus
            type: enum
          - "null"
      type: record
  seventhOption:
    - fields:
        species: long
        option2: int
      type: record
  eightOption:
    - "null"
    - long
  ninth_option:
    - "null"
    - int
  tenth_option:
    - "null"
    - int
outputs:
  []
cwlVersion: v1.2
class: CommandLineTool
stdout: log.txt
