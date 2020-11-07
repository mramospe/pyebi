#ifndef PYTHON_EASY_BINDINGS_CORE
#define PYTHON_EASY_BINDINGS_CORE

#include "pyebi/types.hpp"
#include "pyebi/utils.hpp"

#include <tuple>

namespace pyebi {

  namespace core {

    /// Store argument values and determine the string used to parse python arguments
    template <class... Args> struct struct_of_args {
      static constexpr const char *chars =
          utils::merge_chars<types::python_type<Args>::cid...>::chars;
      std::tuple<types::python_type_t<Args>...> values;
    };

    /// Input type(s) of a function (without qualifiers)
    template <class... Input, class Output>
    auto function_input_f(Output (*)(Input...)) {
      return struct_of_args<utils::remove_cvref_t<Input>...>{};
    }

    /// Output type of a function
    template <class... Input, class Output>
    auto function_output_f(Output (*)(Input...)) {
      return utils::type_holder<Output>{}; // so we can process "void"
    }

    /// Parse the python arguments (implementation)
    template <size_t... I, class... Args>
    auto parse_arguments_impl(PyObject *args, const char *chars,
                              struct_of_args<Args...> &s,
                              std::index_sequence<I...>) {
      return PyArg_ParseTuple(args, chars, &std::get<I>(s.values)...);
    }

    /// Parse the python arguments
    template <class... Args>
    auto parse_arguments(PyObject *args, const char *chars,
                         struct_of_args<Args...> &s) {
      return parse_arguments_impl(args, chars, s,
                                  std::make_index_sequence<sizeof...(Args)>{});
    }

    /// Call a function (implementation)
    template <class Function, class... Input, size_t... I>
    PyObject *call_function_impl(Function f,
                                 struct_of_args<Input...> const &args,
                                 std::index_sequence<I...>) {
      using output_type =
          types::python_type<typename decltype(function_output_f(f))::type>;
      if constexpr (std::is_same_v<typename output_type::type, void>) {
        f(std::get<I>(args.values)...);
        Py_RETURN_NONE;
      } else
        return output_type::pybuilder(f(std::get<I>(args.values)...));
    }

    /// Call a function
    template <class Function, class... Input>
    PyObject *call_function(Function f, struct_of_args<Input...> const &args) {
      return call_function_impl(f, args,
                                std::make_index_sequence<sizeof...(Input)>{});
    }
  } // namespace core
} // namespace pyebi

#define BUILD_INTERFACE_FULL(FUNC, INTERFACE, DEF, DOC)                        \
  PyObject *INTERFACE(PyObject *Py_UNUSED(self), PyObject *args) {             \
    decltype(pyebi::core::function_input_f(&FUNC)) holder;                     \
    if (!parse_arguments(                                                      \
            args, decltype(pyebi::core::function_input_f(&FUNC))::chars,       \
            holder))                                                           \
      return NULL;                                                             \
    return pyebi::core::call_function(FUNC, holder);                           \
  }                                                                            \
  static PyMethodDef DEF = {#FUNC, INTERFACE, METH_VARARGS, DOC};

#define BUILD_INTERFACE_4_ARGS(FUNC, INTERFACE, DEF, DOC)                      \
  BUILD_INTERFACE_FULL(FUNC, INTERFACE, DEF, DOC)
#define BUILD_INTERFACE_3_ARGS(FUNC, DEF, DOC)                                 \
  BUILD_INTERFACE_FULL(FUNC, FUNC##_INTERFACE, DOC)
#define BUILD_INTERFACE_2_ARGS(FUNC, DOC)                                      \
  BUILD_INTERFACE_FULL(FUNC, FUNC##_INTERFACE, FUNC##_DEF, DOC)
#define BUILD_INTERFACE_1_ARGS(FUNC)                                           \
  BUILD_INTERFACE_FULL(FUNC, FUNC##_INTERFACE, FUNC##_DEF, NULL)

#define GET_5TH_ARG(ARG1, ARG2, ARG3, ARG4, ARG5, ...) ARG5
#define BUILD_INTERFACE_VARGS(...)                                             \
  GET_5TH_ARG(__VA_ARGS__, BUILD_INTERFACE_4_ARGS, BUILD_INTERFACE_3_ARGS,     \
              BUILD_INTERFACE_2_ARGS, BUILD_INTERFACE_1_ARGS, )

// Main macro to build the interface
#define BUILD_INTERFACE(...) BUILD_INTERFACE_VARGS(__VA_ARGS__)(__VA_ARGS__)

#endif
