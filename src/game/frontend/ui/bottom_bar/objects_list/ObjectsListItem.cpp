#include "ObjectsListItem.h"

#include "ObjectsList.h"

#include "game/frontend/unit/Unit.h"
#include "ui/object/Label.h"
#include "ui/object/Panel.h"
#include "types/mesh/Mesh.h"

namespace game {
namespace frontend {
namespace ui {

ObjectsListItem::ObjectsListItem( Game* game, ObjectsList* objects_list, TileObject* object )
	: Section( game, "BBObjectsListItem", "BB" )
	, m_objects_list( objects_list )
	, m_object( object ) {
	m_config.no_outer_border = true;
	m_config.no_inner_border = true;
	SetEventContexts( EC_MOUSE | EC_MOUSEMOVE );
}

void ObjectsListItem::Create() {
	Section::Create();

	m_ui_state = m_object->CreateOnBottomBarList( this );

	On(
		::ui::event::EV_MOUSE_OVER, EH( this ) {
			m_objects_list->PreviewObject( m_object );
			return true;
		}
	);
	On(
		::ui::event::EV_MOUSE_OUT, EH( this ) {
			m_objects_list->HideObjectPreview( m_object );
			return true;
		}
	);
	On(
		::ui::event::EV_MOUSE_DOWN, EH( this ) {
			if ( data->mouse.button == ::ui::event::M_LEFT ) {
				m_objects_list->SelectObject( m_object, true );
				return true;
			}
			return false;
		}
	);

	NEW( m_selection_frame, ::ui::object::Panel, "BBObjectsListSelection" );
	AddChild( m_selection_frame );
}

void ObjectsListItem::Destroy() {
	RemoveChild( m_selection_frame );
	m_objects_list->HideObjectPreview( m_object );
	m_object->DestroyOnBottomBarList( this, m_ui_state );

	Section::Destroy();
}

void ObjectsListItem::SelectObject() {
	m_selection_frame->AddStyleModifier( ::ui::M_ACTIVE );
}

void ObjectsListItem::DeselectObject() {
	m_selection_frame->RemoveStyleModifier( ::ui::M_ACTIVE );
}

const TileObject* ObjectsListItem::GetObject() const {
	return m_object;
}

}
}
}
