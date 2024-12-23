#include "Container.h"

#include "common/Common.h"
#include "ui/geometry/Geometry.h"

#include "Surface.h"
#include "Panel.h"
#include "Text.h"

namespace ui {
namespace dom {

Container::Container( DOM_ARGS_T )
	: Area( DOM_ARGS_PASS_T ) {

	FACTORY( "surface", Surface );
	FACTORY( "panel", Panel );
	FACTORY( "text", Text );

}

Container::~Container() {
	for ( const auto& it : m_children ) {
		delete it.second;
	}
}

void Container::Factory( GSE_CALLABLE, const std::string& name, const std::function< Object*( GSE_CALLABLE, const properties_t& ) >& f ) {
	Method( ctx, call_si, name, NATIVE_CALL( this, f ) {
		N_EXPECT_ARGS_MIN_MAX(0, 1);
		properties_t initial_properties = {};
		if ( arguments.size() == 1 ) {
			const auto& v = arguments.at(0).Get();
			if ( v->type != gse::type::Type::T_OBJECT ) {
				throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Expected properties object, got " + v->GetTypeString( v->type ), ctx, call_si );
			}
			initial_properties = ((gse::type::Object*)v)->value;
		}
		auto* obj = f( ctx, call_si, initial_properties );
		obj->Validate( ctx, call_si );
		m_children.insert({ obj->m_id, obj });
		return obj->Wrap( true );
	} ) );
}

}
}
