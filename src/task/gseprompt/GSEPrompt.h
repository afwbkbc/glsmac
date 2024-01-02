#pragma once

#include "base/Task.h"

#include "gse/GSE.h"
#include "gse/Context.h"
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
	const std::string m_syntax;

	gse::GSE m_gse;
	gse::Context m_gse_context = {
		nullptr,
		{}
	};

	util::Timer m_startup_timer;

	fd_set fds;
	int maxfd;
	fd_set rfds;
	struct timeval tv;
	int retval, len;
	char buff[255] = { 0 };

	void PrintPrompt();
	void ProcessInput( const char* input );

	std::vector< const gse::program::Program* > m_programs = {};

};

}
}
