#pragma once

#include "common/Common.h"

#include "gse/type/Types.h"

#include "gse/Value.h"

namespace game {
namespace backend {

class Game;

class State;
namespace bindings {

class Bindings;

class Binding {
public:
	Binding( const std::string& name, Bindings* bindings );
	virtual ~Binding() = default;

	void Add( gse::type::object_properties_t& methods );

protected:

	virtual gse::Value Get() = 0;

	const std::string m_name;
	Bindings* m_bindings;

};

}
}
}

#define BINDING( _name ) Binding_##_name

#define BINDING_DEF( _name ) \
    class BINDING( _name ) : public Binding { \
    public: \
        BINDING( _name )( Bindings* bindings ) \
            : Binding( #_name, bindings ) {} \
    protected: \
        gse::Value Get() override; \
    };

#define BINDING_IMPL( _name ) \
    gse::Value BINDING( _name )::Get()

#define GAME m_bindings->GetGame( ctx, call_si )

namespace game {
namespace backend {
namespace bindings {

BINDING_DEF( message )

BINDING_DEF( exit )

BINDING_DEF( random )

BINDING_DEF( on )

BINDING_DEF( players )

BINDING_DEF( factions )

BINDING_DEF( tiles )

BINDING_DEF( units )

BINDING_DEF( bases )

BINDING_DEF( animations )

BINDING_DEF( map )

BINDING_DEF( resources )

}
}
}

