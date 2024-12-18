#include "Container.h"

#include "common/Common.h"
#include "ui/Geometry.h"

#include "Img.h"

namespace ui {
namespace dom {

Container::Container( GSE_CALLABLE, UI* const ui,const std::string& tag, Object* const parent, const properties_t& properties )
	: Object( ctx, call_si, ui, tag, parent, properties ) {

	NEW( m_geometry, Geometry, ui, m_parent
		? m_parent->GetGeometry()
		: nullptr, nullptr );

	//FACTORY( "img", Img );
	Factory( ctx, call_si, "img", [ this ]( gse::context::Context* ctx, const gse::si_t& call_si, const properties_t& p ) {
		return new Img( ctx, call_si, m_ui, this, p );
	});


}

Container::~Container() {
	for ( const auto& it : m_children ) {
		delete it.second;
	}

	DELETE( m_geometry );
}

Geometry* const Container::GetGeometry() const {
	return m_geometry;
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
