import re
import string
import sys
import glob
import os
import shutil

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
            line.startswith(" ") or line.startswith("\t") or line.startswith("//") or line.endswith(";\n")
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

if __name__ == "__main__":
    rootDir = "."
    include_path = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
    shutil.copy2(os.path.join(include_path,"libProfiler.h"), os.getcwd())

    for dirName, subdirList, fileList in os.walk(rootDir):
        print("Found directory: %s" % dirName)
        shutil.copy2(os.path.join(include_path,"libProfiler.h"), dirName)
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