#include "GSEPrompt.h"

#include <unistd.h>
#include <iostream>

#include "engine/Engine.h"

namespace task {
namespace gseprompt {

GSEPrompt::GSEPrompt( const std::string& syntax )
	: m_syntax( syntax )
	, m_is_tty( isatty( fileno( stdin ) ) ) {

}

void GSEPrompt::Start() {
	Log( "Starting GSE prompt (syntax: " + m_syntax + ")" );

	m_runner = m_gse.GetRunner();
	m_is_running = true;
	if ( m_is_tty ) {
		std::cout << std::endl;
	}
	PrintPrompt();
}

void GSEPrompt::Stop() {
	Log( "Exiting GSE prompt" );
	for ( const auto& it : m_programs ) {
		delete ( it );
	}
	m_programs.clear();
	delete m_runner;
	m_runner = nullptr;
	m_input.clear();
}

void GSEPrompt::Iterate() {
	if ( m_is_running ) {
		FD_ZERO( &rfds );
		FD_SET( 0, &rfds );
		retval = select( 1, &rfds, NULL, NULL, &tv );
		ASSERT( retval != -1, "select() failed" );
		if ( retval ) {
			if ( !fgets( buff, sizeof( buff ), stdin ) ) {
				m_is_running = false;
				if ( m_is_tty ) {
					std::cout << std::endl << std::endl;
				}
				else {
					ProcessInput();
				}
				g_engine->ShutDown();
				return;
			}
			if ( buff[ 0 ] != '\n' ) {
				m_input += buff;
				if ( m_is_tty ) {
					ProcessInput();
				}
			}
			PrintPrompt();
		}
	}
}

void GSEPrompt::PrintPrompt() {
	if ( m_is_tty ) {
		std::cout << m_syntax << "> " << std::flush;
	}
}

void GSEPrompt::ProcessInput() {
	std::string source = m_input + ";"; // hack

	auto* parser = m_gse.GetParser( "input." + m_syntax, source );

	const gse::program::Program* program = nullptr;
	try {
		program = parser->Parse();
		const auto result = m_runner->Execute( &m_gse_context, program );
		std::cout << result.ToString() << std::endl;
	}
	catch ( std::runtime_error& e ) {
		std::cout << "ERROR: " << e.what() << std::endl;
	}
	if ( m_is_tty ) {
		std::cout << std::endl;
	}

	DELETE( parser );
	if ( program ) {
		// can't delete here because program may have contained some functions that are still bound to context
		// TODO: think how to deal with it
		//DELETE( program );
		m_programs.push_back( program );
	}

	m_input.clear();
}

}
}
