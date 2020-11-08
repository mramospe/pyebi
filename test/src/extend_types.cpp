#include "pyebi/core.hpp"

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <iostream>
#include <string>

/// custom dynamic array
template <class T> class custom_array {
public:
  using value_type = T;
  custom_array(std::size_t n) : m_n{n}, m_ptr{new T[n]} {}
  custom_array(custom_array const &other)
      : m_n{other.size()}, m_ptr{new T[m_n]} {
    for (std::size_t i = 0; i < m_n; ++i)
      m_ptr[i] = other[i];
  }
  custom_array &operator=(custom_array const &other) = delete;
  ~custom_array() {
    if (m_ptr)
      delete m_ptr;
  }
  T &operator[](std::size_t i) { return m_ptr[i]; }
  T const &operator[](std::size_t i) const { return m_ptr[i]; }
  std::size_t size() const { return m_n; }

private:
  std::size_t m_n = 0;
  T *m_ptr = nullptr;
};

// register the type conversion
PYEBI_REGISTER_TYPE(
    custom_array<T>,
    [](PyObject *l) {
      auto const s = PyList_Size(l);
      custom_array<T> result(s);
      for (Py_ssize_t i = 0; i < s; ++i)
        result[i] =
            pyebi::types::python_type<T>::c_builder(PyList_GetItem(l, i));
      return result;
    };
    , pyebi::types::create_list<custom_array<T>>, class T);

// function using the previously defined type
custom_array<double>
return_same_custom_array_double(custom_array<double> const &a) {
  return a;
}

// build the interface
PYEBI_INTERFACE(return_same_custom_array_double, "Handle a custom array");

// Methods of the module
static PyMethodDef extend_typesMethods[] = {return_same_custom_array_double_DEF,
                                            {NULL, NULL, 0, NULL}};

// Definition of the module
static struct PyModuleDef extend_types_module = {
    PyModuleDef_HEAD_INIT,
    "extend_types",
    NULL,
    -1,
    extend_typesMethods,
    0,
    0,
    0,
    0,
};

// Initialization function
PyMODINIT_FUNC PyInit_extend_types(void) {

  PyObject *m = PyModule_Create(&extend_types_module);
  if (m == NULL)
    return NULL;

  return m;
}
