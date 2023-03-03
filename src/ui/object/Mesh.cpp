#include "Mesh.h"

#include "util/Clamper.h"

using namespace types;

namespace ui {
namespace object {

Mesh::Mesh( const std::string& class_name )
	: UIObject( class_name )
{
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
		Realign();
	}
}

void Mesh::SetTexture( const types::Texture* texture ) {
	if ( texture != m_texture ) {
		if ( m_texture && m_texture ) {
			Log( "Changing texture from " + m_texture->m_name + " to " + texture->m_name );
		}
		else if ( texture ) {
			Log( "Setting texture " + texture->m_name );
		}
		else if ( m_texture ) {
			Log( "Clearing texture " + texture->m_name );
		}
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

void Mesh::SetTintColor( const types::Color color ) {
	m_tint_color.enabled = true;
	m_tint_color.color = color;
	UpdateRenderFlags();
}

void Mesh::Create() {
	UIObject::Create();
	
/* not needed?
	if ( m_actor && m_texture ) {
		m_actor->SetTexture( m_texture );
	}
 */
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

void Mesh::Draw() {
	UIObject::Draw();

	// looks like this isn't needed. keep this code for a while in case some bugs happen without it
	/*if ( m_texture ) {
		m_actor->SetTexture( m_texture );
	}*/
}

void Mesh::Align() {
	UIObject::Align();
	
	auto c = m_original_mesh->GetVertexCount();
	
	Vec3 coord = {};
	Vec2< coord_t > tex_coord = {};
	
	coord_t mesh_left, mesh_top, mesh_right, mesh_bottom;
	
	for ( types::mesh::Mesh::index_t i = 0 ; i < c ; i++ ) {
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
	struct {
		util::Clamper<coord_t> x;
		util::Clamper<coord_t> y;
	} object_area_to_mesh_coords;
	
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
			float wd = m_object_area.right - m_object_area.left;
			float hd = m_object_area.bottom - m_object_area.top;
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
					ASSERT( false, "unknown aspect ratio mode " + std::to_string( m_aspect_ratio_mode ) );
				}
			}
			
		}
	}
	
	object_area_to_mesh_coords.x.SetRange( UnclampX( mesh_left ), UnclampX( mesh_right ), m_object_area.left, m_object_area.right );
	object_area_to_mesh_coords.y.SetRange( UnclampY( mesh_top ), UnclampY( mesh_bottom ), m_object_area.top, m_object_area.bottom );
	
	for ( types::mesh::Mesh::index_t i = 0 ; i < c ; i++ ) {
		m_original_mesh->GetVertexCoord( i, &coord );
		
		coord.x = ClampX( object_area_to_mesh_coords.x.Clamp( UnclampX( coord.x ) ) );
		coord.y = ClampY( object_area_to_mesh_coords.y.Clamp( UnclampY( coord.y ) ) );
		
		m_mesh->SetVertexCoord( i, coord );

		ASSERT( m_mesh->GetType() == m_original_mesh->GetType(), "mesh and original mesh have different types" );
		switch ( m_mesh->GetType() ) {
			case types::mesh::Mesh::MT_SIMPLE: {
				auto *from = (types::mesh::Simple*) m_original_mesh;
				auto *to = (types::mesh::Simple*) m_mesh;
				from->GetVertexTexCoord( i, &tex_coord );
				to->SetVertexTexCoord( i, tex_coord );
				break;
			}
			case types::mesh::Mesh::MT_RENDER: {
				auto *from = (types::mesh::Render*) m_original_mesh;
				auto *to = (types::mesh::Render*) m_mesh;
				from->GetVertexTexCoord( i, &tex_coord );
				to->SetVertexTexCoord( i, tex_coord );
				break;
			}
			default: {
				ASSERT( false, "unknown mesh type " + std::to_string( m_mesh->GetType() ) );
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
		auto render_flags = scene::actor::Mesh::RF_IGNORE_CAMERA | scene::actor::Mesh::RF_IGNORE_LIGHTING | scene::actor::Mesh::RF_IGNORE_DEPTH;
		if ( m_tint_color.enabled ) {
			render_flags |= scene::actor::Mesh::RF_USE_TINT;
			m_actor->SetTintColor( m_tint_color.color );
		}
		m_actor->SetRenderFlags( render_flags );
	}
}

}
}
