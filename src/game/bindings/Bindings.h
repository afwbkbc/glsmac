#pragma once

#include <unordered_map>
#include <map>
#include <vector>

#include "gse/Types.h"
#include "gse/type/Types.h"

#include "gse/Bindings.h"
#include "gse/Value.h"

namespace gse {
class GSE;

namespace context {
class GlobalContext;
}

}

namespace game {

class Game;
class State;

namespace bindings {

class Binding;

class Bindings : public gse::Bindings {
public:
	Bindings( State* state );
	~Bindings();

	void AddToContext( gse::context::Context* ctx ) override;

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
		CS_ON_UNIT_ATTACK_VALIDATE,
		CS_ON_UNIT_ATTACK_RESOLVE,
		CS_ON_UNIT_ATTACK_APPLY,
		CS_ON_UNIT_TURN,
		CS_ON_BASE_SPAWN,
		CS_ON_BASE_TURN,
	};
	typedef std::map< std::string, gse::Value > callback_arguments_t;
	gse::Value Call( const callback_slot_t slot, const callback_arguments_t& arguments = {} );

	State* GetState() const;
	Game* GetGame( gse::context::Context* ctx, const gse::si_t& call_si ) const;
	void SetCallback( const callback_slot_t slot, const gse::Value& callback, gse::context::Context* context, const gse::si_t& si );

private:

	std::vector< Binding* > m_bindings = {};

	const gse::si_t m_si_internal = { "" };
	const gse::type::function_arguments_t m_no_arguments = {};

	std::unordered_map< callback_slot_t, gse::Value > m_callbacks = {};

	State* m_state = nullptr;

	const std::string m_entry_script;

	gse::GSE* m_gse = nullptr;
	gse::context::GlobalContext* m_gse_context = nullptr;
};

}
}

