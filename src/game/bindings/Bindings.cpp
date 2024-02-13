#include "Bindings.h"

#include "game/Game.h"

#include "gse/type/String.h"
#include "gse/type/Int.h"
#include "gse/type/Object.h"
#include "gse/type/Callable.h"

using namespace gse;

namespace game {
namespace bindings {

Bindings::Bindings( Game* game )
	: m_game( game ) {
	NEW( m_gse, gse::GSE );
	m_gse->AddBindings( this );
	m_gse_context = m_gse->CreateGlobalContext();
	m_gse_context->IncRefs();

#define B( _name ) new BINDING( _name )( this )
	m_bindings = {
		B( message ),
		B( exit ),
		B( on_start ),
		B( units ),
		B( map ),
	};
#undef B
}

Bindings::~Bindings() {
	for ( auto& it : m_bindings ) {
		delete it;
	}
	m_gse_context->DecRefs();
	DELETE( m_gse );
}

void Bindings::AddToContext( gse::Context* ctx ) {
	type::Object::properties_t methods = {};
	for ( auto& it : m_bindings ) {
		it->Add( methods );
	}
	ctx->CreateBuiltin( "game", VALUE( type::Object, methods ) );
}

void Bindings::RunMain() {
	m_gse->GetInclude( m_gse_context, m_si_internal, m_entry_script );
}

void Bindings::Call( const callback_slot_t slot, const callback_arguments_t* arguments ) {
	const auto& it = m_callbacks.find( slot );
	if ( it != m_callbacks.end() ) {
		try {
			( (gse::type::Callable*)it->second.Get() )->Run(
				m_gse_context, m_si_internal, arguments
					? *arguments
					: m_no_arguments
			);
		}
		catch ( gse::Exception& e ) {
			m_game->OnGSEError( e );
		}
	}
}

Game* Bindings::GetGame() const {
	return m_game;
}

const Value Bindings::GetPlayer( const Player* player ) const {
	const type::Object::properties_t properties = {
		{
			{
				"name",
				VALUE( type::String, player->GetPlayerName() ),
			},
			{
				"full_name",
				VALUE( type::String, player->GetFullName() ),
			},
			{
				"faction",
				GetFaction( &player->GetFaction() ),
			},
			{
				"difficulty",
				GetDifficulty( &player->GetDifficultyLevel() )
			},
		}
	};
	return VALUE( type::Object, properties );
}

const gse::Value Bindings::GetFaction( const rules::Faction* faction ) const {
	const type::Object::properties_t properties = {
		{
			"name",
			VALUE( type::String, faction->m_name ),
		}
	};
	return VALUE( type::Object, properties );
}

const gse::Value Bindings::GetDifficulty( const rules::DifficultyLevel* difficulty ) const {
	const type::Object::properties_t properties = {
		{
			"name",  VALUE( type::String, difficulty->m_name ),
		},
		{
			"value", VALUE( type::Int, difficulty->m_difficulty ),
		},
	};
	return VALUE( type::Object, properties );
}

void Bindings::SetCallback( const callback_slot_t slot, const gse::Value& callback, gse::Context* context, const si_t& si ) {
	if ( m_callbacks.find( slot ) != m_callbacks.end() ) {
		throw gse::Exception( EC.GAME_ERROR, "Callback slot already in use", context, si );
	}
	m_callbacks.insert_or_assign( slot, callback );
}

}
}
