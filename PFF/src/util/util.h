#pragma once

// =================================================================================  includes  ==================================================================================

#include "util/logger.h"
#include "util/io/io_handler.h"
#include "util/io/config.h"
#include "util/data_types.h"

// =================================================================================  macros  ==================================================================================

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

namespace PFF {

	namespace util {

		template <size_t N>
		struct char_array {
			char data[N];
		};

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

		template <typename T>
		T convert_from_string(const std::string& str) {
			std::istringstream iss(str);
			T result;
			iss >> result;
			return result;
		}

		template <typename VecType>
		std::string vec_to_str(const VecType& vec, std::string_view name = "") {
			std::stringstream ss;
			ss << name << ": [" << std::fixed << std::setprecision(2);

			for (int i = 0; i < VecType::length(); ++i) {
				ss << std::setw(9) << vec[i];
				if (i < VecType::length() - 1) {
					ss << ", ";
				}
			}

			ss << "]";
			return ss.str();
		}

		// from: https://stackoverflow.com/a/57595105
		template <typename T, typename... Rest>
		void hash_combine(std::size_t& seed, const T& v, const Rest&... rest) {
			seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
			(hash_combine(seed, rest), ...);
		}

		template <size_t N, size_t K>
		constexpr auto remove_substring(const char(&source)[N], const char(&remove)[K]) {

			char_array<N> result = {};
			size_t srcIndex = 0;
			size_t dstIndex = 0;
			while (srcIndex < N) {

				size_t matchIndex = 0;
				while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && source[srcIndex + matchIndex] == remove[matchIndex])
					matchIndex++;

				if (matchIndex == K - 1)
					srcIndex += matchIndex;

				result.data[dstIndex++] = source[srcIndex] == '"' ? '\'' : source[srcIndex];
				srcIndex++;
			}
			return result;
		}

		FORCEINLINE constexpr const char* ptr_validity_check(const void* ptr) {return (ptr != nullptr) ? "valid" : "invalid";}

		FORCEINLINE constexpr bool str_to_bool(const std::string& string) { return(string == "true") ? true : false; }
		FORCEINLINE constexpr const char* bool_to_str(bool boolean) { return boolean ? "true" : "false"; }
	}
}
