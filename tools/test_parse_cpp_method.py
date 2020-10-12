import re
import string
import sys
import glob
import os
import shutil

from add_libprofiler_rec import add_cpp_include, process_code


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

def test_namespace():
    cpp_code_in = """\
static const int = J::K();
namespace {
A::B(int foo)
{
}"""

    cpp_code_out = """\
#include "MyInclude.h"
static const int = J::K();
namespace {
A::B(int foo)
{
MACRO_INITIAL();
}
"""
    assert process_code(cpp_code_in) == cpp_code_out



    # test_method_one_line()
