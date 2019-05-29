#pragma once

// Distributed under the 3-Clause BSD License.  See accompanying
// file LICENSE or https://github.com/CLIUtils/CLI11 for details.

#include "StringTools.hpp"
#include <exception>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

// [CLI11:verbatim]
#if defined(CLI11_CPP17)
#if defined(__has_include)
#if __has_include(<string_view>)
#include <string_view>
#define CLI11_HAS_STRING_VIEW
#endif
#endif
#endif
// [CLI11:verbatim]

namespace CLI {

// Type tools

// Utilities for type enabling
namespace detail {
// Based generally on https://rmf.io/cxx11/almost-static-if
/// Simple empty scoped class
enum class enabler {};

/// An instance to use in EnableIf
constexpr enabler dummy = {};
} // namespace detail

/// A copy of enable_if_t from C++14, compatible with C++11.
///
/// We could check to see if C++14 is being used, but it does not hurt to redefine this
/// (even Google does this: https://github.com/google/skia/blob/master/include/private/SkTLogic.h)
/// It is not in the std namespace anyway, so no harm done.
template <bool B, class T = void> using enable_if_t = typename std::enable_if<B, T>::type;

/// A copy of std::void_t from C++17 (helper for C++11 and C++14)
template <typename... Ts> struct make_void { using type = void; };

/// A copy of std::void_t from C++17 - same reasoning as enable_if_t, it does not hurt to redefine
template <typename... Ts> using void_t = typename make_void<Ts...>::type;

/// A copy of std::conditional_t from C++14 - same reasoning as enable_if_t, it does not hurt to redefine
template <bool B, class T, class F> using conditional_t = typename std::conditional<B, T, F>::type;

/// Check to see if something is a vector (fail check by default)
template <typename T> struct is_vector : std::false_type {};

/// Check to see if something is a vector (true if actually a vector)
template <class T, class A> struct is_vector<std::vector<T, A>> : std::true_type {};

/// Check to see if something is bool (fail check by default)
template <typename T> struct is_bool : std::false_type {};

/// Check to see if something is bool (true if actually a bool)
template <> struct is_bool<bool> : std::true_type {};

/// Check to see if something is a shared pointer
template <typename T> struct is_shared_ptr : std::false_type {};

/// Check to see if something is a shared pointer (True if really a shared pointer)
template <typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

/// Check to see if something is a shared pointer (True if really a shared pointer)
template <typename T> struct is_shared_ptr<const std::shared_ptr<T>> : std::true_type {};

/// Check to see if something is copyable pointer
template <typename T> struct is_copyable_ptr {
    static bool const value = is_shared_ptr<T>::value || std::is_pointer<T>::value;
};

/// This can be specialized to override the type deduction for IsMember.
template <typename T> struct IsMemberType { using type = T; };

/// The main custom type needed here is const char * should be a string.
template <> struct IsMemberType<const char *> { using type = std::string; };

#ifdef CLI11_HAS_STRING_VIEW
template <> struct IsMemberType<std::string_view> { using type = std::string; };
#endif

namespace detail {

// These are utilities for IsMember

/// Handy helper to access the element_type generically. This is not part of is_copyable_ptr because it requires that
/// pointer_traits<T> be valid.
template <typename T> struct element_type {
    using type =
        typename std::conditional<is_copyable_ptr<T>::value, typename std::pointer_traits<T>::element_type, T>::type;
};

/// Combination of the element type and value type - remove pointer (including smart pointers) and get the value_type of
/// the container
template <typename T> struct element_value_type { using type = typename element_type<T>::type::value_type; };

/// Adaptor for set-like structure: This just wraps a normal container in a few utilities that do almost nothing.
template <typename T, typename _ = void> struct pair_adaptor : std::false_type {
    using value_type = typename T::value_type;
    using first_type = typename std::remove_const<value_type>::type;
    using second_type = typename std::remove_const<value_type>::type;

    /// Get the first value (really just the underlying value)
    template <typename Q> static auto first(Q &&pair_value) -> decltype(std::forward<Q>(pair_value)) {
        return std::forward<Q>(pair_value);
    }
    /// Get the second value (really just the underlying value)
    template <typename Q> static auto second(Q &&pair_value) -> decltype(std::forward<Q>(pair_value)) {
        return std::forward<Q>(pair_value);
    }
};

/// Adaptor for map-like structure (true version, must have key_type and mapped_type).
/// This wraps a mapped container in a few utilities access it in a general way.
template <typename T>
struct pair_adaptor<
    T,
    conditional_t<false, void_t<typename T::value_type::first_type, typename T::value_type::second_type>, void>>
    : std::true_type {
    using value_type = typename T::value_type;
    using first_type = typename std::remove_const<typename value_type::first_type>::type;
    using second_type = typename std::remove_const<typename value_type::second_type>::type;

    /// Get the first value (really just the underlying value)
    template <typename Q> static auto first(Q &&pair_value) -> decltype(std::get<0>(std::forward<Q>(pair_value))) {
        return std::get<0>(std::forward<Q>(pair_value));
    }
    /// Get the second value (really just the underlying value)
    template <typename Q> static auto second(Q &&pair_value) -> decltype(std::get<1>(std::forward<Q>(pair_value))) {
        return std::get<1>(std::forward<Q>(pair_value));
    }
};

// Warning is suppressed due to "bug" in gcc<5.0 and gcc 7.0 with c++17 enabled that generates a Wnarrowing warning
// in the unevaluated context even if the function that was using this wasn't used.  The standard says narrowing in
// brace initialization shouldn't be allowed but for backwards compatibility gcc allows it in some contexts.  It is a
// little fuzzy what happens in template constructs and I think that was something GCC took a little while to work out.
// But regardless some versions of gcc generate a warning when they shouldn't from the following code so that should be
// suppressed
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
#endif
// check for constructibility from a specific type and copy assignable used in the parse detection
template <typename T, typename C> class is_direct_constructible {
    template <typename TT, typename CC>
    static auto test(int) -> decltype(TT{std::declval<CC>()}, std::is_move_assignable<TT>());

    template <typename, typename> static auto test(...) -> std::false_type;

  public:
    static const bool value = decltype(test<T, C>(0))::value;
};
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

// Check for output streamability
// Based on https://stackoverflow.com/questions/22758291/how-can-i-detect-if-a-type-can-be-streamed-to-an-stdostream

template <typename T, typename S = std::ostringstream> class is_ostreamable {
    template <typename TT, typename SS>
    static auto test(int) -> decltype(std::declval<SS &>() << std::declval<TT>(), std::true_type());

    template <typename, typename> static auto test(...) -> std::false_type;

  public:
    static const bool value = decltype(test<T, S>(0))::value;
};

/// Check for input streamability
template <typename T, typename S = std::istringstream> class is_istreamable {
    template <typename TT, typename SS>
    static auto test(int) -> decltype(std::declval<SS &>() >> std::declval<TT &>(), std::true_type());

    template <typename, typename> static auto test(...) -> std::false_type;

  public:
    static const bool value = decltype(test<T, S>(0))::value;
};

/// Templated operation to get a value from a stream
template <typename T, enable_if_t<is_istreamable<T>::value, detail::enabler> = detail::dummy>
bool from_stream(const std::string &istring, T &obj) {
    std::istringstream is;
    is.str(istring);
    is >> obj;
    return !is.fail() && !is.rdbuf()->in_avail();
}

template <typename T, enable_if_t<!is_istreamable<T>::value, detail::enabler> = detail::dummy>
bool from_stream(const std::string & /*istring*/, T & /*obj*/) {
    return false;
}

/// Convert an object to a string (directly forward if this can become a string)
template <typename T, enable_if_t<std::is_constructible<std::string, T>::value, detail::enabler> = detail::dummy>
auto to_string(T &&value) -> decltype(std::forward<T>(value)) {
    return std::forward<T>(value);
}

/// Convert an object to a string (streaming must be supported for that type)
template <typename T,
          enable_if_t<!std::is_constructible<std::string, T>::value && is_ostreamable<T>::value, detail::enabler> =
              detail::dummy>
std::string to_string(T &&value) {
    std::stringstream stream;
    stream << value;
    return stream.str();
}

/// If conversion is not supported, return an empty string (streaming is not supported for that type)
template <typename T,
          enable_if_t<!std::is_constructible<std::string, T>::value && !is_ostreamable<T>::value, detail::enabler> =
              detail::dummy>
std::string to_string(T &&) {
    return std::string{};
}

/// special template overload
template <typename T1,
          typename T2,
          typename T,
          enable_if_t<std::is_same<T1, T2>::value, detail::enabler> = detail::dummy>
auto checked_to_string(T &&value) -> decltype(to_string(std::forward<T>(value))) {
    return to_string(std::forward<T>(value));
}

/// special template overload
template <typename T1,
          typename T2,
          typename T,
          enable_if_t<!std::is_same<T1, T2>::value, detail::enabler> = detail::dummy>
std::string checked_to_string(T &&) {
    return std::string{};
}

// Type name print

/// Was going to be based on
///  http://stackoverflow.com/questions/1055452/c-get-name-of-type-in-template
/// But this is cleaner and works better in this case

template <typename T,
          enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value, detail::enabler> = detail::dummy>
constexpr const char *type_name() {
    return "INT";
}

template <typename T,
          enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value, detail::enabler> = detail::dummy>
constexpr const char *type_name() {
    return "UINT";
}

template <typename T, enable_if_t<std::is_floating_point<T>::value, detail::enabler> = detail::dummy>
constexpr const char *type_name() {
    return "FLOAT";
}

/// This one should not be used, since vector types print the internal type
template <typename T, enable_if_t<is_vector<T>::value, detail::enabler> = detail::dummy>
constexpr const char *type_name() {
    return "VECTOR";
}
/// Print name for enumeration types
template <typename T, enable_if_t<std::is_enum<T>::value, detail::enabler> = detail::dummy>
constexpr const char *type_name() {
    return "ENUM";
}

/// Print for all other types
template <typename T,
          enable_if_t<!std::is_floating_point<T>::value && !std::is_integral<T>::value && !is_vector<T>::value &&
                          !std::is_enum<T>::value,
                      detail::enabler> = detail::dummy>
constexpr const char *type_name() {
    return "TEXT";
}

// Lexical cast

/// Convert a flag into an integer value  typically binary flags
inline int64_t to_flag_value(std::string val) {
    static const std::string trueString("true");
    static const std::string falseString("false");
    if(val == trueString) {
        return 1;
    }
    if(val == falseString) {
        return -1;
    }
    val = detail::to_lower(val);
    int64_t ret;
    if(val.size() == 1) {
        switch(val[0]) {
        case '0':
        case 'f':
        case 'n':
        case '-':
            ret = -1;
            break;
        case '1':
        case 't':
        case 'y':
        case '+':
            ret = 1;
            break;
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            ret = val[0] - '0';
            break;
        default:
            throw std::invalid_argument("unrecognized character");
        }
        return ret;
    }
    if(val == trueString || val == "on" || val == "yes" || val == "enable") {
        ret = 1;
    } else if(val == falseString || val == "off" || val == "no" || val == "disable") {
        ret = -1;
    } else {
        ret = std::stoll(val);
    }
    return ret;
}

/// Signed integers
template <
    typename T,
    enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value && !is_bool<T>::value && !std::is_enum<T>::value,
                detail::enabler> = detail::dummy>
bool lexical_cast(std::string input, T &output) {
    try {
        size_t n = 0;
        long long output_ll = std::stoll(input, &n, 0);
        output = static_cast<T>(output_ll);
        return n == input.size() && static_cast<long long>(output) == output_ll;
    } catch(const std::invalid_argument &) {
        return false;
    } catch(const std::out_of_range &) {
        return false;
    }
}

/// Unsigned integers
template <typename T,
          enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value && !is_bool<T>::value, detail::enabler> =
              detail::dummy>
bool lexical_cast(std::string input, T &output) {
    if(!input.empty() && input.front() == '-')
        return false; // std::stoull happily converts negative values to junk without any errors.

    try {
        size_t n = 0;
        unsigned long long output_ll = std::stoull(input, &n, 0);
        output = static_cast<T>(output_ll);
        return n == input.size() && static_cast<unsigned long long>(output) == output_ll;
    } catch(const std::invalid_argument &) {
        return false;
    } catch(const std::out_of_range &) {
        return false;
    }
}

/// Boolean values
template <typename T, enable_if_t<is_bool<T>::value, detail::enabler> = detail::dummy>
bool lexical_cast(std::string input, T &output) {
    try {
        auto out = to_flag_value(input);
        output = (out > 0);
        return true;
    } catch(const std::invalid_argument &) {
        return false;
    }
}

/// Floats
template <typename T, enable_if_t<std::is_floating_point<T>::value, detail::enabler> = detail::dummy>
bool lexical_cast(std::string input, T &output) {
    try {
        size_t n = 0;
        output = static_cast<T>(std::stold(input, &n));
        return n == input.size();
    } catch(const std::invalid_argument &) {
        return false;
    } catch(const std::out_of_range &) {
        return false;
    }
}

/// String and similar direct assignment
template <typename T,
          enable_if_t<!std::is_floating_point<T>::value && !std::is_integral<T>::value &&
                          std::is_assignable<T &, std::string>::value,
                      detail::enabler> = detail::dummy>
bool lexical_cast(std::string input, T &output) {
    output = input;
    return true;
}

/// String and similar constructible and copy assignment
template <typename T,
          enable_if_t<!std::is_floating_point<T>::value && !std::is_integral<T>::value &&
                          !std::is_assignable<T &, std::string>::value && std::is_constructible<T, std::string>::value,
                      detail::enabler> = detail::dummy>
bool lexical_cast(std::string input, T &output) {
    output = T(input);
    return true;
}

/// Enumerations
template <typename T, enable_if_t<std::is_enum<T>::value, detail::enabler> = detail::dummy>
bool lexical_cast(std::string input, T &output) {
    typename std::underlying_type<T>::type val;
    bool retval = detail::lexical_cast(input, val);
    if(!retval) {
        return false;
    }
    output = static_cast<T>(val);
    return true;
}

/// Assignable from double or int
template <typename T,
          enable_if_t<!std::is_floating_point<T>::value && !std::is_integral<T>::value &&
                          !std::is_assignable<T &, std::string>::value &&
                          !std::is_constructible<T, std::string>::value && !std::is_enum<T>::value &&
                          is_direct_constructible<T, double>::value && is_direct_constructible<T, int>::value,
                      detail::enabler> = detail::dummy>
bool lexical_cast(std::string input, T &output) {
    int val;
    if(lexical_cast(input, val)) {
        output = T(val);
        return true;
    } else {
        double dval;
        if(lexical_cast(input, dval)) {
            output = T{dval};
            return true;
        }
    }
    return from_stream(input, output);
}

/// Assignable from int64
template <typename T,
          enable_if_t<!std::is_floating_point<T>::value && !std::is_integral<T>::value &&
                          !std::is_assignable<T &, std::string>::value &&
                          !std::is_constructible<T, std::string>::value && !std::is_enum<T>::value &&
                          !is_direct_constructible<T, double>::value && is_direct_constructible<T, int>::value,
                      detail::enabler> = detail::dummy>
bool lexical_cast(std::string input, T &output) {
    int val;
    if(lexical_cast(input, val)) {
        output = T(val);
        return true;
    }
    return from_stream(input, output);
}

/// Assignable from double
template <typename T,
          enable_if_t<!std::is_floating_point<T>::value && !std::is_integral<T>::value &&
                          !std::is_assignable<T &, std::string>::value &&
                          !std::is_constructible<T, std::string>::value && !std::is_enum<T>::value &&
                          is_direct_constructible<T, double>::value && !is_direct_constructible<T, int>::value,
                      detail::enabler> = detail::dummy>
bool lexical_cast(std::string input, T &output) {
    double val;
    if(lexical_cast(input, val)) {
        output = T{val};
        return true;
    }
    return from_stream(input, output);
}

/// Non-string parsable by a stream
template <typename T,
          enable_if_t<!std::is_floating_point<T>::value && !std::is_integral<T>::value &&
                          !std::is_assignable<T &, std::string>::value &&
                          !std::is_constructible<T, std::string>::value && !std::is_enum<T>::value &&
                          !is_direct_constructible<T, double>::value && !is_direct_constructible<T, int>::value,
                      detail::enabler> = detail::dummy>
bool lexical_cast(std::string input, T &output) {
    static_assert(is_istreamable<T>::value,
                  "option object type must have a lexical cast overload or streaming input operator(>>) defined if it "
                  "is convertible from another type use the add_option<T, XC>(...) with XC being the known type");
    return from_stream(input, output);
}

/// Assign a value through lexical cast operations
template <class T, class XC, enable_if_t<std::is_same<T, XC>::value, detail::enabler> = detail::dummy>
bool lexical_assign(const std::string &input, T &output) {
    return lexical_cast(input, output);
}

/// Assign a value converted from a string in lexical cast to the output value directly
template <
    class T,
    class XC,
    enable_if_t<!std::is_same<T, XC>::value && std::is_assignable<T &, XC &>::value, detail::enabler> = detail::dummy>
bool lexical_assign(const std::string &input, T &output) {
    XC val;
    auto parse_result = lexical_cast<XC>(input, val);
    if(parse_result) {
        output = val;
    }
    return parse_result;
}

/// Assign a value from a lexical cast through constructing a value and move assigning it
template <class T,
          class XC,
          enable_if_t<!std::is_same<T, XC>::value && !std::is_assignable<T &, XC &>::value &&
                          std::is_move_assignable<T>::value,
                      detail::enabler> = detail::dummy>
bool lexical_assign(const std::string &input, T &output) {
    XC val;
    bool parse_result = lexical_cast<XC>(input, val);
    if(parse_result) {
        output = T(val); // use () form of constructor to allow some implicit conversions
    }
    return parse_result;
}

/// Sum a vector of flag representations
/// The flag vector produces a series of strings in a vector,  simple true is represented by a "1",  simple false is
/// by
/// "-1" an if numbers are passed by some fashion they are captured as well so the function just checks for the most
/// common true and false strings then uses stoll to convert the rest for summing
template <typename T,
          enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value, detail::enabler> = detail::dummy>
void sum_flag_vector(const std::vector<std::string> &flags, T &output) {
    int64_t count{0};
    for(auto &flag : flags) {
        count += detail::to_flag_value(flag);
    }
    output = (count > 0) ? static_cast<T>(count) : T{0};
}

/// Sum a vector of flag representations
/// The flag vector produces a series of strings in a vector,  simple true is represented by a "1",  simple false is
/// by
/// "-1" an if numbers are passed by some fashion they are captured as well so the function just checks for the most
/// common true and false strings then uses stoll to convert the rest for summing
template <typename T,
          enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value, detail::enabler> = detail::dummy>
void sum_flag_vector(const std::vector<std::string> &flags, T &output) {
    int64_t count{0};
    for(auto &flag : flags) {
        count += detail::to_flag_value(flag);
    }
    output = static_cast<T>(count);
}

} // namespace detail
} // namespace CLI
