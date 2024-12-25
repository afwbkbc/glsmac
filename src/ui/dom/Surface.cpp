#include "Surface.h"

#include "types/mesh/Rectangle.h"
#include "scene/actor/Mesh.h"
#include "ui/geometry/Rectangle.h"
#include "engine/Engine.h"
#include "graphics/Graphics.h"
#include "types/texture/Texture.h"

namespace ui {
namespace dom {

Surface::Surface( DOM_ARGS )
	: Area( DOM_ARGS_PASS, "surface" ) {

	NEW( m_mesh, types::mesh::Rectangle );
	m_geometry->AsRectangle()->SetMesh( m_mesh );
	m_actor = new scene::actor::Mesh( "UI::Surface", m_mesh );
	Actor( m_actor );

	Property(
		ctx, call_si, "background", gse::type::Type::T_STRING, VALUE( gse::type::Undefined ), PF_NONE, [ this ]( GSE_CALLABLE, const gse::Value& v ) {
			const auto& str = ( (gse::type::String*)v.Get() )->value;
			if ( str.empty() ) {
				throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Property 'background' expects color code or texture path", ctx, call_si );
			}
			if ( str.at( 0 ) == '#' ) {
				types::Color color;
				ParseColor( ctx, call_si, str, color );
				if ( !m_background.texture ) {
					m_background.texture = new types::texture::Texture( 1, 1 );
				}
				m_background.texture->SetPixel( 0, 0, color );
				m_actor->SetTexture( m_background.texture );
			}
			else {
				// texture path
			}
		}
	);
}

Surface::~Surface() {
	if ( m_background.color_texture ) {
		delete m_background.color_texture;
	}
}

}
}
