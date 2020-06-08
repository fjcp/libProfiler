import re
import string
import sys
import glob
import os

def string_to_string_list(string):
    return string.split("\n")

def string_list_to_string(string_list):
    return "\n".join(string_list)

def add_cpp_include(str_list, include_name):
    str_list.insert(0, '#include "' + include_name + '"')
    return str_list


def add_cpp_macros_in_methods(file_lines, initial_macro, final_macro):
    file_lines_out = []

    method_definition = False

    for line in file_lines:
        file_lines_out.append(line)

        if re.search(r'[a-zA-Z_][a-zA-Z0-9_]*::[a-zA-Z_][a-zA-Z0-9_]*(.*)', line):
            method_definition = True

        if method_definition and line.endswith('{'):
            file_lines_out.append(initial_macro + "();")
            method_definition = False

        if line == "}":
            file_lines_out.insert(len(file_lines_out) - 1, final_macro + "();")

    return file_lines_out


def add_cpp_include_and_macros(file_lines_in, include_name, initial_macro, final_macro):
    file_lines_with_include = add_cpp_include(file_lines_in, include_name)
    file_lines_out = add_cpp_macros_in_methods(
        file_lines_with_include, initial_macro, final_macro
    )
    return file_lines_out


#######################################################################################


def test_empty_file():
    file_lines_in = []
    file_lines_out = add_cpp_include(file_lines_in, "MyInclude.h")
    assert file_lines_out == ['#include "MyInclude.h"']


def test_method_one_line():
    cpp_code_in = """\
void A::B()
{
}
"""
    file_lines_in = string_to_string_list(cpp_code_in)
    file_lines_out = add_cpp_include_and_macros(
        file_lines_in, "MyInclude.h", "MACRO_INITIAL", "MACRO_FINAL"
    )

    cpp_code_out = """\
#include "MyInclude.h"
void A::B()
{
MACRO_INITIAL();
MACRO_FINAL();
}
"""
    assert string_list_to_string(file_lines_out) == cpp_code_out

def test_method_one_line_blank_lines_before():
    cpp_code_in = """\

void A::B()
{
}
"""
    file_lines_in = string_to_string_list(cpp_code_in)
    file_lines_out = add_cpp_include_and_macros(
        file_lines_in, "MyInclude.h", "MACRO_INITIAL", "MACRO_FINAL"
    )

    cpp_code_out = """\
#include "MyInclude.h"

void A::B()
{
MACRO_INITIAL();
MACRO_FINAL();
}
"""
    assert string_list_to_string(file_lines_out) == cpp_code_out

def test_method_one_line_open_bracket_same_line():
    cpp_code_in = """\
void A::B() {
}
"""
    file_lines_in = string_to_string_list(cpp_code_in)
    file_lines_out = add_cpp_include_and_macros(
        file_lines_in, "MyInclude.h", "MACRO_INITIAL", "MACRO_FINAL"
    )

    cpp_code_out = """\
#include "MyInclude.h"
void A::B() {
MACRO_INITIAL();
MACRO_FINAL();
}
"""
    assert string_list_to_string(file_lines_out) == cpp_code_out

def test_method_one_line_empty_body():
    cpp_code_in = """\
void A::B() {}
"""
    file_lines_in = string_to_string_list(cpp_code_in)
    file_lines_out = add_cpp_include_and_macros(
        file_lines_in, "MyInclude.h", "MACRO_INITIAL", "MACRO_FINAL"
    )

    cpp_code_out = """\
#include "MyInclude.h"
void A::B() {}
"""
    assert string_list_to_string(file_lines_out) == cpp_code_out

def test_method_several_lines():
    cpp_code_in = """\
void
A::B(int foo,
     int bar)
{

}
"""
    file_lines_in = string_to_string_list(cpp_code_in)
    file_lines_out = add_cpp_include_and_macros(
        file_lines_in, "MyInclude.h", "MACRO_INITIAL", "MACRO_FINAL"
    )

    cpp_code_out = """\
#include "MyInclude.h"
void
A::B(int foo,
     int bar)
{
MACRO_INITIAL();

MACRO_FINAL();
}
"""
    assert string_list_to_string(file_lines_out) == cpp_code_out


if __name__ == "__main__":
    pass
    #test_method_one_line()
