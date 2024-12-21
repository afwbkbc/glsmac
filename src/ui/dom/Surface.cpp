#include "Surface.h"

#include "types/mesh/Rectangle.h"
#include "scene/actor/Mesh.h"
#include "ui/UI.h"
#include "ui/Geometry.h"
#include "scene/Scene.h"
#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"
#include "graphics/Graphics.h"
#include "util/String.h"
#include "types/texture/Texture.h"

namespace ui {
namespace dom {

Surface::Surface( DOM_ARGS_T )
	: Area( DOM_ARGS_PASS_T ) {

	std::string cls = "UI::Mesh::Surface";
	NEW( m_mesh, types::mesh::Rectangle );
	NEW( m_actor, scene::actor::Mesh, cls, m_mesh );
	m_geometry->SetMesh( m_mesh );
	m_ui->m_scene->AddActor( m_actor );

	Property(
		ctx, call_si, "background", gse::type::Type::T_STRING, VALUE( gse::type::Undefined ), PF_NONE, [ this ]( GSE_CALLABLE, const gse::Value& v ) {
			const auto& str = ( (gse::type::String*)v.Get() )->value;
			if ( str.empty() ) {
				throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Property 'background' expects color code or texture path", ctx, call_si );
			}
			if ( str.at( 0 ) == '#' ) {
				types::Color color;
				if ( !util::String::ParseColor( str, color ) ) {
					throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Property 'background' has invalid color format. Expected hex values ( #RRGGBB or #RRGGBBAA ), got: " + str, ctx, call_si );
				}
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
	m_ui->m_scene->RemoveActor( m_actor );
	if ( m_background.color_texture ) {
		delete m_background.color_texture;
	}
	m_geometry->SetMesh( nullptr );
	delete m_actor;
}

}
}
