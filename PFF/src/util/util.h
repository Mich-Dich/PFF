#pragma once

// =================================================================================  includes  ==================================================================================

#include "util/logger.h"
#include "util/io/io_handler.h"
#include "util/io/config.h"
#include "util/data_types.h"

// =================================================================================  macros  ==================================================================================

#define CONSTEXPR				constexpr

#define ARRAY_SIZE(array)		(sizeof(array) / sizeof(array[0]))

#define APP_NAMESPACE PFF

#define BIT(x) (1 << x)

#define BIND_FN(x)							std::bind(&x, this, std::placeholders::_1)

#define DELETE_COPY(type)					type(const type&) = delete;								\
											type& operator=(const type&) = delete

#define DELETE_MOVE(type)					type(type&&) = delete;									\
											type& operator=(type&&) = delete

#define DELETE_COPY_MOVE(type)				type(const type&) = delete;								\
											type& operator=(const type&) = delete;					\
											type(type&&) = delete;									\
											type& operator=(type&&) = delete


// ================================================================================  functions  ================================================================================

template <typename T>
T str_to_num(const std::string& str) {
	std::istringstream ss(str);
	T num{};
	ss >> num;
	return num;
}

template <typename T>
std::string num_to_str(const T& num) {
	std::ostringstream oss;
	oss << num;
	return oss.str();
}

template <typename VecType>
std::string vec_to_str(const VecType& vec, std::string_view name = "") {
	std::stringstream ss;
	ss << name <<": [" << std::fixed << std::setprecision(2);

	for (int i = 0; i < VecType::length(); ++i) {
		ss << std::setw(9) << vec[i];
		if (i < VecType::length() - 1) {
			ss << ", ";
		}
	}

	ss << "]";
	return ss.str();
}

FORCEINLINE bool str_to_bool(const std::string& string) { return(string == "true") ? true : false; }
FORCEINLINE const char* bool_to_str(bool boolean) { return boolean ? "true" : "false"; }
