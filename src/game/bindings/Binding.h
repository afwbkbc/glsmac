#pragma once

#include "gse/type/Object.h"

#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/Value.h"
#include "gse/type/Undefined.h"

namespace game {
class Game;

class State;
namespace bindings {

class Bindings;

class Binding {
public:
	Binding( const std::string& name, Bindings* bindings );
	virtual ~Binding() = default;

	void Add( gse::type::Object::properties_t& methods );

protected:

	virtual gse::Value Get() = 0;

	const std::string m_name;
	Bindings* m_bindings;

};

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

#define ERROR( _type, _text ) throw gse::Exception( _type, _text, ctx, call_si );

#define CALLBACK( _type ) NATIVE_CALL( this ) { \
    N_EXPECT_ARGS( 1 ); \
    N_GET( cb, 0 ); \
    N_CHECKTYPE( cb.Get(), 0, Callable ); \
    m_bindings->SetCallback( _type, cb, ctx, call_si ); \
    return VALUE( gse::type::Undefined ); \
})

#include "Bindings.h"
#include "../Game.h"

namespace game {
namespace bindings {

BINDING_DEF( message )

BINDING_DEF( exit )

BINDING_DEF( random )

BINDING_DEF( on )

BINDING_DEF( factions )

BINDING_DEF( units )

BINDING_DEF( map )

}
}
