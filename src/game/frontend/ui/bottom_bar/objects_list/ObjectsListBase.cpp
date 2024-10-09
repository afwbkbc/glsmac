#include "ObjectsListBase.h"

#include "engine/Engine.h"
#include "game/frontend/Game.h"
#include "ui/object/ScrollView.h"
#include "ObjectsListItem.h"
#include "game/frontend/unit/Unit.h"
#include "game/frontend/unit/UnitManager.h"

namespace game {
namespace frontend {
namespace ui {

ObjectsListBase::ObjectsListBase( Game* game )
	: BBSection( game, "ObjectsList" ) {
	//
}

void ObjectsListBase::Create() {
	BBSection::Create();

}

void ObjectsListBase::Destroy() {

	ClearObjects();

	BBSection::Destroy();
}

void ObjectsListBase::ClearObjects() {
	if ( m_body ) {
		if ( m_hovered_object ) {
			ObjectMouseOut( m_hovered_object );
		}
		m_selected_object = nullptr;
		for ( const auto& it : m_items ) {
			m_body->RemoveChild( it.second );
		}
		m_items.clear();
		RemoveChild( m_body );
		m_body = nullptr;
	}
}

void ObjectsListBase::ListObjects( const std::vector< TileObject* >& objects, const size_t selected_unit_id ) {
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
		OnObjectMouseOver( selected_unit ); // ???
	}
}

TileObject* const ObjectsListBase::GetSelectedObject() const {
	return m_selected_object;
}

bool ObjectsListBase::OnObjectMouseOver( TileObject* const object ) {
	return false;
}

bool ObjectsListBase::OnObjectMouseOut( TileObject* const object ) {
	return false;
}

bool ObjectsListBase::OnObjectClick( TileObject* const object ) {
	return false;
}

void ObjectsListBase::SelectObject( TileObject* object, const bool activate_object ) {
	if (
		object != m_selected_object ||
			object->GetType() == TileObject::TOT_BASE // select base again unless popup is open
		) {
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
					OnObjectClick( object );
					m_body->ScrollToItem( item );
				}
				else {
					item->DeselectObject();
				}
			}
		}
	}
}

bool ObjectsListBase::ObjectMouseOver( TileObject* const object ) {
	if ( m_hovered_object ) {
		OnObjectMouseOut( m_hovered_object );
	}
	m_hovered_object = object;
	return OnObjectMouseOver( object );
}

bool ObjectsListBase::ObjectMouseOut( TileObject* const object ) {
	if ( object == m_hovered_object ) {
		m_hovered_object = nullptr;
		return OnObjectMouseOut( object );
	}
	else {
		return true;
	}
}

}
}
}
