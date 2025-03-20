#pragma once

// top level game object
// passed to scripts and everywhere, contains global state

#include <functional>
#include <atomic>
#include <thread>

#include "common/Common.h"
#include "gse/Bindings.h"
#include "gse/GCWrappable.h"

#include "gse/GSE.h"
#include "gse/value/Object.h"

#include "util/Timer.h"

namespace gse {
class GCWrappable;
}

namespace ui {
class UI;
}

namespace task::main {
class Main;
}

namespace game::frontend {
class Game;
}

namespace game::backend {
namespace faction {
class Faction;
}
class State;
}

CLASS3( GLSMAC, common::Class, gse::Bindings, gse::GCWrappable )
	GLSMAC();
	~GLSMAC();

	void Iterate();

	WRAPDEFS_PTR( GLSMAC )

	void ShowLoader( const std::string& text );
	void SetLoaderText( const std::string& text );
	void HideLoader();

	void ShowError( const std::string& text, const std::function< void() >& on_close );

	typedef std::function< void( gse::value::object_properties_t& args ) > f_args_t;
	gse::Value* const TriggerObject( gse::Wrappable* object, const std::string& event, const f_args_t& f_args = nullptr );

	void GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) override;

#if defined( DEBUG ) || defined( FASTDEBUG )
	const std::string ToString() override;
#endif

private:
	gse::GSE* m_gse = nullptr;
	gse::context::GlobalContext* m_ctx = nullptr;
	gc::Space* m_gc_space = nullptr;

	gse::Value* m_wrapobj = nullptr;

	std::vector< gse::Value* > m_main_callables = {};

	ui::UI* m_ui = nullptr;

	typedef std::function< void() > f_t;
	std::atomic< std::thread* > m_load_thread = nullptr;
	f_t m_f_after_load = nullptr;
	std::atomic< bool > m_is_loading = false;
	bool m_is_loader_shown = false;
	std::string m_loader_text = "";
	uint8_t m_loader_dots = 0;
	util::Timer m_loader_dots_timer;
	void AsyncLoad( const std::string& text, const f_t& f, const f_t& f_after_load );

	// steps
	void S_Init( GSE_CALLABLE, const std::optional< std::string >& path );
	void S_Intro( GSE_CALLABLE );
	void S_MainMenu( GSE_CALLABLE );
	void S_Game( GSE_CALLABLE );

	void UpdateLoaderText();

	game::backend::State* m_state = nullptr;
	bool m_is_game_running = false;

	void DeinitGameState( GSE_CALLABLE );
	void InitGameState( GSE_CALLABLE, const f_t on_complete );
	void RandomizeSettings( GSE_CALLABLE );

	void AddSinglePlayerSlot( game::backend::faction::Faction* const faction );
	void StartGame( GSE_CALLABLE );

	game::frontend::Game* m_game = nullptr;

private:
	friend class task::main::Main;
	void RunMain();

private:
	friend class gse::GSE;
	void AddToContext( gc::Space* const gc_space, gse::context::Context* ctx, gse::ExecutionPointer& ep ) override;

};
