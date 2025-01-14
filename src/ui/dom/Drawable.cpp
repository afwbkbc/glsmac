#include "Drawable.h"

#include "Container.h"
#include "ui/geometry/Rectangle.h"

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
        ctx, call_si, _key, gse::type::Type::_type, VALUE( gse::type::Undefined ), PF_NONE, [ this ]( GSE_CALLABLE, const gse::Value& v )

#define GEOMPROP( _key, _method ) \
    GEOMSETTER( _key, T_INT ) { \
        m_geometry->_method( ( (gse::type::Int*)v.Get() )->value ); \
        if ( m_parent ) { \
            m_parent->UpdateMouseOver( ctx, call_si, this ); \
        } \
    } )
	GEOMPROP( "left", SetLeft );
	GEOMPROP( "top", SetTop );
	GEOMPROP( "right", SetRight );
	GEOMPROP( "bottom", SetBottom );
#undef GEOMPROP

	GEOMSETTER( "align", T_STRING ) {
		const auto strs = util::String::Split( ( (gse::type::String*)v.Get() )->value, ' ' );
		uint8_t align = geometry::Geometry::ALIGN_NONE;
		for ( const auto& str : strs ) {
			const auto& it = s_align_strs.find( str );
			if ( it == s_align_strs.end() ) {
				throw gse::Exception( gse::EC.UI_ERROR, "Invalid align value: " + str, ctx, call_si );
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
			m_parent->UpdateMouseOver( ctx, call_si, this );
		}
	} );
}

Drawable::~Drawable() {
	delete m_geometry;
}

geometry::Geometry* const Drawable::GetGeometry() const {
	return m_geometry;
}

}
}