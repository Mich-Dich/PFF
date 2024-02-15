#pragma once

#include "util/core_config.h"

#include "util/data_types.h"
#include "util/logger.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <string>
#include <thread>
#include <mutex>
#include <sstream>


namespace PFF {

	using float_microseconds = std::chrono::duration<double, std::micro>;

	struct profile_result {

		std::string name;
		float_microseconds start;
		std::chrono::microseconds elapsed_time;
		std::thread::id thread_ID;
	};

	struct instumentation_session {

		std::string name;
	};

	// ==================================================================== instrumentor ====================================================================

	class instrumentor {
	public:

		DELETE_COPY(instrumentor);

		void begin_session(const std::string& name, const std::string& directory, const std::string& filename = "result.json") {
			
			std::lock_guard lock(m_mutex);

			if (m_current_session) {

				// If there is already a current session, then close it before beginning new one.
				// Subsequent profiling output meant for the original session will end up in the newly opened session instead.  That's better than having badly formatted profiling output
				LOG(Error, "Instrumentor::BeginSession(" << name << ") when session [" << m_current_session->name << "] already open");
				InternalEndSession();
			}

			// InstrumentorUtils::create_directory(directory);

			if (!std::filesystem::exists(directory)) {
				if (!std::filesystem::create_directory(directory)) {

					LOG(Error, "Failed to create folder");
				}
			}

			std::string total_filepath = directory + "/" + filename;

			m_output_stream.open(total_filepath);
			if (m_output_stream.is_open()) {

				m_current_session = new instumentation_session{ name };
				write_header();
			} else
				LOG(Error, "Instrumentor could not open file: " << filename);
		}

		void end_session() {

			std::lock_guard lock(m_mutex);
			InternalEndSession();
		}

		void write_profile(const profile_result& result) {

			std::stringstream json;

			json << std::setprecision(3) << std::fixed;
			json << ",{";
			json << "\"cat\":\"function\",";
			json << "\"dur\":" << (result.elapsed_time.count()) << ',';
			json << "\"name\":\"" << result.name << "\",";
			json << "\"ph\":\"X\",";
			json << "\"pid\":0,";
			json << "\"tid\":" << result.thread_ID << ",";
			json << "\"ts\":" << result.start.count();
			json << "}";

			std::lock_guard lock(m_mutex);

			if (m_current_session) {

				m_output_stream << json.str();
				m_output_stream.flush();
				//LOG(Error, "profile function: " << result.name);
			}
		}

		static instrumentor& get() {

			static instrumentor instance;
			return instance;
		}

	private:
		instrumentor()
			: m_current_session(nullptr) {}

		~instrumentor() {
			
			end_session();
		};

		void write_header() {

			m_output_stream << "{\"otherData\": {},\"traceEvents\":[{}";
			m_output_stream.flush();
		}

		void write_footer() {

			m_output_stream << "]}";
			m_output_stream.flush();
		}


		// NOTE: you must already own lock on m_Mutex before calling InternalEndSession()
		void InternalEndSession() {
			if (m_current_session) {

				write_footer();
				m_output_stream.close();
				delete m_current_session;
				m_current_session = nullptr;
			}
		}

	private:
		std::mutex m_mutex;
		instumentation_session* m_current_session;
		std::ofstream m_output_stream;

	};

	// ==================================================================== instrumentor_timer ====================================================================

	class instrumentor_timer {
	public:
		instrumentor_timer(const char* name)
			: m_name(name), m_stopped(false) {

			m_start_timepoint = std::chrono::steady_clock::now();
		}

		~instrumentor_timer() {

			if (!m_stopped)
				stop();
		}

		void stop() {

			auto end_timepoint = std::chrono::steady_clock::now();
			auto high_res_start = float_microseconds{ m_start_timepoint.time_since_epoch() };
			auto elapsed_time = std::chrono::time_point_cast<std::chrono::microseconds>(end_timepoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(m_start_timepoint).time_since_epoch();

			instrumentor::get().write_profile({ m_name, high_res_start, elapsed_time, std::this_thread::get_id() });
			m_stopped = true;
		}

	private:

		const char* m_name;
		bool m_stopped;
		std::chrono::time_point<std::chrono::steady_clock> m_start_timepoint;

	};

}


#if PFF_PROFILE

	// Resolve which function signature macro will be used
	// NOTE: this is only resolved when the (pre)compiler starts, so the syntax highlighting could mark the wrong one in editor!
	#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
		#define PFF_FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__DMC__) && (__DMC__ >= 0x810)
		#define PFF_FUNC_SIG __PRETTY_FUNCTION__
	#elif (defined(__FUNCSIG__) || (_MSC_VER))
		#define PFF_FUNC_SIG __FUNCSIG__
	#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
		#define PFF_FUNC_SIG __FUNCTION__
	#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
		#define PFF_FUNC_SIG __FUNC__
	#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
		#define PFF_FUNC_SIG __func__
	#elif defined(__cplusplus) && (__cplusplus >= 201103)
		#define PFF_FUNC_SIG __func__
	#else
		#define PFF_FUNC_SIG "PFF_FUNC_SIG unknown!"
	#endif

	// double abstraction to take line-number into name string
	#define PFF_PROFILE_SCOPE_LINE2(name, line)						constexpr auto fixedName##line = ::PFF::util::remove_substring(name, "__cdecl ");		\
																	PFF::instrumentor_timer benchmark_timer##line(fixedName##line.data)
	#define PFF_PROFILE_SCOPE_LINE(name, line)						PFF_PROFILE_SCOPE_LINE2(name, line)

	#define PFF_PROFILE_BEGIN_SESSION(name, directory, filename)	PFF::instrumentor::get().begin_session(name, directory, filename)
	#define PFF_PROFILE_END_SESSION()								PFF::instrumentor::get().end_session()
	#define PFF_PROFILE_SCOPE(name)									PFF_PROFILE_SCOPE_LINE(name, __LINE__)
	#define PFF_PROFILE_FUNCTION()									PFF_PROFILE_SCOPE(PFF_FUNC_SIG)
#else
	#define PFF_PROFILE_BEGIN_SESSION(name, directory, filename)
	#define PFF_PROFILE_END_SESSION()
	#define PFF_PROFILE_SCOPE(name)
	#define PFF_PROFILE_FUNCTION()
#endif	// PFF_PROFILE
