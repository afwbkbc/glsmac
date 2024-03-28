#pragma once

#include <unordered_map>
#include <map>

#include "util/FS.h"
#include "gse/GSE.h"
#include "gse/Bindings.h"
#include "gse/GlobalContext.h"
#include "gse/type/Callable.h"
#include "game/Player.h"

#include "Binding.h"

namespace game {
class Game;

class State;

namespace bindings {

class Bindings : public gse::Bindings {
public:
	Bindings( State* state );
	~Bindings();

	void AddToContext( gse::Context* ctx ) override;

	void RunMain();

	enum callback_slot_t {
		CS_ON_CONFIGURE,
		CS_ON_START,
		CS_ON_TURN,
		CS_ON_UNIT_SPAWN,
		CS_ON_UNIT_DESPAWN,
		CS_ON_UNIT_MOVE_VALIDATE,
		CS_ON_UNIT_MOVE_RESOLVE,
		CS_ON_UNIT_MOVE_APPLY,
		CS_ON_UNIT_TURN,
	};
	typedef std::map< std::string, gse::Value > callback_arguments_t;
	gse::Value Call( const callback_slot_t slot, const callback_arguments_t& arguments = {} );

	State* GetState() const;
	Game* GetGame( gse::Context* ctx, const gse::si_t& call_si ) const;
	void SetCallback( const callback_slot_t slot, const gse::Value& callback, gse::Context* context, const gse::si_t& si );

private:

	std::vector< Binding* > m_bindings = {};

	const gse::si_t m_si_internal = { "" };
	const gse::type::Callable::function_arguments_t m_no_arguments = {};

	std::unordered_map< callback_slot_t, gse::Value > m_callbacks = {};

	State* m_state = nullptr;

	const std::string m_entry_script = util::FS::GeneratePath(
		{
			"gse", // directory is expected to be in working dir
			"default", // only 'default' mod for now
			"main" // script name (extension is appended automatically)
		}, gse::GSE::PATH_SEPARATOR
	);

	gse::GSE* m_gse = nullptr;
	gse::GlobalContext* m_gse_context = nullptr;
};

}
}

