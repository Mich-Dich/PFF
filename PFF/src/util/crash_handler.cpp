
#include <util/pffpch.h>
#include <util/util.h>

#include "crash_handler.h"

#if defined(PFF_PLATFORM_WINDOWS)
    #include <windows.h>
#endif

namespace PFF {

    // need to catch signals related to termination to shutdown gracefully (eg: flash remaining log messages). Was inspired by reckless_log: https://github.com/mattiasflodin/reckless

	/* Copyright 2015 - 2020 Mattias Flodin <git@codepentry.com>
	 *
	 * Permission is hereby granted, free of charge, to any person obtaining a copy
	 * of this software and associated documentation files(the "Software"), to deal
	 * in the Software without restriction, including without limitation the rights
	 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
	 * copies of the Software, and to permit persons to whom the Software is
	 * furnished to do so, subject to the following conditions :
	 *
	 * The above copyright notice and this permission notice shall be included in all
	 * copies or substantial portions of the Software.
	 *
	 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
	 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	 * SOFTWARE.
	 */

#if defined(PFF_PLATFORM_LINUX)

	const std::initializer_list<int> signals = {
		SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGABRT, SIGFPE, SIGKILL, SIGSEGV, SIGPIPE, SIGALRM, SIGTERM, SIGUSR1, SIGUSR2,    // POSIX.1-1990 signals
		SIGBUS, SIGPOLL, SIGPROF, SIGSYS, SIGTRAP, SIGVTALRM, SIGXCPU, SIGXFSZ,                                             // SUSv2 + POSIX.1-2001 signals
		SIGIOT, SIGSTKFLT, SIGIO, SIGPWR,                                                                                   // Various other signals
	};
	std::vector<std::pair<int, struct sigaction>> g_old_sigactions;

	void detach_crash_handler() {

		while(!g_old_sigactions.empty()) {
			auto const& p = g_old_sigactions.back();
			auto signal = p.first;
			auto const& oldact = p.second;
			if(0 != sigaction(signal, &oldact, nullptr))
				throw std::system_error(errno, std::system_category());
			g_old_sigactions.pop_back();
		}
	}

	void signal_handler(const int signal) {

		std::cout << "signal caught => terminating" << std::endl;
		LOG(Fatal, "crash_hander caught signal [" << signal << "]. flushing remaining logs")
		logger::shutdown();
		// detach_crash_handler();
	}

	void attach_crash_handler() {

		struct sigaction act;
		std::memset(&act, 0, sizeof(act));
		act.sa_handler = &signal_handler;
		sigfillset(&act.sa_mask);
		act.sa_flags = SA_RESETHAND;

		// Some signals are synonyms for each other. Some are explictly specified
		// as such, but others may just be implemented that way on specific
		// systems. So we'll remove duplicate entries here before we loop through
		// all the signal numbers.
		std::vector<int> unique_signals(signals);
		sort(begin(unique_signals), end(unique_signals));
		unique_signals.erase(unique(begin(unique_signals), end(unique_signals)), end(unique_signals));
		try {
			g_old_sigactions.reserve(unique_signals.size());
			for(auto signal : unique_signals) {
				struct sigaction oldact;
				if(0 != sigaction(signal, nullptr, &oldact))
					throw std::system_error(errno, std::system_category());
				if(oldact.sa_handler == SIG_DFL) {
					if(0 != sigaction(signal, &act, nullptr)) {
						if(errno == EINVAL)             // If we get EINVAL then we assume that the kernel does not know about this particular signal number.
							continue;

						throw std::system_error(errno, std::system_category());
					}
					g_old_sigactions.push_back({signal, oldact});
				}
			}
		} catch(...) {
			detach_crash_handler();
			throw;
		}
	}

#elif defined(PFF_PLATFORM_WINDOWS)

	LPTOP_LEVEL_EXCEPTION_FILTER old_exception_filter = nullptr;

	LONG WINAPI exception_filter(_EXCEPTION_POINTERS* ExceptionInfo) {

		logger::shutdown();

		// Save the old filter and detach the crash handler
		LPTOP_LEVEL_EXCEPTION_FILTER old_filter = old_exception_filter;
		detach_crash_handler();

		if (old_filter)
			return old_filter(ExceptionInfo);
		else
			return EXCEPTION_CONTINUE_SEARCH;
	}

	void attach_crash_handler() {

		assert(old_exception_filter == nullptr);
		old_exception_filter = SetUnhandledExceptionFilter(&exception_filter);
	}

	void detach_crash_handler() {

		if (old_exception_filter) {
			SetUnhandledExceptionFilter(old_exception_filter);
			old_exception_filter = nullptr;
		}
	}

#endif

}
