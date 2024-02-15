#pragma once

#include <unordered_map>

#include "util/FS.h"
#include "gse/Bindings.h"
#include "gse/GlobalContext.h"
#include "gse/type/Callable.h"
#include "game/Player.h"
#include "game/rules/Faction.h"
#include "game/rules/DifficultyLevel.h"

#include "Binding.h"

namespace game {
class Game;

namespace bindings {

class Bindings : public gse::Bindings {
public:
	Bindings( Game* game );
	~Bindings();

	void AddToContext( gse::Context* ctx ) override;

	void RunMain();

	enum callback_slot_t {
		CS_ON_START,
		CS_ON_SPAWN_UNIT,
		CS_ON_DESPAWN_UNIT,
	};
	typedef std::vector< gse::Value > callback_arguments_t;
	void Call( const callback_slot_t slot, const callback_arguments_t& arguments = {} );

	// for bindings
	Game* GetGame() const;
	void SetCallback( const callback_slot_t slot, const gse::Value& callback, gse::Context* context, const gse::si_t& si );

private:

	std::vector< Binding* > m_bindings = {};

	const gse::si_t m_si_internal = { "" };
	const gse::type::Callable::function_arguments_t m_no_arguments = {};

	std::unordered_map< callback_slot_t, gse::Value > m_callbacks = {};

	Game* m_game;

	const gse::Value GetPlayer( const Player* player ) const;
	const gse::Value GetFaction( const rules::Faction* faction ) const;
	const gse::Value GetDifficulty( const rules::DifficultyLevel* difficulty ) const;
	//const gse::Value GetTurn( const Turn* turn ) const;

	const std::string m_entry_script = util::FS::GeneratePath(
		{
			"gse", // directory is expected to be in working dir
			"default", // only 'default' mod for now
			"main" // script name (extension is appended automatically)
		}
	);
	gse::GSE* m_gse = nullptr;
	gse::GlobalContext* m_gse_context = nullptr;
};

}
}

