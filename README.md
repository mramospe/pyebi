Python Easy Bindings
====================

[![Build Status](https://travis-ci.org/mramospe/pyebi.svg?branch=master)](https://travis-ci.org/mramospe/pyebi)
[![Documentation](https://img.shields.io/badge/documentation-link-blue.svg)](https://mramospe.github.io/pyebi)

This header-only `C++` library offers several tools to expose other `C++` functions in python using its C-API.
The package does not aim to provide a way to completely wrap the python C-API and offer a more handy interface, but rather simplifying the export of functions based on its signature.
`PyEBi` currently supports the basic `C++` types, like `int`, `float`, `double`, `const char*` and `std::string`, as well as any `std::vector` instance constructed using the previous types.
Python classes are converted to their corresponding `C++` types, and then the function is called with these arguments.
Instances of the `std::vector` class are converted to python `list` objects and viceversa.

Installation
------------

You can use the `PyEBi` package directly after downloading.
Simply make sure you include the directory `include` at compile-time.
Alternatively, the package can be installed on a different location using `cmake`, via
```bash
cmake -B <build directory>
cmake --build <build directory> --target install
```

Getting started
---------------

The process to export a function is the following:
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
1 function
2 function and documentation
3 function, definition and documentation
4 function, definition, interface and documentation

Similarly, it is possible to work with `std::vector` objects, so a function like
```cpp
float sum_list(std::vector<float> const& v) {
   return std::accumulate(v.begin(), v.end(), 0);
}
```
would work.

Registering a new type
----------------------

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

Important notes
---------------

Note that all arguments must be either pointers, `rvalue` or cv-qualified types.
If you wish to modify the value of a python object, you must define the way to converting it from `PyObject*` to your pointer type and viceversa.
