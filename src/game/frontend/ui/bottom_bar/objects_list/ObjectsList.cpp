#include "ObjectsList.h"

#include "engine/Engine.h"
#include "game/frontend/Game.h"
#include "ui/object/ScrollView.h"
#include "game/frontend/ui/bottom_bar/ObjectPreview.h"
#include "ObjectsListItem.h"
#include "game/frontend/unit/Unit.h"
#include "game/frontend/unit/UnitManager.h"

namespace game {
namespace frontend {
namespace ui {

ObjectsList::ObjectsList( Game* game, ObjectPreview* object_preview )
	: BBSection( game, "ObjectsList" )
	, m_object_preview( object_preview ) {}

void ObjectsList::Create() {
	BBSection::Create();

}

void ObjectsList::Destroy() {

	ClearObjects();

	BBSection::Destroy();
}

void ObjectsList::ClearObjects() {
	if ( m_body ) {
		m_object_preview->HideObjectPreview();
		m_selected_object = nullptr;
		for ( const auto& it : m_items ) {
			m_body->RemoveChild( it.second );
		}
		m_items.clear();
		RemoveChild( m_body );
		m_body = nullptr;
	}
}

void ObjectsList::ListObjects( const std::vector< TileObject* >& objects, const size_t selected_unit_id ) {
	const auto last_scroll_x = m_body
		? m_body->GetScrollX()
		: 0;
	ClearObjects(); // TODO: avoid redraws?
	NEW( m_body, ::ui::object::ScrollView, ::ui::object::ScrollView::ST_HORIZONTAL_INLINE );
	m_body->SetSticky( false );
	m_body->SetScrollSpeed( 70 );
	AddChild( m_body );
	float left = 0;
	unit::Unit* selected_unit = nullptr;
	for ( const auto& object : objects ) {
		NEWV( item, ObjectsListItem, m_game, this, object );
		item->SetLeft( left );
		m_body->AddChild( item );
		if (
			(
				selected_unit_id &&
					object->GetType() == TileObject::TOT_UNIT &&
					( (unit::Unit*)object )->GetId() == selected_unit_id // select unit as requested
			) ||
				(
					!selected_unit_id && m_items.empty() // auto-select first unit by default
				)
			) {
			selected_unit = (unit::Unit*)object;
		}
		m_items.insert(
			{
				object,
				item
			}
		);
		left += item->GetWidth();
	}
	m_body->SetScrollX( last_scroll_x );
	if ( selected_unit ) {
		SelectObject( selected_unit, false );
		m_object_preview->PreviewObject( selected_unit );
	}
}

void ObjectsList::PreviewObject( TileObject* object ) {
	if ( object != m_previewing_object ) {
		m_previewing_object = object;
		m_object_preview->PreviewObject( object );
	}
}

void ObjectsList::HideObjectPreview( TileObject* object ) {
	if ( object != m_previewing_object ) {
		if ( m_previewing_object ) {
			m_object_preview->PreviewObject( m_previewing_object );
		}
		else if ( m_selected_object ) {
			m_object_preview->PreviewObject( m_selected_object );
		}
		else if ( object == m_previewing_object ) { // TODO: test
			m_object_preview->HideObjectPreview();
		}
	}
	if ( object == m_previewing_object ) {
		m_previewing_object = nullptr;
	}
}

void ObjectsList::SelectObject( TileObject* object, const bool activate_object ) {
	if ( object != m_selected_object ) {
		m_selected_object = object;
		bool was_selected = true;
		if ( activate_object ) {
			was_selected = m_selected_object->OnBottomBarListActivate( m_game );
		}
		if ( was_selected ) {
			for ( auto& it : m_items ) {
				const auto& item = it.second;
				if ( it.first == object ) {
					item->SelectObject();
					m_body->ScrollToItem( item );
				}
				else {
					item->DeselectObject();
				}
			}
		}
	}
}

}
}
}
