//
// Copyright (c) 2016, Benjamin Kaufmann
//
// This file is part of Potassco.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef POTASSCO_PLATFORM_H_INCLUDED
#define POTASSCO_PLATFORM_H_INCLUDED
#include <cstddef>
#include <cassert>
#include <cstdlib>
#include <cerrno>
#if !defined(POTASSCO_HAS_STATIC_ASSERT)
#	if (defined(__cplusplus) && __cplusplus >= 201103L) || (defined(_MSC_VER) && _MSC_VER >= 1600) || (defined(static_assert) && !defined(_LIBCPP_VERSION))
#		define POTASSCO_HAS_STATIC_ASSERT 1
#	else
#		define POTASSCO_HAS_STATIC_ASSERT 0
#	endif
#endif

#define POTASSCO_STRING2(x) #x
#define POTASSCO_STRING(x) POTASSCO_STRING2(x)
#define POTASSCO_CONCAT2(X, Y) X ## Y
#define POTASSCO_CONCAT(X, Y) POTASSCO_CONCAT2(X, Y)

#if defined(_MSC_VER)
#define POTASSCO_ATTR_UNUSED
#define POTASSCO_PRAGMA_TODO(X) __pragma(message(__FILE__ "(" POTASSCO_STRING(__LINE__) ") : TODO: " X))
#define POTASSCO_FUNC_NAME __FUNCTION__
#define POTASSCO_WARNING_BEGIN_RELAXED \
	__pragma(warning(push))\
	__pragma(warning (disable : 4200))

#define POTASSCO_WARNING_END_RELAXED \
	__pragma(warning(pop))

#elif defined(__GNUC__) || defined(__clang__)
#if !defined(__STDC_FORMAT_MACROS)
#define __STDC_FORMAT_MACROS
#endif
#if !defined(__STDC_LIMIT_MACROS)
#define __STDC_LIMIT_MACROS
#endif
#define POTASSCO_ATTR_UNUSED __attribute__((unused))
#define POTASSCO_FUNC_NAME __PRETTY_FUNCTION__
#define POTASSCO_APPLY_PRAGMA(x) _Pragma (#x)
#define POTASSCO_PRAGMA_TODO(x) POTASSCO_APPLY_PRAGMA(message ("TODO: " #x))
#	if defined(__clang__)
#		pragma clang diagnostic push
#		pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#		pragma clang diagnostic ignored "-Wvariadic-macros"
#		define POTASSCO_WARNING_BEGIN_RELAXED \
		_Pragma("clang diagnostic push") \
		_Pragma("clang diagnostic ignored \"-Wzero-length-array\"")
#		define POTASSCO_WARNING_END_RELAXED _Pragma("clang diagnostic pop")
#	else
#		pragma GCC diagnostic push
#		pragma GCC system_header
#		define POTASSCO_WARNING_BEGIN_RELAXED \
		_Pragma("GCC diagnostic push")\
		_Pragma("GCC diagnostic ignored \"-Wpragmas\"")\
		_Pragma("GCC diagnostic ignored \"-Wpedantic\"")\
		_Pragma("GCC diagnostic ignored \"-pedantic\"")
#		define POTASSCO_WARNING_END_RELAXED _Pragma("GCC diagnostic pop")
#endif
#else
#define POTASSCO_ATTR_UNUSED
#define POTASSCO_FUNC_NAME __FILE__
#define POTASSCO_WARNING_BEGIN_RELAXED
#define POTASSCO_WARNING_END_RELAXED
#endif
#include <inttypes.h>

#if !defined(POTASSCO_ENABLE_PRAGMA_TODO) || POTASSCO_ENABLE_PRAGMA_TODO==0
#undef POTASSCO_PRAGMA_TODO
#define POTASSCO_PRAGMA_TODO(X)
#endif

#if POTASSCO_HAS_STATIC_ASSERT == 0
template <bool> struct static_assertion;
template <>     struct static_assertion<true> {};
#undef static_assert
#define static_assert(x, message) typedef bool POTASSCO_CONCAT(potassco_static_assertion,__LINE__)[sizeof(static_assertion< (x) >)] POTASSCO_ATTR_UNUSED
#endif

#if UINTPTR_MAX > UINT64_MAX
#error Unsupported platform!
#endif

namespace Potassco {
struct EnumClass {
	const char *name, *rep;
	int         min, max;
};
enum FailType {
	error_assert   = -1,
	error_logic    = -2,
	error_runtime  = -3
};
extern void fail(int ec, const char* file, unsigned line, const char* exp, const char* fmt, ...);
} // namespace Potassco

//! Macro for defining a set of constants similar to a C++11 strong enum.
#define POTASSCO_ENUM_CONSTANTS_T(TypeName, BaseType, minVal, ...) \
	enum E { __VA_ARGS__, __eEnd, eMin = minVal, eMax = __eEnd - 1 };\
	TypeName(E x = eMin) : val_(x) {}\
	explicit TypeName(BaseType x) : val_(static_cast<E>(x)) {assert(x <= eMax);}\
	operator BaseType() const { return static_cast<BaseType>(val_); } \
	static Potassco::EnumClass enumClass() { Potassco::EnumClass r = {#TypeName, #__VA_ARGS__, eMin, eMax}; return r;}\
	E val_

//! Macro for defining a set of constants starting at 0.
#define POTASSCO_ENUM_CONSTANTS(TypeName, ...) \
	POTASSCO_ENUM_CONSTANTS_T(TypeName, unsigned, 0u, __VA_ARGS__)

//! Executes the given expression and calls Potassco::fail() with the given error code if it evaluates to false.
#define POTASSCO_CHECK(exp, ec, ...) \
	(void)( (!!(exp)) || (Potassco::fail(ec, POTASSCO_FUNC_NAME, unsigned(__LINE__), #exp, ## __VA_ARGS__, 0),std::abort(),0))

//! Shorthand for POTASSCO_CHECK(exp, Potassco::error_logic, args...).
#define POTASSCO_REQUIRE(exp, ...) POTASSCO_CHECK(exp, Potassco::error_logic, ## __VA_ARGS__)
//! Shorthand for POTASSCO_CHECK(exp, Potassco::error_assert, args...).
#define POTASSCO_ASSERT(exp, ...)  POTASSCO_CHECK(exp, Potassco::error_assert, ## __VA_ARGS__)
//! Shorthand for POTASSCO_CHECK(exp, Potassco::error_runtime, args...).
#define POTASSCO_EXPECT(exp, ...)  POTASSCO_CHECK(exp, Potassco::error_runtime, ## __VA_ARGS__)

#if (defined(__cplusplus) && __cplusplus >= 201103L) || defined(_MSC_VER) || defined(_LIBCPP_VERSION)
#define POTASSCO_EXT_INCLUDE(x) <x>
#if !defined(_MSC_VER) || _MSC_VER > 1500
#define POTASSCO_EXT_NS std
#else
#define POTASSCO_EXT_NS std::tr1
#endif
#else
#define POTASSCO_EXT_INCLUDE(x) <tr1/x>
#define POTASSCO_EXT_NS std::tr1
#endif

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
