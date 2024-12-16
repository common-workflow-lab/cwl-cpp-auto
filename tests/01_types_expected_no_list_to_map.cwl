id: Some id
label: some label
doc: documentation that is brief
inputs:
  - id: firstOption
    type: long
  - id: secondOption
    type: int
  - id: thirdOption
    type: string
    inputBinding:
      prefix: --third_option
  - id: fourthOption
    type:
      - fields:
          - name: species
            type:
              - symbols:
                  - homo_sapiens
                  - mus_musculus
                type: enum
              - "null"
        type: record
  - id: fifthOption
    type: array
  - id: sixthOption
    type:
      - fields:
          - name: species
            type:
              - symbols:
                  - homo_sapiens
                  - mus_musculus
                type: enum
              - "null"
        type: record
  - id: seventhOption
    type:
      - fields:
          - name: species
            type: long
          - name: option2
            type: int
        type: record
  - id: eightOption
    type: long?
  - id: ninth_option
    type: int?
  - id: tenth_option
    type: int?
outputs:
  []
cwlVersion: v1.2
class: CommandLineTool
stdout: log.txt
