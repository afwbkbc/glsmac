#include "Text.h"

#include "ui/UI.h"
#include "scene/actor/Text.h"

namespace ui {
namespace geometry {

Text::Text( const UI* const ui, Geometry* const parent )
	: Geometry( ui, parent, GT_TEXT ) {

}

void Text::SetActor( scene::actor::Text* const actor ) {
	if ( actor != m_actor ) {
		m_actor = actor;
		UpdateImpl();
	}
}

void Text::UpdateImpl() {
	if ( m_actor ) {
		m_actor->SetPosition(
			{
				m_ui->ClampX( std::round( m_area.left ) ),
				m_ui->ClampY( std::round( m_area.bottom ) ),
				m_area.zindex,
			}
		);
	}
}

}
}
