#if defined( DEBUG ) || defined( FASTDEBUG )

#ifdef __linux__

#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#endif

#include <iostream>

#include "System.h"

namespace util {

// from https://stackoverflow.com/questions/3596781/how-to-detect-if-the-current-process-is-being-run-by-gdb
bool System::AreWeUnderGDB() {
#ifdef __linux__
	int pid = fork();
	int status;
	int res;

	if ( pid == -1 ) {
		perror( "fork" );
		std::cout << "WARNING: gdb check failed" << std::endl;
		return false;
	}

	if ( pid == 0 ) {
		int ppid = getppid();

		/* Child */
		if ( ptrace( PTRACE_ATTACH, ppid, NULL, NULL ) == 0 ) {
			/* Wait for the parent to stop and continue it */
			waitpid( ppid, NULL, 0 );
			ptrace( PTRACE_CONT, NULL, NULL );

			/* Detach */
			ptrace( PTRACE_DETACH, getppid(), NULL, NULL );

			/* We were the tracers, so gdb is not present */
			res = false;
		}
		else {
			/* Trace failed so GDB is present */
			res = true;
		}
		exit( res );
	}
	else {
		waitpid( pid, &status, 0 );
		res = WEXITSTATUS( status )
			? true
			: false;
	}
	return res;

#else
	std::cout << "WARNING: gdb check skipped due to unsupported platform" << std::endl;
	return false;
#endif
}

bool System::IsGDBAvailable() {
#ifdef __linux__
	return ( !system( "which gdb > /dev/null 2>&1" ) );
#else
	std::cout << "WARNING: gdb check skipped due to unsupported platform" << std::endl;
	return false;
}

#endif

}

#endif
