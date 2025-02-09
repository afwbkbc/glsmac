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

	Events(
		{
			input::EV_MOUSE_MOVE,
			input::EV_MOUSE_OVER,
			input::EV_MOUSE_OUT,
			input::EV_MOUSE_DOWN,
			input::EV_MOUSE_UP,
		}
	);

#define GEOMSETTER( _key, _type ) \
    Property( \
        GSE_CALL, _key, gse::type::Type::_type, VALUE( gse::type::Undefined ), PF_NONE, [ this ]( GSE_CALLABLE, const gse::Value& v )

#define GEOMPROP( _key, _method ) \
    GEOMSETTER( _key, T_INT ) { \
        m_geometry->_method( ( (gse::type::Int*)v.Get() )->value ); \
        if ( m_parent ) { \
            m_parent->UpdateMouseOver( GSE_CALL ); \
        } \
    } )
	GEOMPROP( "width", SetWidth );
	GEOMPROP( "height", SetHeight );
#undef GEOMPROP

}

const bool Area::IsEventRelevant( const input::Event& event ) const {
	if ( event.flags & input::EF_MOUSE ) {
		if ( ( event.type != input::EV_MOUSE_OUT ) && !m_geometry->Contains(
			{
				event.data.mouse.x,
				event.data.mouse.y
			}
		) ) {
			return false;
		}
	}
	return Drawable::IsEventRelevant( event );
}

void Area::SerializeEvent( const input::Event& e, gse::type::object_properties_t& obj ) const {
	switch ( e.type ) {
		case input::EV_MOUSE_MOVE:
		case input::EV_MOUSE_OVER:
		case input::EV_MOUSE_OUT:
		case input::EV_MOUSE_DOWN:
		case input::EV_MOUSE_UP: {
			const auto& area = m_geometry->GetEffectiveArea();
			obj.insert(
				{
					"x",
					VALUE( gse::type::Int, e.data.mouse.x - area.left )
				}
			);
			obj.insert(
				{
					"y",
					VALUE( gse::type::Int, e.data.mouse.y - area.top )
				}
			);
			obj.insert(
				{
					"ax",
					VALUE( gse::type::Int, e.data.mouse.x )
				}
			);
			obj.insert(
				{
					"ay",
					VALUE( gse::type::Int, e.data.mouse.y )
				}
			);
			if ( e.type == input::EV_MOUSE_DOWN || e.type == input::EV_MOUSE_UP ) {
				std::string buttonstr = "";
				switch ( e.data.mouse.button ) {
					case input::MB_LEFT: {
						buttonstr = "left";
						break;
					}
					case input::MB_RIGHT: {
						buttonstr = "right";
						break;
					}
					case input::MB_MIDDLE: {
						buttonstr = "middle";
						break;
					}
					default: {
						ASSERT_NOLOG( false, "unknown button: " + std::to_string( e.data.mouse.button ) );
					}
				}
				obj.insert(
					{
						"button",
						VALUE( gse::type::String, buttonstr )
					}
				);
			}
			break;
		}
		default: {
			Drawable::SerializeEvent( e, obj );
		}
	}
}

}
}
