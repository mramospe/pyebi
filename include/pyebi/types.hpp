#ifndef PYTHON_EASY_BINDINGS_TYPES
#define PYTHON_EASY_BINDINGS_TYPES
#include <Python.h>
#include <cstddef>
#include <string>
#include <vector>

namespace pyebi {
  namespace types {

    const char *pystring_as_string(PyObject *str) {
      PyObject *temp_bytes = PyUnicode_AsEncodedString(str, "UTF-8", "strict");
      const char *result = PyBytes_AS_STRING(temp_bytes);
      Py_DECREF(temp_bytes);
      return result;
    }

    /// Base template to define a conversion from C++ to Python types
    template <class T> struct python_type;
    /// Standard C++ strings
    template <> struct python_type<std::string> {
      static char const cid = 's';
      using py_c_type = const char *;
      static constexpr auto const c_builder = pystring_as_string;
      static constexpr auto const py_builder = [](std::string const &s) {
        return PyUnicode_FromString(s.c_str());
      };
    };
    /// Standard C strings
    template <> struct python_type<const char *> {
      static char const cid = 's';
      using py_c_type = const char *;
      static constexpr auto const c_builder = pystring_as_string;
      static constexpr auto const py_builder = PyUnicode_FromString;
    };
    /// Conversion of single precision floating point numbers
    template <> struct python_type<float> {
      static char const cid = 'f';
      using py_c_type = float;
      static constexpr auto const c_builder = PyFloat_AsDouble;
      static constexpr auto const py_builder = PyFloat_FromDouble;
    };
    /// Conversion of double precision floating point numbers
    template <> struct python_type<double> {
      static char const cid = 'd';
      using py_c_type = double;
      static constexpr auto const c_builder = PyFloat_AsDouble;
      static constexpr auto const py_builder = PyFloat_FromDouble;
    };
    /// Conversion of double precision integer numbers
    template <> struct python_type<int> {
      static char const cid = 'i';
      using py_c_type = int;
      static constexpr auto const c_builder = PyLong_AsLong;
      static constexpr auto const py_builder = PyLong_FromLong;
    };
    /// Void values
    template <> struct python_type<void> {
      // cid is not defined as it is meant for input arguments
      using py_c_type = void;
      // c_builder is not defined as a void can not be built
      // py_builder is not defined as a void can not be built
    };

    template <class Container> PyObject *create_list(Container const &c) {
      PyObject *list = PyList_New(c.size());
      for (std::size_t i = 0; i < c.size(); ++i)
        PyList_SetItem(
            list, i,
            python_type<typename Container::value_type>::py_builder(c[i]));
      return list;
    }

    template <class T, class Allocator>
    struct python_type<std::vector<T, Allocator>> {
      static char const cid = 'O';
      using py_c_type = PyObject *;
      static constexpr auto const c_builder = [](PyObject *l) {
        auto const s = PyList_Size(l);
        std::vector<T, Allocator> result;
        result.reserve(s);
        for (Py_ssize_t i = 0; i < s; ++i)
          result.push_back(python_type<T>::c_builder(PyList_GetItem(l, i)));
        return result;
      };
      static constexpr auto const py_builder =
          create_list<std::vector<T, Allocator>>;
    };

    /// Alias to get corresponding python type for a given C++ type
    template <class T> using python_type_t = typename python_type<T>::py_c_type;
  } // namespace types
} // namespace pyebi

#define PYEBI_REGISTER_TYPE(TYPE, C_BUILDER, PY_BUILDER, ...)                  \
  namespace pyebi {                                                            \
    namespace types {                                                          \
      template <__VA_ARGS__> struct python_type<TYPE> {                        \
        static char const cid = 'O';                                           \
        using py_c_type = PyObject *;                                          \
        static constexpr auto const c_builder = C_BUILDER;                     \
        static constexpr auto const py_builder = PY_BUILDER;                   \
      };                                                                       \
    }                                                                          \
  }

#endif
