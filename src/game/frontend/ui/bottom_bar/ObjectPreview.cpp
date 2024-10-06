#include "ObjectPreview.h"

#include "engine/Engine.h"

#include "game/frontend/unit/Unit.h"
#include "ui/object/Mesh.h"
#include "ui/object/Label.h"
#include "types/mesh/Mesh.h"

namespace game {
namespace frontend {
namespace ui {

ObjectPreview::ObjectPreview( Game* game )
	: BBSection( game, "ObjectPreview" ) {}

void ObjectPreview::Create() {
	BBSection::Create();

}

void ObjectPreview::Destroy() {
	HideObjectPreview();

	BBSection::Destroy();
}

void ObjectPreview::PreviewObject( const TileObject* object ) {
	HideObjectPreview();
	m_object = object;
	m_ui_state = m_object->CreateOnBottomBarPreview( this );
}

void ObjectPreview::HideObjectPreview() {
	if ( m_object ) {
		m_object->DestroyOnBottomBarPreview( this, m_ui_state );
		m_object = nullptr;
	}
}

}
}
}
