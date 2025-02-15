#include "ObjectsListItem.h"

#include "ObjectsListBase.h"

#include "game/frontend/unit/Unit.h"
#include "ui_legacy/object/Label.h"
#include "ui_legacy/object/Panel.h"
#include "types/mesh/Mesh.h"

namespace game {
namespace frontend {
namespace ui_legacy {

ObjectsListItem::ObjectsListItem( Game* game, ObjectsListBase* objects_list, TileObject* object )
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
		::ui_legacy::event::EV_MOUSE_OVER, EH( this ) {
			return m_objects_list->ObjectMouseOver( m_object );
		}
	);
	On(
		::ui_legacy::event::EV_MOUSE_OUT, EH( this ) {
			return m_objects_list->ObjectMouseOut( m_object );
		}
	);
	On(
		::ui_legacy::event::EV_MOUSE_DOWN, EH( this ) {
			if ( data->mouse.button == ::ui_legacy::event::M_LEFT ) {
				m_objects_list->SelectObject( m_object, true );
				return true;
			}
			return false;
		}
	);

	NEW( m_selection_frame, ::ui_legacy::object::Panel, "BBObjectsListSelection" );
	AddChild( m_selection_frame );
}

void ObjectsListItem::Destroy() {
	RemoveChild( m_selection_frame );
	m_objects_list->ObjectMouseOut( m_object );
	m_object->DestroyOnBottomBarList( this, m_ui_state );

	Section::Destroy();
}

void ObjectsListItem::SelectObject() {
	m_selection_frame->AddStyleModifier( ::ui_legacy::M_ACTIVE );
}

void ObjectsListItem::DeselectObject() {
	m_selection_frame->RemoveStyleModifier( ::ui_legacy::M_ACTIVE );
}

const TileObject* ObjectsListItem::GetObject() const {
	return m_object;
}

}
}
}
