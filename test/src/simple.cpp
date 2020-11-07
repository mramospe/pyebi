#include "pyebi/core.hpp"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <iostream>

void display(std::string const &name) { std::cout << name << std::endl; }

BUILD_INTERFACE(display, "Display the input string");

// Methods of the module
static PyMethodDef SimpleMethods[] = {display_DEF, {NULL, NULL, 0, NULL}};

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
