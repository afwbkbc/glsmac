#include "Area.h"

#include "ui/geometry/Rectangle.h"

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

}
}
