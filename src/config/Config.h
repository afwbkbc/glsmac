#pragma once

// TODO: move to config
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_VSYNC true
#define MAXFPS 200

#include <string>

#include "base/Module.h"

using namespace std;

namespace config {

MAJOR_CLASS( Config, base::Module )
	Config( const int argc, const char *argv[] );
	
	const string& GetGamePath() {
		return m_game_path;
	}
	
private:
	const int m_argc;
	const char** const m_argv;
	
	string m_game_path;
};

} /* namespace config */
