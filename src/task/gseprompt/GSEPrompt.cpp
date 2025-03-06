#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#else

#include <unistd.h>

#endif

#include <cstring>

#include "GSEPrompt.h"

#include "gse/Exception.h"
#include "engine/Engine.h"
#include "util/String.h"
#include "util/LogHelper.h"
#include "gse/GSE.h"
#include "gse/context/GlobalContext.h"
#include "gse/context/ChildContext.h"
#include "gse/runner/Runner.h"
#include "gse/parser/Parser.h"
#include "gse/program/Program.h"
#include "gse/ExecutionPointer.h"
#include "gc/Space.h"

namespace task {
namespace gseprompt {

GSEPrompt::GSEPrompt( const std::string& syntax )
	: m_syntax( syntax )
	, m_is_tty( isatty( fileno( stdin ) ) ) {}

GSEPrompt::~GSEPrompt() {}

void GSEPrompt::Start() {
	Log( "Starting GSE prompt (syntax: " + m_syntax + ")" );
	m_gse = new gse::GSE();
	m_gc_space = m_gse->GetGCSpace();
	m_runner = m_gse->GetRunner();
	if ( m_is_tty ) {
		m_runner->EnableScopeContextJoins();
	}
	m_gse_context = m_gse->CreateGlobalContext();
	m_is_running = true;
	if ( m_is_tty ) {
		util::LogHelper::Println( "" );
	}
	PrintPrompt();
}

void GSEPrompt::Stop() {
	Log( "Exiting GSE prompt" );
	for ( const auto& it : m_programs ) {
		delete ( it );
	}
	m_programs.clear();
	m_gse_context = nullptr;
	m_runner = nullptr;
	m_input.clear();
	if ( m_gse_context ) {
		delete m_gse_context;
		m_gse_context = nullptr;
	}
	delete m_gse;
	m_gse = nullptr;
	m_gc_space = nullptr;
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
					util::LogHelper::Println( "" );
					util::LogHelper::Println( "" );
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
					m_gse_context->AddSourceLine( buff );
					ProcessInput();
				}
			}
			m_gse_context->AddSourceLines( util::String::Split( m_input, '\n' ) );
			PrintPrompt();
		}
	}
}

void GSEPrompt::PrintPrompt() {
	if ( m_is_tty ) {
		util::LogHelper::Print( m_syntax + "> " );
		util::LogHelper::Flush();
	}
}

void GSEPrompt::ProcessInput() {

	m_gc_space->Accumulate(
		[ this ]() {
			std::string source = m_input;

			if ( m_is_tty ) {
				m_lines_count++;
			}
			auto* parser = m_gse->CreateParser(
				"<STDIN>.gls." + m_syntax, source, m_is_tty
					? m_lines_count
					: 1
			);

			const gse::program::Program* program = nullptr;
			gse::Value* result = nullptr;
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
					{
						gse::ExecutionPointer ep;
						m_gse_context->ForkAndExecute(
							m_gc_space, m_gse_context, si, ep, false, [ this, &result, &ep, &program ]( gse::context::ChildContext* const subctx ) {
								result = m_runner->Execute( subctx, ep, program );
								subctx->JoinContext();
							}
						);
					}
				}
				else {
					m_gse_context->Execute(
						[ this, &result, &program ] {
							gse::ExecutionPointer ep;
							result = m_runner->Execute( m_gse_context, ep, program );
						}
					);
				}
				if ( result ) {
					util::LogHelper::Println( result->Dump() );
				}
			}
			catch ( const gse::Exception& e ) {
				util::LogHelper::Println( e.ToString() );
			}
			catch ( std::runtime_error& e ) {
				util::LogHelper::Println( (std::string)"Internal error: " + e.what() );
			}
			if ( m_is_tty ) {
				util::LogHelper::Println( "" );
			}

			if ( program ) {
				// can't delete here because program may have contained some functions that are still bound to context
				// TODO: think how to deal with it
				//DELETE( program );
				m_programs.push_back( program );
			}
		}
	);
	m_input.clear();
}

}
}
