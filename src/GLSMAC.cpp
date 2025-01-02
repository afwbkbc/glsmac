#include "GLSMAC.h"

#include <iostream>

#include "util/FS.h"
#include "engine/Engine.h"
#include "config/Config.h"

#include "ui/UI.h"

#include "gse/GSE.h"
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

	// global objects
	NEW( m_ui, ui::UI, m_ctx, {""} );

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
	m_gse->RunScript( m_ctx, { }, entry_script );
	for ( const auto& mod_path : g_engine->GetConfig()->GetModPaths() ) {
		m_gse->RunScript( m_ctx, {}, util::FS::GeneratePath({ mod_path, "main" }));
	}

}

GLSMAC::~GLSMAC() {

	Log( "Destroying global state" );

	m_ctx->DecRefs();

	m_gse->GetAsync()->StopTimers();

	DELETE( m_gse );
	s_glsmac = nullptr;
}

void GLSMAC::Iterate() {
	m_gse->Iterate();
}

WRAPIMPL_BEGIN( GLSMAC )
	WRAPIMPL_PROPS
		{
			"ui",
			m_ui->Wrap( true )
		},
	};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( GLSMAC )

void GLSMAC::RunMain() {
	try {
		for ( const auto& main : m_main_callables ) {
			ASSERT_NOLOG( main.Get()->type == gse::type::Type::T_CALLABLE, "main not callable" );
			( (gse::type::Callable*)main.Get() )->Run( m_ctx, {}, { Wrap() } );
		}
	} catch ( gse::Exception& e ) {
		std::cout << e.ToStringAndCleanup() << std::endl;
		throw e;
	}
}

void GLSMAC::AddToContext( gse::context::Context* ctx ) {
	ctx->CreateBuiltin( "main", NATIVE_CALL( this ) {
		N_EXPECT_ARGS( 1 );
		const auto& main = arguments.at(0);
		N_CHECKARG( main.Get(), 0, Callable );
		m_main_callables.push_back( main );
		return VALUE( gse::type::Undefined );
	} ) );
}
