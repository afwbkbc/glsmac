#include "Drawable.h"

#include "Container.h"
#include "ui/geometry/Rectangle.h"
#include "input/Event.h"
#include "util/String.h"

namespace ui {
namespace dom {

static const std::unordered_map< std::string, geometry::Geometry::align_t > s_align_strs = {
	{
		"left",
		geometry::Geometry::ALIGN_LEFT
	},
	{
		"right",
		geometry::Geometry::ALIGN_RIGHT
	},
	{
		"top",
		geometry::Geometry::ALIGN_TOP
	},
	{
		"bottom",
		geometry::Geometry::ALIGN_BOTTOM
	},
	{
		"center",
		geometry::Geometry::ALIGN_CENTER
	},
};

Drawable::Drawable( DOM_ARGS_T, geometry::Geometry* const geometry )
	: Object( DOM_ARGS_PASS_T )
	, m_geometry( geometry ) {

#define GEOMSETTER( _key, _type ) \
    Property( \
        GSE_CALL, _key, gse::Value::_type, nullptr, PF_NONE, [ this ]( GSE_CALLABLE, gse::Value* const v )

#define GEOMPROP( _key, _method ) \
    GEOMSETTER( _key, T_INT ) { \
        m_geometry->_method( ( (gse::value::Int*)v )->value ); \
        if ( m_parent ) { \
            m_parent->UpdateMouseOver( GSE_CALL ); \
        } \
    } )
	GEOMPROP( "left", SetLeft );
	GEOMPROP( "top", SetTop );
	GEOMPROP( "right", SetRight );
	GEOMPROP( "bottom", SetBottom );
#undef GEOMPROP

	GEOMSETTER( "align", T_STRING ) {
		const auto strs = util::String::Split( ( (gse::value::String*)v )->value, ' ' );
		uint8_t align = geometry::Geometry::ALIGN_NONE;
		for ( const auto& str : strs ) {
			const auto& it = s_align_strs.find( str );
			if ( it == s_align_strs.end() ) {
				GSE_ERROR( gse::EC.UI_ERROR, "Invalid align value: " + str );
			}
			auto a = it->second;
			if ( a == geometry::Geometry::ALIGN_CENTER ) {
				if ( ( ( align & geometry::Geometry::ALIGN_VCENTER ) != geometry::Geometry::ALIGN_VCENTER ) && ( ( align & geometry::Geometry::ALIGN_TOP ) || ( align & geometry::Geometry::ALIGN_BOTTOM ) ) ) {
					a = geometry::Geometry::ALIGN_HCENTER;
				}
				if ( ( ( align & geometry::Geometry::ALIGN_HCENTER ) != geometry::Geometry::ALIGN_HCENTER ) && ( ( align & geometry::Geometry::ALIGN_LEFT ) || ( align & geometry::Geometry::ALIGN_RIGHT ) ) ) {
					a = geometry::Geometry::ALIGN_VCENTER;
				}
			}
			else if ( a == geometry::Geometry::ALIGN_LEFT ) {
				align &= ~geometry::Geometry::ALIGN_RIGHT;
			}
			else if ( a == geometry::Geometry::ALIGN_RIGHT ) {
				align &= ~geometry::Geometry::ALIGN_LEFT;
			}
			else if ( a == geometry::Geometry::ALIGN_TOP ) {
				align &= ~geometry::Geometry::ALIGN_BOTTOM;
			}
			else if ( a == geometry::Geometry::ALIGN_BOTTOM ) {
				align &= ~geometry::Geometry::ALIGN_TOP;
			}
			align |= a;
		}
		m_geometry->SetAlign( (geometry::Geometry::align_t)align );
		if ( m_parent ) {
			m_parent->UpdateMouseOver( GSE_CALL );
		}
	} );
}

Drawable::~Drawable() {
	for ( const auto& id : m_geometry_handler_ids ) {
		m_geometry->RemoveHandler( id );
	}
	delete m_geometry;
}

geometry::Geometry* const Drawable::GetGeometry() const {
	return m_geometry;
}

void Drawable::GeometryHandler( const geometry_handler_type_t type, const std::function< void() >& f ) {
	m_geometry_handler_ids.push_back( m_geometry->AddHandler( type, f ) );
}

const bool Drawable::ProcessEvent( GSE_CALLABLE, const input::Event& event ) {
	switch ( event.type ) {
		case input::EV_MOUSE_OVER: {
			m_is_mouse_over = true;
			break;
		}
		case input::EV_MOUSE_OUT: {
			m_is_mouse_over = false;
			break;
		}
		default: {
		}
	}
	return Object::ProcessEvent( GSE_CALL, event );
}

void Drawable::Show() {
	if ( !m_is_visible ) {
		Object::Show();
		m_geometry->Show();
	}
}

void Drawable::Hide() {
	if ( m_is_visible ) {
		m_geometry->Hide();
		Object::Hide();
	}
}

}
}
