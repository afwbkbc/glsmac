#include "GSEPrompt.h"

#include <unistd.h>
#include <iostream>

#include "gse/Exception.h"
#include "engine/Engine.h"
#include "util/String.h"

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
			if ( !m_is_tty || buff[ 0 ] != '\n' ) {
				if ( m_is_tty ) {
					char* end = buff + strlen( buff ) - 1;
					ASSERT( *end == '\n', "eol expected here" );
					if ( end > buff && ( *( end - 1 ) ) != ';' ) {
						*end = ';';
					}
					else {
						*end = '\0';
					}
				}
				m_input += buff;
				if ( m_is_tty ) {
					m_gse_context.AddSourceLine( buff );
					ProcessInput();
				}
			}
			m_gse_context.AddSourceLines( util::String::SplitToLines( m_input ) );
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
	std::string source = m_input;

	if ( m_is_tty ) {
		m_lines_count++;
	}

	auto* parser = m_gse.GetParser(
		"<STDIN>.gls." + m_syntax, source, m_is_tty
			? m_lines_count
			: 1
	);

	const gse::program::Program* program = nullptr;
	gse::Context* context = nullptr;
	try {
		program = parser->Parse();
		if ( m_is_tty ) {
			const gse::si_t si = {
				"",
				{
					m_lines_count,
					1
				},
				{
					m_lines_count,
					1
				}
			};
			context = m_gse_context.ForkContext( si, {}, {} );
		}
		else {
			context = &m_gse_context;
		}
		const auto result = m_runner->Execute( context, program );
		if ( m_is_tty ) {
			context->JoinContext( &m_gse_context );
		}
		std::cout << result.Dump() << std::endl;
	}
	catch ( gse::Exception& e ) {
		std::cout << "Unhandled exception (" + e.class_name + "): " << e.reason << std::endl;
		const auto backtrace = e.GetBacktraceAndCleanup( context );
		for ( const auto& it : backtrace ) {
			std::cout << it << std::endl;
		}
	}
	catch ( std::runtime_error& e ) {
		std::cout << "Internal error: " << e.what() << std::endl;
	}
	if ( m_is_tty ) {
		if ( context ) {
			delete context;
		}
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
