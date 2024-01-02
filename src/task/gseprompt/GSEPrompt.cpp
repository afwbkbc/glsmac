#include "GSEPrompt.h"

#include <unistd.h>
#include <iostream>

#include "engine/Engine.h"

namespace task {
namespace gseprompt {

GSEPrompt::GSEPrompt( const std::string& syntax )
	: m_syntax( syntax ) {

}

void GSEPrompt::Start() {
	Log( "Starting GSE prompt (syntax: " + m_syntax + ")" );

	m_startup_timer.SetTimeout( 100 ); // wait for other things to initialize
}

void GSEPrompt::Stop() {
	Log( "Exiting GSE prompt" );
	for ( const auto& it : m_programs ) {
		delete ( it );
	}
}

void GSEPrompt::Iterate() {
	if ( m_startup_timer.HasTicked() ) {
		PrintPrompt();
	}
	if ( !m_startup_timer.IsRunning() ) {
		FD_ZERO( &rfds );
		FD_SET( 0, &rfds );
		retval = select( 1, &rfds, NULL, NULL, &tv );
		ASSERT( retval != -1, "select() failed" );
		if ( retval ) {
			if ( !fgets( buff, sizeof( buff ), stdin ) ) {
				g_engine->ShutDown();
				return;
			}
			if ( buff[ 0 ] != '\n' ) {
				ProcessInput( buff );
			}
			PrintPrompt();
		}
	}
}

void GSEPrompt::PrintPrompt() {
	std::cout << std::endl << m_syntax << "> " << std::flush;
}

void GSEPrompt::ProcessInput( const char* input ) {
	std::string source = std::string( input ) + ";"; // hack

	auto* parser = m_gse.GetParser( "input." + m_syntax, source );
	auto* runner = m_gse.GetRunner();

	const gse::program::Program* program = nullptr;
	try {
		program = parser->Parse();
		const auto result = runner->Execute( &m_gse_context, program );
		std::cout << result.ToString() << std::endl;
	}
	catch ( std::runtime_error& e ) {
		std::cout << "ERROR: " << e.what() << std::endl;
	}

	DELETE( parser );
	DELETE( runner );
	if ( program ) {

		// can't delete here because program may have contained some functions that are still bound to context
		// TODO: think how to deal with it
		//DELETE( program );
		m_programs.push_back( program );
	}

}

}
}
