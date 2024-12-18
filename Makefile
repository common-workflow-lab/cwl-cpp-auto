# This file is part of cwl-cpp-auto,
# https://github.com/common-workflow-lab/cwl-cpp-auto/, and is
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Contact: common-workflow-language@googlegroups.com

## help         : print this help message and exit
help: Makefile
	@sed -n 's/^##//p' $<

LDLIBS += -lyaml-cpp
CXXFLAGS += -Werror -Wextra -Wall

cwl_v1_2.h: FORCE
	schema-salad-tool --codegen cpp \
		--codegen-spdx-copyright-text "Copyright 2016-2024 CWL Project Contributors" \
		--codegen-spdx-license-identifier "Apache-2.0" \
		https://github.com/common-workflow-language/cwl-v1.2/raw/main/CommonWorkflowLanguage.yml \
		> $@

## clean        : clean up the build
clean: FORCE
	rm -f cwl_output_example cwl_input_example cwl_input_example_store_config

## regen_parser : regenerate the CWL v1.2 parser
regen_parser: cwl_v1_*.h

define run_test
	@result="$(shell eval $(1) | md5sum -b)" ; \
	expected="$(shell eval $(2) | md5sum -b)" ; \
	if [ "$$result" != "$$expected" ] ; then \
		echo test failed '$(1)': $$result != $$expected; exit 1; \
	fi;
endef

## tests        : compile and run the tests
tests: FORCE cwl_output_example cwl_input_example cwl_input_example_store_config
	$(call run_test,./cwl_output_example,cat tests/expected_cwl.cwl)
	$(call run_test,./cwl_input_example tests/expected_cwl.cwl,cat tests/expected_cwl.cwl)
	$(call run_test,./cwl_input_example_store_config tests/01_types.cwl,cat tests/01_types_expected.cwl)
	$(call run_test,./cwl_input_example_store_config tests/01_types.cwl no_simplification,cat tests/01_types_expected_no_simplification.cwl)
	$(call run_test,./cwl_input_example_store_config tests/01_types.cwl no_list_to_map,cat tests/01_types_expected_no_list_to_map.cwl)
	$(call run_test,./cwl_input_example_store_config tests/01_types.cwl no_simplification no_list_to_map,cat tests/01_types_expected_no_simplification_and_list_to_map.cwl)
	$(call run_test,./cwl_input_example tests/02_expression.cwl,cat tests/02_expression_expected.cwl)

	@echo test passed
FORCE:

# Use this to print the value of a Makefile variable
# Example `make print-VERSION`
# From https://www.cmcrossroads.com/article/printing-value-makefile-variable
print-%  : ; @echo $* = $($*)
