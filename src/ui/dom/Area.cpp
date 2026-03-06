#include "Area.h"

#include "Container.h"
#include "ui/geometry/Rectangle.h"
#include "input/Event.h"
#include "ui/UI.h"

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
        GSE_CALL, _key, gse::_type, nullptr, PF_NONE, [ this ]( GSE_CALLABLE, gse::Value* const v )

#define GEOMPROP( _key, _method ) \
    GEOMSETTER( _key, VT_INT ) { \
        m_geometry->_method( ( (gse::value::Int*)v )->value ); \
        if ( m_parent ) { \
            m_parent->UpdateMouseOver( GSE_CALL ); \
        } \
    } )
	GEOMPROP( "width", SetWidth );
	GEOMPROP( "height", SetHeight );
#undef GEOMPROP

	m_geometry->m_on_resize = [ this ]( const size_t width, const size_t height, const bool is_update_from_parent ) {
		m_ui->WithGSE( // TODO: optimize a bit
			[ this, width, height, is_update_from_parent ]( GSE_CALLABLE ) {
				auto* const w = VALUE( gse::value::Int, , width );
				auto* const h = VALUE( gse::value::Int, , height );
				UpdateProperty( "width", w );
				UpdateProperty( "height", h );
				if ( m_on_resize ) {
					m_on_resize( width, height );
				}
				if ( is_update_from_parent ) {
					gse::value::object_properties_t event_data = {
						{ "width",  w },
						{ "height", h },
					};
					Trigger( GSE_CALL, "resize", ARGS( event_data ) );
				}
			}
		);
	};
}

const bool Area::IsEventRelevant( const input::Event& event ) const {
	if ( event.flags & input::EF_MOUSE ) {
		if (
			event.type != input::EV_MOUSE_OUT &&
				(
					!m_is_visible ||
						!m_geometry->Contains(
							{
								event.data.mouse.x,
								event.data.mouse.y
							}
						)
				)
			) {
			return false;
		}
	}
	return Drawable::IsEventRelevant( event );
}

void Area::WrapEvent( GSE_CALLABLE, const input::Event& e, gse::value::object_properties_t& obj ) const {
	switch ( e.type ) {
		case input::EV_MOUSE_MOVE:
		case input::EV_MOUSE_OVER:
		case input::EV_MOUSE_OUT:
		case input::EV_MOUSE_DOWN:
		case input::EV_MOUSE_UP:
		case input::EV_MOUSE_SCROLL: {
			const auto& area = m_geometry->GetEffectiveArea();
			obj.insert(
				{
					"x",
					VALUE( gse::value::Int, , e.data.mouse.x - area.left )
				}
			);
			obj.insert(
				{
					"y",
					VALUE( gse::value::Int, , e.data.mouse.y - area.top )
				}
			);
			obj.insert(
				{
					"ax",
					VALUE( gse::value::Int, , e.data.mouse.x )
				}
			);
			obj.insert(
				{
					"ay",
					VALUE( gse::value::Int, , e.data.mouse.y )
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
						ASSERT( false, "unknown button: " + std::to_string( e.data.mouse.button ) );
					}
				}
				obj.insert(
					{
						"button",
						VALUE( gse::value::String, , buttonstr )
					}
				);
			}
			if ( e.type == input::EV_MOUSE_SCROLL ) {
				obj.insert(
					{
						"scroll_y",
						VALUE( gse::value::Int, , e.data.mouse.scroll_y )
					}
				);
			}
			break;
		}
		default: {
			Drawable::WrapEvent( GSE_CALL, e, obj );
		}
	}
}

}
}
