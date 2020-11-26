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
  int integer() { return m_i; }
  std::string string() { return m_s; }

private:
  int m_i;
  std::string m_s;
};

typedef struct {
  PyObject_HEAD some_class *underlying_class;
} py_some_class;

static PyObject *py_some_class_new(PyTypeObject *type, PyObject *args,
                                   PyObject *Py_UNUSED(kwargs)) {
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
                              PyObject *Py_UNUSED(kwargs)) {
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
