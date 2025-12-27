#include "Stdout.h"

#include <cstdio>
#include <cstdlib>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
#include <cxxabi.h>

namespace error_handler {

namespace {
	// Signal handler for crashes
	void CrashHandler(int sig) {
		const char* sig_name = "UNKNOWN";
		switch (sig) {
			case SIGSEGV: sig_name = "SIGSEGV"; break;
			case SIGABRT: sig_name = "SIGABRT"; break;
			case SIGBUS: sig_name = "SIGBUS"; break;
			case SIGILL: sig_name = "SIGILL"; break;
			case SIGFPE: sig_name = "SIGFPE"; break;
		}
		
		fprintf(stderr, "\n=== CRASH DETECTED: %s ===\n", sig_name);
		fprintf(stderr, "Stack trace:\n");
		
		void* array[50];
		size_t size = backtrace(array, 50);
		char** symbols = backtrace_symbols(array, size);
		
		if (symbols != nullptr) {
			for (size_t i = 0; i < size; i++) {
				fprintf(stderr, "[%zu] %s\n", i, symbols[i]);
			}
			free(symbols);
		}
		
		fprintf(stderr, "=== End of stack trace ===\n\n");
		
		// Restore default handler and re-raise signal
		signal(sig, SIG_DFL);
		raise(sig);
	}
	
	// Install crash handlers
	struct CrashHandlerInstaller {
		CrashHandlerInstaller() {
			signal(SIGSEGV, CrashHandler);
			signal(SIGABRT, CrashHandler);
			signal(SIGBUS, CrashHandler);
			signal(SIGILL, CrashHandler);
			signal(SIGFPE, CrashHandler);
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
