cwlVersion: v1.2
class: Workflow
inputs:
  09first_input:
    type:
      - "null"
      - string
  05second_input:
    type: int?
  01third_input:
    type: File
  03fourth_input:
    type:
      type: array
      items: int
  03_fifth_input:
    type:
      - "null"
      - type: array
        items: int
  15_sixth_input: int[]?
  16_seventh_input:
    type: int[]?
steps:
 zz_step_one:
   run:
     class: ExpressionTool
     inputs: []
     outputs: []
     expression: ${return {}; }
     requirements:
       InlineJavascriptRequirement: {}
   in: []
   out: []
 00_step_two:
   out: []
   run:
     inputs: []
     requirements:
       InlineJavascriptRequirement: {}
     outputs: []
     expression: ${return2 {}; }
     class: ExpressionTool
   in: []
outputs:
  zz_first_output:
    type: File
    outputSource: 01third_input
  ll_second_output:
    type: string
    outputSource: 09first_input
  aa_third_output:
    type: int
    outputSource: 05second_input
