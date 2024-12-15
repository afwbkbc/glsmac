#include "ObjectPreview.h"

#include "engine/Engine.h"

#include "game/frontend/unit/Unit.h"
#include "ui_legacy/object/Mesh.h"
#include "ui_legacy/object/Label.h"
#include "types/mesh/Mesh.h"

namespace game {
namespace frontend {
namespace ui_legacy {

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
