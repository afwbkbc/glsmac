#pragma once

#include "base/Task.h"

#include "gse/GSE.h"
#include "gse/Context.h"
#include "gse/runner/Runner.h"
#include "gse/program/Program.h"

#include "util/Timer.h"

namespace task {
namespace gseprompt {

CLASS( GSEPrompt, base::Task )

	GSEPrompt( const std::string& syntax );

	void Start() override;
	void Stop() override;
	void Iterate() override;

private:
	bool m_is_running = false;

	const std::string m_syntax;

	const bool m_is_tty;

	gse::GSE m_gse;
	gse::Context m_gse_context = {
		nullptr,
		{}
	};
	const gse::runner::Runner* m_runner;

	util::Timer m_startup_timer;

	fd_set fds;
	int maxfd;
	fd_set rfds;
	struct timeval tv;
	int retval, len;
	char buff[255] = { 0 };

	std::string m_input = "";

	void PrintPrompt();
	void ProcessInput();

	std::vector< const gse::program::Program* > m_programs = {};

};

}
}
