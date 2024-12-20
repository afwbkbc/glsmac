#include "Surface.h"

#include "types/mesh/Rectangle.h"
#include "scene/actor/Mesh.h"

#include "ui/UI.h"
#include "ui/Geometry.h"
#include "scene/Scene.h"

#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"
#include "graphics/Graphics.h"

namespace ui {
namespace dom {

Surface::Surface( DOM_ARGS_T )
	: Area( DOM_ARGS_PASS_T ) {

	std::string cls = "UI::Mesh::Surface";
	NEW( m_mesh, types::mesh::Rectangle );
	NEW( m_actor, scene::actor::Mesh, cls, m_mesh );
	m_actor->SetTexture(
		g_engine->GetTextureLoader()->GetColorTexture(
			{
				1.0f,
				1.0f,
				1.0f,
				1.0f
			}
		)
	);
	m_geometry->SetMesh( m_mesh );
	m_ui->m_scene->AddActor( m_actor );

}

Surface::~Surface() {
	m_ui->m_scene->RemoveActor( m_actor );
	m_geometry->SetMesh( nullptr );
	delete m_actor;
}

}
}
