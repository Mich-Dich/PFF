
#include <util/pffpch.h>
#include <util/util.h>

#include "crash_handler.h"

#if defined(PFF_PLATFORM_WINDOWS)
    #include <windows.h>
#endif

namespace PFF {

#if defined(PFF_PLATFORM_LINUX)

    // need to catch signals related to termination to shutdown gracefully (eg: flash remaining log messages). Was inspired by reckless_log: https://github.com/mattiasflodin/reckless
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

		logger::shutdown();
		detach_crash_handler();
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

    void detach_crash_handler() {
        if (g_vectored_exception_handler) {
            RemoveVectoredExceptionHandler(g_vectored_exception_handler);
            g_vectored_exception_handler = nullptr;
        }
        
        if (g_old_console_handler) {
            SetConsoleCtrlHandler(g_old_console_handler, TRUE);
            g_old_console_handler = nullptr;
        }
    }

    LONG WINAPI exception_handler(PEXCEPTION_POINTERS pExceptionInfo) {
        logger::shutdown();
        detach_crash_handler();
        ExitProcess(1);
        return EXCEPTION_CONTINUE_SEARCH;
    }

    BOOL WINAPI console_handler(DWORD dwCtrlType) {
        logger::shutdown();
        detach_crash_handler();
        ExitProcess(0);
        return TRUE;
    }

    void attach_crash_handler() {
        // Add vectored exception handler for critical exceptions
        g_vectored_exception_handler = AddVectoredExceptionHandler(1, exception_handler);
        if (!g_vectored_exception_handler) {
            throw std::system_error(GetLastError(), std::system_category(), "AddVectoredExceptionHandler failed");
        }

        // Set console control handler for termination signals
        if (!SetConsoleCtrlHandler(console_handler, TRUE)) {
            RemoveVectoredExceptionHandler(g_vectored_exception_handler);
            g_vectored_exception_handler = nullptr;
            throw std::system_error(GetLastError(), std::system_category(), "SetConsoleCtrlHandler failed");
        }
        
        // Store the previous console handler (if any)
        g_old_console_handler = SetConsoleCtrlHandler(nullptr, FALSE);
        if (g_old_console_handler) {
            SetConsoleCtrlHandler(g_old_console_handler, TRUE);
        }
    }

#endif

}
