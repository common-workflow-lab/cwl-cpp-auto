id: Some id
label: some label
doc: documentation that is brief
inputs:
  firstOption: long
  secondOption:
    type: int
  thirdOption:
    type: string
    inputBinding:
      prefix: --third_option
  fourthOption:
    type:
      - type: record
        fields:
          species:
            type:
              - symbols:
                  - homo_sapiens
                  - mus_musculus
                type: enum
              - "null"
  fifthOption:
    type: array
    items:
      - fields:
          type: record
          species:
            type:
              - symbols:
                  - homo_sapiens
                  - mus_musculus
                type: enum
              - "null"
  sixthOption:
    - type: record
      fields:
        species:
          type:
            - symbols:
                - homo_sapiens
                - mus_musculus
              type: enum
            - "null"
  seventhOption:
    type:
      - type: record
        fields:
          species:
            type: long
          option2: int
  eightOption: long?
  ninth_option:
    type: int?
  tenth_option:
    type:
        - "null"
        - int

outputs:
cwlVersion: v1.2
class: CommandLineTool
stdout: log.txt
