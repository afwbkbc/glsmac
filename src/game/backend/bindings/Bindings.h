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
class Wrappable;

namespace context {
class GlobalContext;
}

}

namespace game {
namespace backend {

class System;
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

	typedef std::map< std::string, gse::Value > callback_arguments_t;
	const gse::Value Trigger( gse::Wrappable* object, const std::string& event, const gse::type::object_properties_t& args );

	State* GetState() const;
	Game* GetGame( GSE_CALLABLE ) const;

private:

	std::vector< Binding* > m_bindings = {};

	const gse::si_t m_si_internal = { "" };
	const gse::type::function_arguments_t m_no_arguments = {};

	State* m_state = nullptr;

	const std::string m_entry_script;

	gse::GSE* m_gse = nullptr;
	gse::context::GlobalContext* m_gse_context = nullptr;
};

}
}
}
