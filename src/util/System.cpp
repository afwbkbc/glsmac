#include "System.h"

#ifdef DEBUG
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <iostream>
#endif

using namespace std;

namespace util {

#ifdef DEBUG

// from https://stackoverflow.com/questions/3596781/how-to-detect-if-the-current-process-is-being-run-by-gdb
bool System::AreWeUnderGDB() {
#ifdef __linux__
	int pid = fork();
	int status;
	int res;

	if (pid == -1)
	{
		perror("fork");
		cout << "WARNING: gdb check failed" << endl;
		return false;
	}

	if (pid == 0)
	{
		int ppid = getppid();

		/* Child */
		if (ptrace(PTRACE_ATTACH, ppid, NULL, NULL) == 0)
		{
			/* Wait for the parent to stop and continue it */
			waitpid(ppid, NULL, 0);
			ptrace(PTRACE_CONT, NULL, NULL);

			/* Detach */
			ptrace(PTRACE_DETACH, getppid(), NULL, NULL);

			/* We were the tracers, so gdb is not present */
			res = false;
		}
		else
		{
			/* Trace failed so GDB is present */
			res = true;
		}
		exit(res);
	}
	else
	{
		waitpid(pid, &status, 0);
		res = WEXITSTATUS(status) ? true : false;
	}
	return res;

#else
	cout << "WARNING: gdb check skipped due to unsupported platform" << endl;
#endif
}

bool System::IsGDBAvailable() {
#ifdef __linux__
	return ( !system("which gdb > /dev/null 2>&1") );
#else
	cout << "WARNING: gdb check skipped due to unsupported platform" << endl;
#endif
}

#endif

}
