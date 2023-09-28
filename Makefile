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

cwl_v1_2.h: FORCE
	schema-salad-tool --codegen cpp \
		https://github.com/common-workflow-language/cwl-v1.2/raw/1.2.1_proposed/CommonWorkflowLanguage.yml \
		> $@

## clean        : clean up the build
clean: FORCE
	rm -f cwl_output_example

## regen_parser : regenerate the CWL v1.2 parser
regen_parser: cwl_v1_*.h

## tests        : compile and run the tests
tests: FORCE cwl_output_example
	@result="$(shell ./cwl_output_example | md5sum)" ; \
	expected="917791e054e1703bd42b181154b885d7  -" ;\
	if [ "$$result" = "$$expected" ] ; \
		then echo test passed ; \
	else echo test failed $$result != $$expected; exit 1; \
	fi
FORCE:

# Use this to print the value of a Makefile variable
# Example `make print-VERSION`
# From https://www.cmcrossroads.com/article/printing-value-makefile-variable
print-%  : ; @echo $* = $($*)
