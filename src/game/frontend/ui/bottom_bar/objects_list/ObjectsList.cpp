#include "ObjectsList.h"

#include "game/frontend/ui/bottom_bar/ObjectPreview.h"

namespace game {
namespace frontend {
namespace ui {

ObjectsList::ObjectsList( Game* game, ObjectPreview* object_preview )
	: ObjectsListBase( game )
	, m_object_preview( object_preview ) {}

bool ObjectsList::OnObjectMouseOver( TileObject* const object ) {
	PreviewObject( object );
	return true;
}

bool ObjectsList::OnObjectMouseOut( TileObject* const object ) {
	HideObjectPreview( object );
	return true;
}

void ObjectsList::PreviewObject( TileObject* object ) {
	if ( object != m_previewing_object ) {
		m_previewing_object = object;
		m_object_preview->PreviewObject( object );
	}
}

void ObjectsList::HideObjectPreview( TileObject* object ) {
	auto* selected_object = GetSelectedObject();
	if ( object != m_previewing_object ) {
		if ( m_previewing_object ) {
			m_object_preview->PreviewObject( m_previewing_object );
		}
		else if ( selected_object ) {
			m_object_preview->PreviewObject( selected_object );
		}
		else if ( object == m_previewing_object ) { // TODO: test
			m_object_preview->HideObjectPreview();
		}
	}
	if ( object == m_previewing_object ) {
		if ( selected_object ) {
			m_object_preview->PreviewObject( selected_object );
		}
		m_previewing_object = nullptr;
	}
}

}
}
}
