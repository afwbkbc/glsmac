#include "Mesh.h"

#include "util/Clamper.h"
#include "engine/Engine.h"
#include "scene/actor/Mesh.h"
#include "types/mesh/Mesh.h"
#include "types/mesh/Simple.h"
#include "types/mesh/Render.h"
#include "types/texture/Texture.h"
#include "graphics/Graphics.h"

namespace ui_legacy {
namespace object {

Mesh::Mesh( const std::string& class_name )
	: UIObject( class_name ) {
	//
}

void Mesh::SetMesh( const types::mesh::Mesh* mesh ) {
	if ( m_actor ) {
		RemoveActor( m_actor );
		DELETE( m_actor );
		m_actor = nullptr;
	}
	if ( mesh ) {
		if ( m_original_mesh ) {
			DELETE( m_original_mesh );
		}
		m_original_mesh = mesh;
		std::string cls = "UI::Mesh";
		std::string style_class = GetStyleClass();
		if ( !style_class.empty() ) {
			cls += "::" + style_class;
		}
		NEW( m_mesh, types::mesh::Mesh, *m_original_mesh ); // copy
		NEW( m_actor, scene::actor::Mesh, cls, m_mesh );
		UpdateRenderFlags();
		if ( m_texture ) {
			m_actor->SetTexture( m_texture );
		}
		AddActor( m_actor );
	}
}

const types::mesh::Mesh* Mesh::GetOriginalMesh() const {
	return m_original_mesh;
}

void Mesh::SetTexture( types::texture::Texture* texture ) {
	if ( texture != m_texture ) {
		/*if ( m_texture && m_texture ) {
			Log( "Changing texture from " + m_texture->m_name + " to " + texture->m_name );
		}
		else if ( texture ) {
			Log( "Setting texture " + texture->m_name );
		}
		else if ( m_texture ) {
			Log( "Clearing texture " + texture->m_name );
		}*/
		m_texture = texture;
		if ( m_actor ) {
			m_actor->SetTexture( m_texture );
		}
		Realign();
	}
}

void Mesh::ClearTexture() {
	if ( m_texture ) {
		//Log( "Clearing texture" );
		if ( m_actor ) {
			m_actor->SetTexture( nullptr );
		}
		m_texture = nullptr;
	}
}

void Mesh::SetTintColor( const types::Color& color ) {
	m_tint_color.enabled = true;
	m_tint_color.color = color;
	UpdateRenderFlags();
}

void Mesh::SetTintAlpha( const float alpha ) {
	m_tint_color.enabled = true;
	m_tint_color.color = types::Color( 1.0f, 1.0f, 1.0f, alpha );
	UpdateRenderFlags();
}

void Mesh::Destroy() {
	if ( m_actor ) {
		RemoveActor( m_actor );
		DELETE( m_actor );
	}
	if ( m_original_mesh ) {
		DELETE( m_original_mesh );
		m_original_mesh = nullptr;
	}

	UIObject::Destroy();
}

void Mesh::Align() {
	UIObject::Align();

	auto c = m_original_mesh->GetVertexCount();

	types::Vec3 coord = {};
	types::Vec2< coord_t > tex_coord = {};

	coord_t mesh_left = 0.0f;
	coord_t mesh_top = 0.0f;
	coord_t mesh_right = 1.0f;
	coord_t mesh_bottom = 1.0f;

	for ( types::mesh::index_t i = 0 ; i < c ; i++ ) {
		m_original_mesh->GetVertexCoord( i, &coord );
		if ( i == 0 || coord.x < mesh_left ) {
			mesh_left = coord.x;
		}
		if ( i == 0 || coord.y < mesh_bottom ) {
			mesh_bottom = coord.y;
		}
		if ( i == 0 || coord.x > mesh_right ) {
			mesh_right = coord.x;
		}
		if ( i == 0 || coord.y > mesh_top ) {
			mesh_top = coord.y;
		}
	}

	bool is_render_mesh = m_mesh->GetType() == types::mesh::Mesh::MT_RENDER;

	if ( is_render_mesh ) {
		// for render meshes y is inversed for some reason, TODO: investigate
		auto tmp = mesh_bottom;
		mesh_bottom = mesh_top;
		mesh_top = tmp;

		if ( m_aspect_ratio_mode != AM_NONE ) {

			float ws = mesh_right - mesh_left;
			float hs = mesh_bottom - mesh_top;
			float as = hs / ws;
			float wd = m_object_area.width;
			float hd = m_object_area.height;
			float ad = hd / wd;

			if ( as != ad ) {
				if ( m_aspect_ratio_mode == AM_SCALE_WIDTH ) {
					float c = ( mesh_right + mesh_left ) / 2;
					ws /= ad * 2;
					mesh_right = c + ws;
					mesh_left = c - ws;
				}
				else if ( m_aspect_ratio_mode == AM_SCALE_HEIGHT ) {
					float c = ( mesh_bottom + mesh_top ) / 2;
					hs *= ad / 2;
					mesh_bottom = c + hs;
					mesh_top = c - hs;
				}
				else {
					THROW( "unknown aspect ratio mode " + std::to_string( m_aspect_ratio_mode ) );
				}
			}
		}
	}

	m_actor->SetPositionX( ClampX( m_object_area.left ) + 1.0f );
	m_actor->SetPositionY( ClampY( m_object_area.top ) - 1.0f );

	bool need_resize = false;
	if ( // TODO: make object areas ints?
		round( m_last_object_area.width ) != round( m_object_area.width ) ||
			round( m_last_object_area.height ) != round( m_object_area.height )
		) {
		//Log( "Resizing because object area has changed ( " + std::to_string( m_last_object_area.width ) + " != " + std::to_string( m_object_area.width ) + " || " + std::to_string( m_last_object_area.height ) + " != " + std::to_string( m_object_area.height ) + " )" );
		m_last_object_area = m_object_area;
		need_resize = true;
	}
	const auto tw = m_texture->GetWidth();
	const auto th = m_texture->GetHeight();
	if ( m_texture && ( m_last_texture_size.x != tw || m_last_texture_size.y != th ) ) {
		//Log( "Resizing because texture size has changed" );
		m_last_texture_size = {
			tw,
			th
		};
		need_resize = true;
	}
	if ( m_last_mesh != m_mesh ) {
		//Log( "Resizing because mesh has been replaced" );
		m_last_mesh = m_mesh;
		need_resize = true;
	}
	if ( m_last_mesh_updated_count != m_original_mesh->UpdatedCount() ) {
		//Log( "Resizing because mesh has changed" );
		m_last_mesh_updated_count = m_original_mesh->UpdatedCount();
		need_resize = true;
	}
	if ( m_force_resize ) {
		//Log( "Resizing because forced" );
		m_force_resize = false;
		need_resize = true;
	}
	if ( m_last_margin != m_margin ) {
		//Log( "Resizing because margin has changed" );
		m_last_margin = m_margin;
		need_resize = true;
	}
	auto* g = g_engine->GetGraphics();
	types::Vec2< size_t > viewport_size = {
		g->GetViewportWidth(),
		g->GetViewportHeight()
	};
	if ( m_last_viewport_size != viewport_size ) {
		//Log( "Resizing because viewport size has changed" );
		m_last_viewport_size = viewport_size;
		need_resize = true;
	}

	if ( need_resize ) {

		// SLOW! Do this only when absolutely needed

		struct {
			util::Clamper< coord_t > x;
			util::Clamper< coord_t > y;
		} object_area_to_mesh_coords;

		object_area_to_mesh_coords.x.SetRange(
			{
				{ UnclampX( mesh_left ), UnclampX( mesh_right ) },
				{ 0.0f,                  m_object_area.width }
			}
		);
		object_area_to_mesh_coords.y.SetRange(
			{
				{ UnclampY( mesh_top ), UnclampY( mesh_bottom ) },
				{ 0.0f,                 m_object_area.height }
			}
		);

		for ( types::mesh::index_t i = 0 ; i < c ; i++ ) {
			m_original_mesh->GetVertexCoord( i, &coord );

			coord.x = ClampX( object_area_to_mesh_coords.x.Clamp( UnclampX( coord.x ) ) );
			coord.y = ClampY( object_area_to_mesh_coords.y.Clamp( UnclampY( coord.y ) ) );

			m_mesh->SetVertexCoord( i, coord );

			ASSERT( m_mesh->GetType() == m_original_mesh->GetType(), "mesh and original mesh have different types" );
			switch ( m_mesh->GetType() ) {
				case types::mesh::Mesh::MT_SIMPLE: {
					auto* from = (types::mesh::Simple*)m_original_mesh;
					auto* to = (types::mesh::Simple*)m_mesh;
					from->GetVertexTexCoord( i, &tex_coord );
					to->SetVertexTexCoord( i, tex_coord );
					break;
				}
				case types::mesh::Mesh::MT_RENDER: {
					auto* from = (types::mesh::Render*)m_original_mesh;
					auto* to = (types::mesh::Render*)m_mesh;
					from->GetVertexTexCoord( i, &tex_coord );
					to->SetVertexTexCoord( i, tex_coord );
					break;
				}
				default: {
					THROW( "unknown mesh type " + std::to_string( m_mesh->GetType() ) );
				}
			}
		}
	}
}

void Mesh::SetAspectRatioMode( const aspect_ratio_mode_t mode ) {
	if ( mode != m_aspect_ratio_mode ) {
		m_aspect_ratio_mode = mode;
		Realign();
	}
}

void Mesh::UpdateRenderFlags() {
	if ( m_actor ) {
		m_actor->SetRenderFlags(
			scene::actor::Actor::RF_IGNORE_CAMERA |
				scene::actor::Actor::RF_IGNORE_LIGHTING |
				scene::actor::Actor::RF_IGNORE_DEPTH |
				scene::actor::Actor::RF_USE_2D_POSITION
		);
		if ( m_tint_color.enabled ) {
			m_actor->SetTintColor( m_tint_color.color );
		}
		Refresh();
	}
}

}
}
