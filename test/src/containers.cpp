#include "pyebi/api.hpp"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <vector>

std::vector<int> return_same_vector_int(std::vector<int> const &v) { return v; }

std::vector<float> return_same_vector_float(std::vector<float> const &v) {
  return v;
}

PYEBI_INTERFACE(return_same_vector_int, "Return the input list of values");
PYEBI_INTERFACE(return_same_vector_float, "Return the input list of values");

// Methods of the module
static PyMethodDef ContainersMethods[] = {return_same_vector_int_DEF,
                                          return_same_vector_float_DEF,
                                          {NULL, NULL, 0, NULL}};

// Definition of the module
static struct PyModuleDef containersmodule = {
    PyModuleDef_HEAD_INIT,
    "containers",
    NULL,
    -1,
    ContainersMethods,
    0,
    0,
    0,
    0,
};

// Initialization function
PyMODINIT_FUNC PyInit_containers(void) {

  PyObject *m = PyModule_Create(&containersmodule);
  if (m == NULL)
    return NULL;

  return m;
}
