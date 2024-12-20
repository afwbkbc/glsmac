#include "Area.h"

#include "ui/Geometry.h"

#include "util/String.h"

namespace ui {
namespace dom {

static const std::unordered_map< std::string, Geometry::align_t > s_align_strs = {
	{
		"left",
		Geometry::ALIGN_LEFT
	},
	{
		"right",
		Geometry::ALIGN_RIGHT
	},
	{
		"top",
		Geometry::ALIGN_TOP
	},
	{
		"bottom",
		Geometry::ALIGN_BOTTOM
	},
	{
		"center",
		Geometry::ALIGN_CENTER
	},
};

Area::Area( DOM_ARGS_T )
	: Object( DOM_ARGS_PASS_T )
	, m_geometry(
		new Geometry(
			ui, m_parent
				? m_parent->GetGeometry()
				: nullptr
		)
	) {

#define GEOMSETTER( _key, _type ) \
    Property( \
        ctx, call_si, _key, gse::type::Type::_type, VALUE( gse::type::Undefined ), PF_NONE, [ this ]( GSE_CALLABLE, const gse::Value& v )

#define GEOMPROP( _key, _method ) \
    GEOMSETTER( _key, T_INT ) { \
        m_geometry->_method( ( (gse::type::Int*)v.Get() )->value ); \
    } )
	GEOMPROP( "width", SetWidth );
	GEOMPROP( "height", SetHeight );
	GEOMPROP( "left", SetLeft );
	GEOMPROP( "top", SetTop );
	GEOMPROP( "right", SetRight );
	GEOMPROP( "bottom", SetBottom );
#undef GEOMPROP

	GEOMSETTER( "align", T_STRING ) {
		const auto strs = util::String::Split( ( (gse::type::String*)v.Get() )->value, ' ' );
		uint8_t align = Geometry::ALIGN_NONE;
		for ( const auto& str : strs ) {
			const auto& it = s_align_strs.find( str );
			if ( it == s_align_strs.end() ) {
				throw gse::Exception( gse::EC.UI_ERROR, "Invalid align value: " + str, ctx, call_si );
			}
			auto a = it->second;
			if ( a == Geometry::ALIGN_CENTER ) {
				if ( ( ( align & Geometry::ALIGN_VCENTER ) != Geometry::ALIGN_VCENTER ) && ( ( align & Geometry::ALIGN_TOP ) || ( align & Geometry::ALIGN_BOTTOM ) ) ) {
					a = Geometry::ALIGN_HCENTER;
				}
				if ( ( ( align & Geometry::ALIGN_HCENTER ) != Geometry::ALIGN_HCENTER ) && ( ( align & Geometry::ALIGN_LEFT ) || ( align & Geometry::ALIGN_RIGHT ) ) ) {
					a = Geometry::ALIGN_VCENTER;
				}
			}
			else if ( a == Geometry::ALIGN_LEFT ) {
				align &= ~Geometry::ALIGN_RIGHT;
			}
			else if ( a == Geometry::ALIGN_RIGHT ) {
				align &= ~Geometry::ALIGN_LEFT;
			}
			else if ( a == Geometry::ALIGN_TOP ) {
				align &= ~Geometry::ALIGN_BOTTOM;
			}
			else if ( a == Geometry::ALIGN_BOTTOM ) {
				align &= ~Geometry::ALIGN_TOP;
			}
			align |= a;
		}
		m_geometry->SetAlign( (Geometry::align_t)align );
	} );

}

Area::~Area() {
	delete m_geometry;
}

Geometry* const Area::GetGeometry() const {
	return m_geometry;
}

}
}