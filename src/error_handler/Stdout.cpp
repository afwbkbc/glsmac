#include "Stdout.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
#include <cxxabi.h>
#include <fcntl.h>
#include <sys/mman.h>

namespace error_handler {

namespace {
	// Compile-time string length helper (safe in signal handler)
	#define STRLEN_LITERAL(s) (sizeof(s) - 1)
	
	// Alternate signal stack for crash handler (needed for SA_ONSTACK)
	static char g_sigstack[SIGSTKSZ];
	
	// Signal handler for crashes - must be async-signal-safe
	void CrashHandler(int sig) {
		// Minimal async-signal-safe operations only
		// Use compile-time string length calculations
		const char msg1[] = "\n=== CRASH DETECTED: ";
		write(STDERR_FILENO, msg1, STRLEN_LITERAL(msg1));
		
		// Write signal name based on signal number
		const char* sig_name = "UNKNOWN";
		size_t sig_name_len = 7;
		switch (sig) {
			case SIGSEGV: sig_name = "SIGSEGV"; sig_name_len = 7; break;
			case SIGABRT: sig_name = "SIGABRT"; sig_name_len = 7; break;
			case SIGBUS: sig_name = "SIGBUS"; sig_name_len = 6; break;
			case SIGILL: sig_name = "SIGILL"; sig_name_len = 6; break;
			case SIGFPE: sig_name = "SIGFPE"; sig_name_len = 6; break;
		}
		write(STDERR_FILENO, sig_name, sig_name_len);
		
		const char msg2[] = " ===\nStack trace:\n";
		write(STDERR_FILENO, msg2, STRLEN_LITERAL(msg2));
		
		// Get backtrace - try multiple times with different sizes
		void* array[100];
		size_t size = 0;
		
		// Try with larger buffer first
		size = backtrace(array, 100);
		if (size == 0) {
			// Try with smaller buffer
			size = backtrace(array, 50);
		}
		if (size == 0) {
			// Try with minimal buffer
			size = backtrace(array, 10);
		}
		
		if (size == 0) {
			const char msg3[] = "ERROR: backtrace() returned 0 frames (stack may be corrupted)\n";
			write(STDERR_FILENO, msg3, STRLEN_LITERAL(msg3));
			const char msg4[] = "Note: Use 'lldb ./build/bin/GLSMAC' and 'bt' command for full stack trace\n";
			write(STDERR_FILENO, msg4, STRLEN_LITERAL(msg4));
		} else {
			// Use backtrace_symbols_fd() which writes directly to fd
			// This is the most async-signal-safe way to get symbols
			backtrace_symbols_fd(array, size, STDERR_FILENO);
		}
		
		const char msg4[] = "=== End of stack trace ===\n\n";
		write(STDERR_FILENO, msg4, STRLEN_LITERAL(msg4));
		
		// Flush stderr to ensure output is visible
		fsync(STDERR_FILENO);
		
		// Restore default handler and re-raise signal
		signal(sig, SIG_DFL);
		raise(sig);
	}
	
	// Install crash handlers
	struct CrashHandlerInstaller {
		CrashHandlerInstaller() {
			// Allocate alternate signal stack
			stack_t ss;
			ss.ss_sp = g_sigstack;
			ss.ss_size = sizeof(g_sigstack);
			ss.ss_flags = 0;
			if (sigaltstack(&ss, nullptr) != 0) {
				fprintf(stderr, "[CrashHandler] WARNING: Failed to set alternate signal stack\n");
			}
			
			// Use sigaction instead of signal() for better reliability
			struct sigaction sa;
			sa.sa_handler = CrashHandler;
			sigemptyset(&sa.sa_mask);
			sa.sa_flags = SA_ONSTACK | SA_RESETHAND; // Use alternate stack, reset handler after first call
			
			sigaction(SIGSEGV, &sa, nullptr);
			sigaction(SIGABRT, &sa, nullptr);
			sigaction(SIGBUS, &sa, nullptr);
			sigaction(SIGILL, &sa, nullptr);
			sigaction(SIGFPE, &sa, nullptr);
			
			// Log installation (this is safe here, not in signal handler)
			fprintf(stderr, "[CrashHandler] Installed signal handlers (SIGSEGV, SIGABRT, SIGBUS, SIGILL, SIGFPE)\n");
			fflush(stderr);
		}
	};
	
	// Global instance installs handlers at startup
	static CrashHandlerInstaller g_crash_handler_installer;
}

void Stdout::HandleError( const std::runtime_error& e ) const {
	printf( "FATAL ERROR: %s\n", e.what() );
	//exit( EXIT_FAILURE );
	throw e;
}

}
