inputs:
  09first_input: string?
  05second_input: int?
  01third_input: File
  03fourth_input: int[]
  03_fifth_input: int[]?
  15_sixth_input: int[]?
  16_seventh_input: int[]?
outputs:
  zz_first_output:
    outputSource: 01third_input
    type: File
  ll_second_output:
    outputSource: 09first_input
    type: string
  aa_third_output:
    outputSource: 05second_input
    type: int
cwlVersion: v1.2
class: Workflow
steps:
  zz_step_one:
    in:
      []
    out:
      []
    run:
      inputs:
        []
      outputs:
        []
      requirements:
        InlineJavascriptRequirement:
          {}
      class: ExpressionTool
      expression: ${return {}; }
  00_step_two:
    in:
      []
    out:
      []
    run:
      inputs:
        []
      outputs:
        []
      requirements:
        InlineJavascriptRequirement:
          {}
      class: ExpressionTool
      expression: ${return2 {}; }
