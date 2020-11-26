# Python Easy Bindings

[![Build Status](https://travis-ci.org/mramospe/pyebi.svg?branch=master)](https://travis-ci.org/mramospe/pyebi)
[![Documentation](https://img.shields.io/badge/documentation-link-blue.svg)](https://mramospe.github.io/pyebi)

This header-only `C++` library offers several tools to expose other `C++` functions in python using its C-API.
The package does not aim to provide a way to completely wrap the python C-API and offer a more handy interface, but rather simplifying the export of functions based on its signature.
`PyEBi` currently supports the basic `C++` types, like `int`, `float`, `double`, `const char*` and `std::string`, as well as any `std::vector` instance constructed using the previous types.
Python classes are converted to their corresponding `C++` types, and then the function is called with these arguments.
Instances of the `std::vector` class are converted to python `list` objects and viceversa.

## Installation

You can use the `PyEBi` package directly after downloading.
Simply make sure you include the directory `include` at compile-time.
Alternatively, the package can be installed on a different location using `cmake`, via
```bash
cmake -B <build directory>
cmake --build <build directory> --target install
```

## Getting started

### Exposing a function in Python

The most common use case of this package is exposing a `C++` function in `python`.
The following snippet show how this can be achieved.
```cpp
#include <pyebi/api.hpp>

float sum_numbers(float a, float b) { return a + b; }
float sub_numbers(float a, float b) { return a - b; }
float mul_numbers(float a, float b) { return a * b; }
float div_numbers(float a, float b) { return a / b; }

PYEBI_INTERFACE(sum_numbers);
PYEBI_INTERFACE(sub_numbers, "Subtract two numbers");
PYEBI_INTERFACE(mul_numbers, mul_numbers_definition, "Multiply two numbers");
PYEBI_INTERFACE(div_numbers, div_numbers_interface, div_numbers_definition, "Divide two numbers");

static PyMethodDef extend_typesMethods[] = {sum_numbers_DEF,
       		   			    sub_numbers_DEF,
					    mul_numbers_definition,
                                            div_numbers_definition,
					    {NULL, NULL, 0, NULL}};

static struct PyModuleDef module = { // definition of the module
    PyModuleDef_HEAD_INIT,
    "module",
    NULL,
    -1,
    module_methods,
    0,
    0,
    0,
    0,
};

PyMODINIT_FUNC PyInit_module(void) { // initialize the module

  PyObject *m = PyModule_Create(&module);
  if (m == NULL)
    return NULL;

  return m;
}
```
As you can see in the previous example, we have exposed four functions working with single-precision floating point numbers.
The function `PYEBI_INTERFACE` creates a static `PyMethodDef` constant object for that function, which stores its signature.
In addition, a new interface function is created (which manages the python types and converts them to `C++` types), whose name is set to that of the input function with `_INTERFACE` appended.
In addition the documentation of the string can be provided, as a string.
It is possible to provide from one to four arguments, corresponding to:
1. function
2. function and documentation
3. function, definition and documentation
4. function, definition, interface and documentation

Similarly, it is possible to work with `std::vector` objects, so a function like
```cpp
float sum_list(std::vector<float> const& v) {
   return std::accumulate(v.begin(), v.end(), 0);
}
```
would work.

### Exposing member functions for a `C++` wrapper

This package also offers the possibility to define interfaces for member
functions of certain classes.
The first thing it is needed is to create a `python` class to act as a wrapper
around one (or more) `C++` classes.
```cpp
#include "pyebi/api.hpp"

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <iostream>
#include <string>

struct some_class {
public:
  some_class(int i, std::string s) : m_i{i}, m_s{s} {}
  void set(int i, std::string s) {
    m_i = i;
    m_s = s;
  }
  int const &integer() const { return m_i; }
  std::string const &string() const { return m_s; }

private:
  int m_i;
  std::string m_s;
};

typedef struct {
  PyObject_HEAD some_class *underlying_class;
} py_some_class;

static PyObject *py_some_class_new(PyTypeObject *type, PyObject *args,
                                   PyObject *kwargs) {
  py_some_class *self = (py_some_class *)type->tp_alloc(type, 0);

  if (!self)
    return NULL;

  int i;
  const char *s = "";

  if (!PyArg_ParseTuple(args, "is", &i, &s))
    return NULL;

  self->underlying_class = new some_class(i, s);

  return (PyObject *)self;
}

static void py_some_class_dealloc(PyObject *self) {
  some_class *p = ((py_some_class *)self)->underlying_class;
  if (p)
    delete p;
  Py_TYPE(self)->tp_free(self);
}

static int py_some_class_init(py_some_class *self, PyObject *args,
                              PyObject *kwargs) {
  int i;
  const char *s = "";
  if (!PyArg_ParseTuple(args, "is", &i, &s))
    return -1;
  self->underlying_class->set(i, s);
  return 0;
}

PYEBI_MEMBER_INTERFACE(py_some_class, &py_some_class::underlying_class, integer,
                       &some_class::integer)
PYEBI_MEMBER_INTERFACE(py_some_class, &py_some_class::underlying_class, string,
                       &some_class::string)

static PyMethodDef py_some_class_methods[] = {
    py_some_class_integer_DEF, py_some_class_string_DEF, {NULL, NULL, 0, NULL}};

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
static PyTypeObject py_some_class_type = {
    PyVarObject_HEAD_INIT(NULL, 0) "classes.py_some_class",
    sizeof(py_some_class),
    0,
    py_some_class_dealloc,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    Py_TPFLAGS_DEFAULT,
    "py_some_class object",
    0,
    0,
    0,
    0,
    0,
    0,
    py_some_class_methods,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    (initproc)py_some_class_init,
    0,
    py_some_class_new};
#pragma GCC diagnostic pop

// Methods of the module
static PyMethodDef classesMethods[] = {{NULL, NULL, 0, NULL}};

// Definition of the module
static struct PyModuleDef classes_module = {
    PyModuleDef_HEAD_INIT, "classes", NULL, -1, classesMethods, 0, 0, 0, 0,
};

// Initialization function
PyMODINIT_FUNC PyInit_classes(void) {

  if (PyType_Ready(&py_some_class_type) < 0)
    return NULL;

  PyObject *m = PyModule_Create(&classes_module);
  if (m == NULL)
    return NULL;

  Py_INCREF(&py_some_class_type);
  if (PyModule_AddObject(m, "py_some_class", (PyObject *)&py_some_class_type) <
      0) {
    Py_DECREF(&py_some_class_type);
    Py_DECREF(m);
    return NULL;
  }

  return m;
}
```
In this case, `py_some_class` is a wrapper around the `some_class` class.
We have exposed two functions to access two members of the underlying class
using the `PYEBI_MEMBER_INTERFACE` macro.
This macro can accept up to seven arguments.
The first four are mandatory, and correspond to the python object, the accessor
to the underlying member of the class, the function name and the pointer to the
member function.
Additionally you can provide the name of the interface and the name of the
method definition (set to `<class name>_<function name>_(INTERFACE|DEF)` by
default), as well as the docstring.
Similarly to `PYEBI_INTERFACE`, the arguments must be provided in any of the
following ways:
1. class, accessor, name, function
2. class, accessor, name, function and documentation
3. class, accessor, name, function, definition and documentation
4. class, accessor, name, function, definition, interface and documentation

### Registering a new type

It is possible to register a new type so the `PYEBI_INTERFACE` function can create the interface and the definition object.
This is done through the `PYEBI_REGISTER_TYPE` macro, that takes the type (can be a template), the c-conversion function (from `PyObject*` to your type) and the python-conversion type (from your type to `PyObject*`).
In addition, you can supply template parameters, in the same format of comma-separated list as for the `template<...>` statement.
For example:

```cpp
struct my_int { int i; }

PYEBI_REGISTER_TYPE(
    my_int,
    [] (int i) { return my_int{i}; },
    , [] (my_int const& m) { return PyLong_FromLong(m.i); });

template<class T>
struct my_tpl { T i; }

PYEBI_REGISTER_TYPE(
    my_tpl<T>,
    [] (T i) { return my_tpl<T>{i}; },
    , [] (my_tpl<T> const& m) { ... }
    , tpl T);
```
In the first case, we simply take an integer and assign it to the value stored in `my_int`.
The conversion from `C++` types to python `C` types must be done through the python C-API.
For the second case, we must still define an appropriate conversion from `my_tpl` to `PyObject*`.
Once this is done, we can expose functions of the type
```cpp
my_int return_same(my_int const& i) { return i; }
```

## Important notes

Note that all arguments must be either pointers, `rvalue` or cv-qualified types.
If you wish to modify the value of a python object, you must define the way to converting it from `PyObject*` to your pointer type and viceversa.
