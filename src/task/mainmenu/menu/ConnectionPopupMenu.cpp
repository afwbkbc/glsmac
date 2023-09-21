#include "ConnectionPopupMenu.h"

#include "lobby/Lobby.h"

namespace task {
namespace mainmenu {

ConnectionPopupMenu::ConnectionPopupMenu( MainMenu *mainmenu, const std::string& title )
	: PopupMenu( mainmenu, title )
{
	//
}

void ConnectionPopupMenu::Iterate() {
	if ( m_connection ) {
		m_connection->Iterate();
	}
}

void ConnectionPopupMenu::SetConnection( ::game::connection::Connection* connection ) {
	ASSERT( !m_connection, "connection already set" );
	m_connection = connection;
	m_connection->m_on_connect = [ this ] () -> void {
		Show();
		NEWV( menu, lobby::Lobby, m_mainmenu, m_connection );
		m_connection = nullptr;
		NextMenu( menu );
	};
	m_connection->m_on_error = [ this ] ( const std::string& message ) -> void {
		DELETE( m_connection );
		m_connection = nullptr;
		Show();
		MenuError( message );
	};
	m_connection->m_on_cancel = [ this ] () -> void {
		DELETE( m_connection );
		m_connection = nullptr;
		Show();
	};
	m_connection->Connect();
}

}
}
