import re
import string
import sys
import glob
import os


def string_to_string_list(string):
    str_list_out = []
    str_list_aux = string.split("\n")
    for string in str_list_aux:
        string += "\n"
        str_list_out.append(string)
    return str_list_out


def string_list_to_string(string_list):
    return "".join(string_list)


def add_cpp_include(str_list, include_name):
    str_list.insert(0, '#include "' + include_name + '"\n')
    return str_list


def add_cpp_macros_in_methods(file_lines, initial_macro):
    file_lines_out = []

    method_definition = False

    for line in file_lines:
        file_lines_out.append(line)

        if not (
            line.startswith(" ") or line.startswith("\t") or line.startswith("//")
        ) and re.search(r"[a-zA-Z_][a-zA-Z0-9_]*::[a-zA-Z_][a-zA-Z0-9_]*\(.*", line):
            method_definition = True

        if method_definition and line.rstrip(" \n").endswith("{") and not line.startswith("//"):
            file_lines_out.append(initial_macro + "();\n")
            method_definition = False

    return file_lines_out

def add_cpp_include_and_macros(file_lines_in, include_name, initial_macro):
    file_lines_with_include = add_cpp_include(file_lines_in, include_name)
    file_lines_out = add_cpp_macros_in_methods( file_lines_with_include, initial_macro)
    return file_lines_out


#######################################################################################

def process_code(cpp_code_in):
    file_lines_in = string_to_string_list(cpp_code_in)
    file_lines_out_ = add_cpp_include_and_macros(
        file_lines_in, "MyInclude.h", "MACRO_INITIAL" )
    
    return string_list_to_string(file_lines_out_)


def test_empty_file():
    file_lines_in = []
    file_lines_out = add_cpp_include(file_lines_in, "MyInclude.h")
    assert file_lines_out == ['#include "MyInclude.h"\n']

def test_method_one_line():
    cpp_code_in = """\
void A::B()
{
}"""

    cpp_code_out = """\
#include "MyInclude.h"
void A::B()
{
MACRO_INITIAL();
}
"""
    assert process_code(cpp_code_in) == cpp_code_out


def test_method_one_line_blank_lines_before():
    cpp_code_in = """\

void A::B()
{
}"""

    cpp_code_out = """\
#include "MyInclude.h"

void A::B()
{
MACRO_INITIAL();
}
"""
    assert process_code(cpp_code_in) == cpp_code_out


def test_method_one_line_open_bracket_same_line():
    cpp_code_in = """\
void A::B() {
}"""

    cpp_code_out = """\
#include "MyInclude.h"
void A::B() {
MACRO_INITIAL();
}
"""
    assert process_code(cpp_code_in) == cpp_code_out


def test_method_one_line_empty_body():
    cpp_code_in = """\
void A::B() {}"""


    cpp_code_out = """\
#include "MyInclude.h"
void A::B() {}
"""
    assert process_code(cpp_code_in) == cpp_code_out


def test_method_several_lines():
    cpp_code_in = """\
void
A::B(int foo,
     int bar)
{

}"""

    cpp_code_out = """\
#include "MyInclude.h"
void
A::B(int foo,
     int bar)
{
MACRO_INITIAL();

}
"""
    assert process_code(cpp_code_in) == cpp_code_out


def test_method_destructor():
    cpp_code_in = """\
A::~A(){

}"""

    cpp_code_out = """\
#include "MyInclude.h"
A::~A(){

}
"""
    assert process_code(cpp_code_in) == cpp_code_out


def test_method_constructor_and_destructor():
    cpp_code_in = """\
A::A()
{

}
A::~A(){

}"""

    cpp_code_out = """\
#include "MyInclude.h"
A::A()
{
MACRO_INITIAL();

}
A::~A(){

}
"""
    assert process_code(cpp_code_in) == cpp_code_out


def test_method_extra_bracket():
    cpp_code_in = """\
A::B()
{
  for ( , , ){
  }
}"""

    cpp_code_out = """\
#include "MyInclude.h"
A::B()
{
MACRO_INITIAL();
  for ( , , ){
  }
}
"""
    assert process_code(cpp_code_in) == cpp_code_out

def test_method_class_definition():
    cpp_code_in = """\
class TeamCityPrinter : public ::testing::EmptyTestEventListener {
};"""

    cpp_code_out = """\
#include "MyInclude.h"
class TeamCityPrinter : public ::testing::EmptyTestEventListener {
};
"""
    assert process_code(cpp_code_in) == cpp_code_out

def test_method_with_initialization_and_trailing_spaces():
    cpp_code_in = """\
A::B(int foo):
 _member(0)
{ 
}"""

    cpp_code_out = """\
#include "MyInclude.h"
A::B(int foo):
 _member(0)
{ 
MACRO_INITIAL();
}
"""
    assert process_code(cpp_code_in) == cpp_code_out

def test_method_with_comments():
    cpp_code_in = """\
A::B(int foo)
//{ 
//}"""

    cpp_code_out = """\
#include "MyInclude.h"
A::B(int foo)
//{ 
//}
"""
    assert process_code(cpp_code_in) == cpp_code_out


if __name__ == "__main__":
    rootDir = "."
    for dirName, subdirList, fileList in os.walk(rootDir):
        print("Found directory: %s" % dirName)
        for file in fileList:
            #        for file in ['SortWireDataOp.cpp']:
            if file.endswith(".cpp"):
                full_file_name = os.path.join(dirName, file)
                # full_file_name = file
                print("\t%s" % full_file_name)
                try:
                    input_file = open(full_file_name)
                    input_lines = input_file.readlines()
                    input_file.close()
                    output_file = open(full_file_name, "w")

                    output_lines = add_cpp_include_and_macros(
                        input_lines, "libProfiler.h", "PROFILER_F")
                    output_file.writelines(output_lines)
                    output_file.close()

                except:
                    print("Error processing: " + full_file_name)

    # test_method_one_line()
