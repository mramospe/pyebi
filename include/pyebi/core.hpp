#ifndef PYTHON_EASY_BINDINGS_CORE
#define PYTHON_EASY_BINDINGS_CORE

#include "pyebi/types.hpp"
#include "pyebi/utils.hpp"

#include <tuple>

namespace pyebi {

  namespace core {

    /// Store argument values and determine the string used to parse python
    /// arguments
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

    /// Input type(s) of a function (without qualifiers)
    template <class Object, class... Input, class Output>
    auto function_input_f(Output (Object::*)(Input...)) {
      return struct_of_args<utils::remove_cvref_t<Input>...>{};
    }

    /// Output type of a function
    template <class... Input, class Output>
    auto function_output_f(Output (*)(Input...)) {
      return utils::type_holder<Output>{}; // so we can process "void"
    }

    /// Output type of a function
    template <class Object, class... Input, class Output>
    auto function_output_f(Output (Object::*)(Input...)) {
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

    template <std::size_t I, class... Args>
    constexpr auto get_element(struct_of_args<Args...> const &sa) {
      using type = types::python_type<utils::type_at_t<I, Args...>>;
      if constexpr (std::is_same_v<
                        utils::remove_cvref_t<typename type::py_c_type>,
                        PyObject *>) {
        return type::c_builder(std::get<I>(sa.values));
      } else
        return std::get<I>(sa.values);
    }

    /// Call a function (implementation)
    template <class Function, class... Args, size_t... I>
    PyObject *call_function_impl(Function f,
                                 struct_of_args<Args...> const &args,
                                 std::index_sequence<I...>) {
      using output_type =
          types::python_type<typename decltype(function_output_f(f))::type>;
      if constexpr (std::is_same_v<typename output_type::py_c_type, void>) {
        f(get_element<I>(args)...);
        Py_RETURN_NONE;
      } else
        return output_type::py_builder(f(get_element<I>(args)...));
    }

    /// Call a function
    template <class Function, class... Args>
    PyObject *call_function(Function f, struct_of_args<Args...> const &args) {
      return call_function_impl(f, args,
                                std::make_index_sequence<sizeof...(Args)>{});
    }

    /// Call a member function (implementation)
    template <class Object, class Function, class... Args, size_t... I>
    PyObject *call_member_function_impl(Object *o, Function f,
                                        struct_of_args<Args...> const &args,
                                        std::index_sequence<I...>) {
      using output_type =
          types::python_type<typename decltype(function_output_f(f))::type>;
      if constexpr (std::is_same_v<typename output_type::py_c_type, void>) {
        (o->*f)(get_element<I>(args)...);
        Py_RETURN_NONE;
      } else
        return output_type::py_builder((o->*f)(get_element<I>(args)...));
    }

    /// Call a member function
    template <class Object, class Function, class... Args>
    PyObject *call_member_function(Object *o, Function f,
                                   struct_of_args<Args...> const &args) {
      return call_member_function_impl(
          o, f, args, std::make_index_sequence<sizeof...(Args)>{});
    }
  } // namespace core
} // namespace pyebi

#define PYEBI_INTERFACE_FULL(FUNC, INTERFACE, DEF, DOC)                        \
  PyObject *INTERFACE(PyObject *Py_UNUSED(self), PyObject *args) {             \
    decltype(pyebi::core::function_input_f(&FUNC)) holder;                     \
    if (!parse_arguments(                                                      \
            args, decltype(pyebi::core::function_input_f(&FUNC))::chars,       \
            holder))                                                           \
      return NULL;                                                             \
    return pyebi::core::call_function(FUNC, holder);                           \
  }                                                                            \
  static PyMethodDef DEF = {#FUNC, INTERFACE, METH_VARARGS, DOC};

#define PYEBI_INTERFACE_4_ARGS(FUNC, INTERFACE, DEF, DOC)                      \
  PYEBI_INTERFACE_FULL(FUNC, INTERFACE, DEF, DOC)
#define PYEBI_INTERFACE_3_ARGS(FUNC, DEF, DOC)                                 \
  PYEBI_INTERFACE_FULL(FUNC, FUNC##_INTERFACE, DOC)
#define PYEBI_INTERFACE_2_ARGS(FUNC, DOC)                                      \
  PYEBI_INTERFACE_FULL(FUNC, FUNC##_INTERFACE, FUNC##_DEF, DOC)
#define PYEBI_INTERFACE_1_ARGS(FUNC)                                           \
  PYEBI_INTERFACE_FULL(FUNC, FUNC##_INTERFACE, FUNC##_DEF, NULL)

#define GET_5TH_ARG(ARG1, ARG2, ARG3, ARG4, ARG5, ...) ARG5
#define PYEBI_INTERFACE_VARGS(...)                                             \
  GET_5TH_ARG(__VA_ARGS__, PYEBI_INTERFACE_4_ARGS, PYEBI_INTERFACE_3_ARGS,     \
              PYEBI_INTERFACE_2_ARGS, PYEBI_INTERFACE_1_ARGS, )

// Main macro to build the interface
#define PYEBI_INTERFACE(...) PYEBI_INTERFACE_VARGS(__VA_ARGS__)(__VA_ARGS__)

#define PYEBI_MEMBER_INTERFACE_FULL(OBJECT, ACCESSOR, NAME, FUNC, INTERFACE,   \
                                    DEF, DOC)                                  \
  PyObject *INTERFACE(PyObject *self, PyObject *args) {                        \
    decltype(pyebi::core::function_input_f(FUNC)) holder;                      \
    if (!parse_arguments(args,                                                 \
                         decltype(pyebi::core::function_input_f(FUNC))::chars, \
                         holder))                                              \
      return NULL;                                                             \
    return pyebi::core::call_member_function(((OBJECT *)self)->*ACCESSOR,      \
                                             FUNC, holder);                    \
  }                                                                            \
  static PyMethodDef DEF = {#NAME, INTERFACE, METH_VARARGS, DOC};

#define PYEBI_MEMBER_INTERFACE_7_ARGS(OBJECT, ACCESSOR, NAME, FUNC, INTERFACE, \
                                      DEF, DOC)                                \
  PYEBI_MEMBER_INTERFACE_FULL(OBJECT, ACCESSOR, NAME, FUNC, INTERFACE, DEF, DOC)
#define PYEBI_MEMBER_INTERFACE_6_ARGS(OBJECT, ACCESSOR, NAME, FUNC, DEF, DOC)  \
  PYEBI_MEMBER_INTERFACE_FULL(OBJECT, ACCESSOR, NAME, FUNC,                    \
                              OBJECT##_##NAME##_INTERFACE, DEF, DOC)
#define PYEBI_MEMBER_INTERFACE_5_ARGS(OBJECT, ACCESSOR, NAME, FUNC, DOC)       \
  PYEBI_MEMBER_INTERFACE_FULL(OBJECT, ACCESSOR, NAME, FUNC, NAME##_INTERFACE,  \
                              OBJECT##_##NAME##_DEF, DOC)
#define PYEBI_MEMBER_INTERFACE_4_ARGS(OBJECT, ACCESSOR, NAME, FUNC)            \
  PYEBI_MEMBER_INTERFACE_FULL(OBJECT, ACCESSOR, NAME, FUNC,                    \
                              OBJECT##_##NAME##_INTERFACE,                     \
                              OBJECT##_##NAME##_DEF, NULL)
#define PYEBI_MEMBER_INTERFACE_3_ARGS(...)                                     \
  static_assert(false, "Must specify at least the object name, the accessor, " \
                       "the function name and the function itself");
#define PYEBI_MEMBER_INTERFACE_2_ARGS(...)                                     \
  static_assert(false, "Must specify at least the object name, the accessor, " \
                       "the function name and the function itself");
#define PYEBI_MEMBER_INTERFACE_1_ARGS(...)                                     \
  static_assert(false, "Must specify at least the object name, the accessor, " \
                       "the function name and the function itself");

#define GET_8TH_ARG(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ...) ARG8
#define PYEBI_MEMBER_INTERFACE_VARGS(...)                                      \
  GET_8TH_ARG(__VA_ARGS__, PYEBI_MEMBER_INTERFACE_7_ARGS,                      \
              PYEBI_MEMBER_INTERFACE_6_ARGS, PYEBI_MEMBER_INTERFACE_5_ARGS,    \
              PYEBI_MEMBER_INTERFACE_4_ARGS, PYEBI_MEMBER_INTERFACE_3_ARGS,    \
              PYEBI_MEMBER_INTERFACE_2_ARGS, PYEBI_MEMBER_INTERFACE_1_ARGS, )

// Main macro to build interfaces for member functions
#define PYEBI_MEMBER_INTERFACE(...)                                            \
  PYEBI_MEMBER_INTERFACE_VARGS(__VA_ARGS__)(__VA_ARGS__)

#endif
