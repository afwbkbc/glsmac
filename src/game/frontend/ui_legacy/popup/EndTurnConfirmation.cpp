#include "EndTurnConfirmation.h"

#include "ui_legacy/object/Section.h"
#include "ui_legacy/object/Label.h"

namespace game {
namespace frontend {
namespace ui_legacy {
namespace popup {

EndTurnConfirmation::EndTurnConfirmation( Game* game, const ::ui_legacy::ui_handler_t on_confirm )
	: Popup( game )
	, m_on_confirm( on_confirm ) {
	SetTitleText( "OPERATIONS DIRECTOR" );
	SetWidth( 500 );
	SetHeight( 300 );
}

void EndTurnConfirmation::Create() {
	Popup::Create();

	NEW( m_message_section, ::ui_legacy::object::Section, SubClass( "InnerFrame" ) );
	m_message_section->SetAlign( ::ui_legacy::ALIGN_TOP );
	m_message_section->SetLeft( 6 );
	m_message_section->SetRight( 6 );
	m_message_section->SetTop( 34 );
	m_message_section->SetHeight( 54 );
	AddChild( m_message_section );

	// TODO: multiline labels? word wrap?
	NEW( m_message_labels.first, ::ui_legacy::object::Label, SubClass( "Label" ) );
	m_message_labels.first->SetText( "Some of our units have not yet moved this turn." );
	m_message_labels.first->SetLeft( 4 );
	m_message_labels.first->SetTop( 4 );
	m_message_section->AddChild( m_message_labels.first );
	NEW( m_message_labels.second, ::ui_legacy::object::Label, SubClass( "Label" ) );
	m_message_labels.second->SetText( "Do you really want to end the turn now?" );
	m_message_labels.second->SetLeft( 4 );
	m_message_labels.second->SetTop( 24 );
	m_message_section->AddChild( m_message_labels.second );

	NEW( m_choices, ::ui_legacy::object::NumChoiceList, "PopupButtonList" );
	m_choices->SetZIndex( 0.6f ); // TODO: fix z index bugs
	m_choices->SetTop( 87 );
	m_choices->SetMargin( 3 );
	m_choices->SetChoicesV(
		{
			"Cancel.",
			"Yes, end the turn."
		}
	);
	m_choices->On(
		::ui_legacy::event::EV_SELECT, EH( this ) {
			SelectChoice();
			return true;
		}
	);
	AddChild( m_choices );
	m_choices->SetValue( 2 );

}

void EndTurnConfirmation::Destroy() {

	m_message_section->RemoveChild( m_message_labels.first );
	m_message_section->RemoveChild( m_message_labels.second );
	RemoveChild( m_message_section );
	RemoveChild( m_choices );

	Popup::Destroy();
}

void EndTurnConfirmation::Align() {
	Popup::Align();
}

void EndTurnConfirmation::SelectChoice() {
	if ( m_choices->GetValueString() == "Yes, end the turn." ) {
		m_on_confirm();
		CloseNow();
	}
	else {
		Close();
	}
}

}
}
}
}
