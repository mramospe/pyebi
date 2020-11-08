#include "pyebi/api.hpp"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <iostream>

void display(std::string const &name) { std::cout << name << std::endl; }

int return_same(int i) { return i; }

int sum_int(int a, int b) { return a + b; }

float sum_float(float a, float b) { return a + b; }

double sum_double(double a, double b) { return a + b; }

PYEBI_INTERFACE(display, "Display the input string");
PYEBI_INTERFACE(return_same, "Return the same input value");
PYEBI_INTERFACE(sum_int, "Sum two integers");
PYEBI_INTERFACE(sum_float, "Sum two single-precision floating point numbers");
PYEBI_INTERFACE(sum_double, "Sum two double-precision floating point numbers");

// Methods of the module
static PyMethodDef SimpleMethods[] = {display_DEF,    return_same_DEF,
                                      sum_int_DEF,    sum_float_DEF,
                                      sum_double_DEF, {NULL, NULL, 0, NULL}};

// Definition of the module
static struct PyModuleDef simplemodule = {
    PyModuleDef_HEAD_INIT, "simple", NULL, -1, SimpleMethods, 0, 0, 0, 0,
};

// Initialization function
PyMODINIT_FUNC PyInit_simple(void) {

  PyObject *m = PyModule_Create(&simplemodule);
  if (m == NULL)
    return NULL;

  return m;
}
