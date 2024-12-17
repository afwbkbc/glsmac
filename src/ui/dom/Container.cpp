#include "Container.h"

#include "Img.h"

namespace ui {
namespace dom {

Container::Container( const std::string& cls, Object* const parent, const properties_t& properties )
	: Object( cls, parent, properties ) {

	FACTORY( "img", Img );

}

Container::~Container() {
	for ( const auto& it : m_children ) {
		delete it.second;
	}
}

void Container::Factory( const std::string& name, const std::function< Object*( const properties_t& ) >& f ) {
	Method( name, NATIVE_CALL( this, f ) {
		N_EXPECT_ARGS_MIN_MAX(0, 1);
		properties_t initial_properties = {};
		if ( arguments.size() == 1 ) {
			const auto& v = arguments.at(0).Get();
			if ( v->type != gse::type::Type::T_OBJECT ) {
				throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Expected properties object, got " + v->GetTypeString( v->type ), ctx, call_si );
			}
			initial_properties = ((gse::type::Object*)v)->value;
		}
		auto* obj = f( initial_properties );
		m_children.insert({ obj->m_id, obj });
		return obj->Wrap( true );
	} ) );
}

}
}
