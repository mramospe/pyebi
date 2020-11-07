#ifndef PYTHON_EASY_BINDINGS_UTILS
#define PYTHON_EASY_BINDINGS_UTILS
#include <utility>

namespace pyebi {
  namespace utils {

    /// Hold a type
    template <class T> struct type_holder { using type = T; };

    /// Merge the corresponding characters
    template <char... C> struct merge_chars {
      static constexpr const char chars[sizeof...(C) + 1] = {C..., '\0'};
    };

    /// Remove "const" and reference qualifiers
    template <class T> struct remove_cvref {
      // (use std::remove_cvref in C++20)
      typedef std::remove_cv_t<std::remove_reference_t<T>> type;
    };

    template <class T> using remove_cvref_t = typename remove_cvref<T>::type;
  } // namespace utils
} // namespace pyebi
#endif
