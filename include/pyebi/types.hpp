#ifndef PYTHON_EASY_BINDINGS_TYPES
#define PYTHON_EASY_BINDINGS_TYPES
#include <Python.h>
#include <string>

namespace pyebi {
  namespace types {

    /// Base template to define a conversion from C++ to Python types
    template <class T> struct python_type;
    /// Conversion of strings
    template <> struct python_type<std::string> {
      static char const cid = 's';
      using type = const char *;
      static constexpr auto const pybuilder = PyUnicode_FromString;
    };
    /// Standard C strings
    template <> struct python_type<const char *> {
      static char const cid = 's';
      using type = const char *;
      static constexpr auto const pybuilder = PyUnicode_FromString;
    };
    /// Conversion of single precision floating point numbers
    template <> struct python_type<float> {
      static char const cid = 'f';
      using type = float;
      static constexpr auto const pybuilder = PyFloat_FromDouble;
    };
    /// Conversion of double precision floating point numbers
    template <> struct python_type<double> {
      static char const cid = 'd';
      using type = double;
      static constexpr auto const pybuilder = PyFloat_FromDouble;
    };
    /// Conversion of double precision integer numbers
    template <> struct python_type<int> {
      static char const cid = 'i';
      using type = int;
      static constexpr auto const pybuilder = PyLong_FromLong;
    };
    /// Void values
    template <> struct python_type<void> {
      // cid is not defined as it is meant for input arguments
      using type = void;
      // pybuilder is not defined as a void can not be built
    };
    /// Alias to get corresponding python type for a given C++ type
    template <class T> using python_type_t = typename python_type<T>::type;
  } // namespace types
} // namespace pyebi
#endif
