#include "Area.h"

#include "Container.h"
#include "ui/geometry/Rectangle.h"
#include "input/Event.h"

namespace ui {
namespace dom {

Area::Area( DOM_ARGS_T )
	: Drawable(
	DOM_ARGS_PASS_T, new geometry::Rectangle(
		ui, parent
			? parent->GetGeometry()
			: nullptr
	)
) {

	Events( { input::EV_MOUSE_MOVE } );

#define GEOMSETTER( _key, _type ) \
    Property( \
        ctx, call_si, _key, gse::type::Type::_type, VALUE( gse::type::Undefined ), PF_NONE, [ this ]( GSE_CALLABLE, const gse::Value& v )

#define GEOMPROP( _key, _method ) \
    GEOMSETTER( _key, T_INT ) { \
        m_geometry->_method( ( (gse::type::Int*)v.Get() )->value ); \
    } )
	GEOMPROP( "width", SetWidth );
	GEOMPROP( "height", SetHeight );
#undef GEOMPROP

}

const bool Area::IsEventRelevant( const input::Event& event ) const {
	if ( event.flags & input::EF_MOUSE ) {
		if ( !m_geometry->Contains(
			{
				event.data.mouse.absolute.x,
				event.data.mouse.absolute.y
			}
		) ) {
			return false;
		}
	}
	return Drawable::IsEventRelevant( event );
}

void Area::SerializeEvent( const input::Event& e, gse::type::object_properties_t& obj ) const {
	switch ( e.type ) {
		case input::EV_MOUSE_MOVE: {
			const auto& area = m_geometry->GetEffectiveArea();
			obj.insert(
				{
					"x",
					VALUE( gse::type::Int, e.data.mouse.absolute.x - area.left )
				}
			);
			obj.insert(
				{
					"y",
					VALUE( gse::type::Int, e.data.mouse.absolute.y - area.top )
				}
			);
			obj.insert(
				{
					"ax",
					VALUE( gse::type::Int, e.data.mouse.absolute.x )
				}
			);
			obj.insert(
				{
					"ay",
					VALUE( gse::type::Int, e.data.mouse.absolute.y )
				}
			);
			break;
		}
		default: {
			Drawable::SerializeEvent( e, obj );
		}
	}
}

}
}
