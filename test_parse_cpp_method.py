import re
import string
import sys
import glob
import os

def string_to_string_list(string):
    str_list_out =[]
    str_list_aux = string.split("\n")
    for string in str_list_aux:
        string +='\n'
        str_list_out.append(string)
    return str_list_out

def string_list_to_string(string_list):
    return "".join(string_list)

def add_cpp_include(str_list, include_name):
    str_list.insert(0, '#include "' + include_name + '"\n')
    return str_list


def add_cpp_macros_in_methods(file_lines, initial_macro, final_macro):
    file_lines_out = []

    method_definition = False
    final_macro_pending = False

    for line in file_lines:
        file_lines_out.append(line)

        if re.search(r'[a-zA-Z_][a-zA-Z0-9_]*::[a-zA-Z_][a-zA-Z0-9_]*\(.*', line):
            method_definition = True

        if method_definition and not final_macro_pending and line.endswith('{\n'):
            file_lines_out.append(initial_macro + "();\n")
            method_definition = False
            final_macro_pending = True

        if line == "}\n" and final_macro_pending:
            file_lines_out.insert(len(file_lines_out) - 1, final_macro + "();\n")
            final_macro_pending = False

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
    assert file_lines_out == ['#include "MyInclude.h"\n']


def test_method_one_line():
    cpp_code_in = """\
void A::B()
{
}"""
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
}"""
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
}"""
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
void A::B() {}"""
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

}"""
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

def test_method_destructor():
    cpp_code_in = """\
A::~A(){

}"""
    file_lines_in = string_to_string_list(cpp_code_in)
    file_lines_out = add_cpp_include_and_macros(
        file_lines_in, "MyInclude.h", "MACRO_INITIAL", "MACRO_FINAL"
    )

    cpp_code_out = """\
#include "MyInclude.h"
A::~A(){

}
"""
    assert string_list_to_string(file_lines_out) == cpp_code_out

def test_method_constructor_and_destructor():
    cpp_code_in = """\
A::A()
{

}
A::~A(){

}"""
    file_lines_in = string_to_string_list(cpp_code_in)
    file_lines_out = add_cpp_include_and_macros(
        file_lines_in, "MyInclude.h", "MACRO_INITIAL", "MACRO_FINAL"
    )

    cpp_code_out = """\
#include "MyInclude.h"
A::A()
{
MACRO_INITIAL();

MACRO_FINAL();
}
A::~A(){

}
"""
    assert string_list_to_string(file_lines_out) == cpp_code_out

def test_method_extra_bracket():
    cpp_code_in = """\
A::B()
{
  for ( , , ){
  }
}"""
    file_lines_in = string_to_string_list(cpp_code_in)
    file_lines_out = add_cpp_include_and_macros(
        file_lines_in, "MyInclude.h", "MACRO_INITIAL", "MACRO_FINAL"
    )

    cpp_code_out = """\
#include "MyInclude.h"
A::B()
{
MACRO_INITIAL();
  for ( , , ){
  }
MACRO_FINAL();
}
"""
    assert string_list_to_string(file_lines_out) == cpp_code_out
    
def test_method_class_definition():
    cpp_code_in = """\
class TeamCityPrinter : public ::testing::EmptyTestEventListener {
};"""
    file_lines_in = string_to_string_list(cpp_code_in)
    file_lines_out = add_cpp_include_and_macros(
        file_lines_in, "MyInclude.h", "MACRO_INITIAL", "MACRO_FINAL"
    )

    cpp_code_out = """\
#include "MyInclude.h"
class TeamCityPrinter : public ::testing::EmptyTestEventListener {
};
"""
    assert string_list_to_string(file_lines_out) == cpp_code_out



if __name__ == "__main__":
    rootDir = '.'
    for dirName, subdirList, fileList in os.walk(rootDir):
        print('Found directory: %s' % dirName)
        for file in fileList:
#        for file in ['SortWireDataOp.cpp']:
            if file.endswith(".cpp"):
                full_file_name = os.path.join(dirName, file)
                #full_file_name = file
                print('\t%s' % full_file_name)
                try:
                    input_file = open(full_file_name)
                    input_lines = input_file.readlines()
                    input_file.close()
                    output_file = open(full_file_name, 'w')

                    output_lines = add_cpp_include_and_macros(
                        input_lines,'libProfiler.h','PROFILER_START_F','PROFILER_END'
                    )
                    output_file.writelines(output_lines)
                    output_file.close()

                except:
                    print("Error processing: " + full_file_name)

  
    #test_method_one_line()
