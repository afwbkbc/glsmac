#include "GLSMAC.h"

#include <iostream>

#include "util/FS.h"
#include "engine/Engine.h"
#include "config/Config.h"
#include "resource/ResourceManager.h"

#include "ui/UI.h"

#include "gse/GSE.h"
#include "gse/ExecutionPointer.h"
#include "gse/Async.h"
#include "gse/context/GlobalContext.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Undefined.h"

static GLSMAC* s_glsmac = nullptr;

GLSMAC::GLSMAC() {
	// there can only be one GLSMAC
	ASSERT( !s_glsmac, "GLSMAC already defined" );
	s_glsmac = this;

	Log( "Creating global state" );

	// scripting stuff
	NEW( m_gse, gse::GSE );
	m_gse->AddBindings( this );
	m_ctx = m_gse->CreateGlobalContext();
	m_ctx->IncRefs();

	const auto& c = g_engine->GetConfig();

	const auto entry_script =
		util::FS::GeneratePath(
			{
				c->GetDataPath(),
				"default", // only 'default' mod for now
				c->GetNewUIMainScript() // script name (extension is appended automatically)
			}, gse::GSE::PATH_SEPARATOR
		)
	;

	{
		gse::ExecutionPointer ep;

		// global objects
		NEW( m_ui, ui::UI, m_ctx, { "" }, ep );

		// run main(s)
		m_gse->RunScript( m_ctx, {}, ep, entry_script );
		for ( const auto& mod_path : g_engine->GetConfig()->GetModPaths() ) {
			m_gse->RunScript(
				m_ctx, {}, ep, util::FS::GeneratePath(
					{
						mod_path,
						"main"
					}
				)
			);
		}
	}
}

GLSMAC::~GLSMAC() {

	Log( "Destroying global state" );

	m_ctx->DecRefs();

	m_gse->GetAsync()->StopTimers();

	DELETE( m_ui );
	DELETE( m_gse );

	s_glsmac = nullptr;
}

void GLSMAC::Iterate() {
	m_gse->Iterate();
	m_ui->Iterate();
}

WRAPIMPL_BEGIN( GLSMAC )
	WRAPIMPL_PROPS
		{
			"ui",
			m_ui->Wrap( true )
		},
		{
			"start",
			NATIVE_CALL( this ) {
				S_Init( GSE_CALL, {} );
				return VALUE( gse::type::Undefined );
			} )
		},
		{
			"exit",
			NATIVE_CALL( this ) {
				g_engine->ShutDown();
				return VALUE( gse::type::Undefined );
			} )
		},
	};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( GLSMAC )

void GLSMAC::S_Init( GSE_CALLABLE, const std::optional< std::string >& path ) {
	const auto& c = g_engine->GetConfig();
	try {
		if ( path.has_value() ) {
			g_engine->GetResourceManager()->Init( { path.value() }, config::ST_AUTO );
		}
		else {
			g_engine->GetResourceManager()->Init( c->GetPossibleSMACPaths(), c->GetSMACType() );
		}
	} catch ( const std::runtime_error& e ) {
		gse::type::object_properties_t args = {
			{
				"set_smacpath", VALUE( gse::callable::Native, [ this ]( GSE_CALLABLE, const gse::type::function_arguments_t& arguments ) -> gse::Value {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( path, 0, String );
				S_Init( GSE_CALL, path );
				return VALUE( gse::type::Undefined );
			} )
			}
		};
		if ( path.has_value() ) {
			args.insert({ "last_failed_path", VALUE( gse::type::String, path.value() ) } );
		}
		Trigger( GSE_CALL, "smacpath_prompt", args );
		return;
	}
	if ( path.has_value() ) {
		c->SetSMACPath( path.value() );
	}

	if ( c->HasLaunchFlag( config::Config::LF_QUICKSTART ) ) {
		THROW( "TODO: QUICKSTART" );
	}
	else if ( c->HasLaunchFlag( config::Config::LF_SKIPINTRO ) ) {
		S_MainMenu( GSE_CALL );
	}
	else {
		S_Intro( GSE_CALL );
	}
}

void GLSMAC::S_Intro( GSE_CALLABLE ) {
	Trigger( GSE_CALL, "intro", {} );
}

void GLSMAC::S_MainMenu( GSE_CALLABLE ) {
	Trigger( GSE_CALL, "mainmenu", {} );
}

void GLSMAC::RunMain() {
	try {
		for ( const auto& main : m_main_callables ) {
			ASSERT_NOLOG( main.Get()->type == gse::type::Type::T_CALLABLE, "main not callable" );
			gse::ExecutionPointer ep;
			( (gse::type::Callable*)main.Get() )->Run( m_ctx, {}, ep, { Wrap() } );
		}
	} catch ( gse::Exception& e ) {
		std::cout << e.ToString() << std::endl;
		throw e;
	}
}

void GLSMAC::AddToContext( gse::context::Context* ctx, gse::ExecutionPointer& ep ) {
	ctx->CreateBuiltin( "main", NATIVE_CALL( this ) {
		N_EXPECT_ARGS( 1 );
		const auto& main = arguments.at(0);
		N_CHECKARG( main.Get(), 0, Callable );
		m_main_callables.push_back( main );
		return VALUE( gse::type::Undefined );
	} ), ep );
}
