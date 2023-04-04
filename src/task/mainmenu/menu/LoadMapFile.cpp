#include "LoadMapFile.h"

#include "engine/Engine.h"

#include "task/game/map/Map.h"
#include "util/FS.h"

#include "Host.h"
#include "Join.h"

namespace task {
namespace mainmenu {

LoadMapFile::LoadMapFile( MainMenu* mainmenu )
	: PopupMenu( mainmenu, "LOAD MAP" )
{
	Align();
	SetFlags( { PF_HAS_OK, PF_HAS_CANCEL } );
}

LoadMapFile::~LoadMapFile() {
}

void LoadMapFile::Show() {
	PopupMenu::Show();
	
	NEW( m_section, Section, "PopupSection" );
	m_body->AddChild( m_section );
	
	NEW( m_file_browser, ::ui::object::FileBrowser );
		// TODO: determine position from style
		m_file_browser->SetTop( 8 );
		m_file_browser->SetLeft( 6 );
		m_file_browser->SetBottom( 8 );
		m_file_browser->SetRight( 6 );
		m_file_browser->SetDefaultDirectory( util::FS::GetAbsolutePath( task::game::map::Map::s_consts.fs.default_map_directory ) );
		m_file_browser->SetFileExtension( task::game::map::Map::s_consts.fs.default_map_extension );
		m_file_browser->On( UIEvent::EV_SELECT, EH( this ) {
			const auto& path = m_file_browser->GetSelectedFile();
			if ( !util::FS::FileExists( path ) ) {
				g_engine->GetUI()->Error(
					"File " + util::FS::GetBaseName( path ) + " does not exist!"
				);
			}
			else {
				ASSERT( util::FS::IsAbsolutePath( path ), "path must be absolute" );
				m_mainmenu->m_settings.local.map_file = path;
				m_mainmenu->StartGame();
			}
			return true;
		});
	m_section->AddChild( m_file_browser );
}

void LoadMapFile::Align() {
	PopupMenu::Align();
	
	const auto* g = g_engine->GetGraphics();
	
	SetWidth( 500 );
	SetHeight( std::min< size_t >( 600, g_engine->GetGraphics()->GetViewportHeight() - 40 ) );
	
	Resize();
}

void LoadMapFile::Hide() {
		m_section->RemoveChild( m_file_browser );
	m_body->RemoveChild( m_section );
	
	PopupMenu::Hide();
}

void LoadMapFile::OnNext() {
/*	const auto value = m_choices->GetValue();
	MenuObject* menu = nullptr;
	if ( value == "Host new game" ) {
		m_mainmenu->m_settings.local.network_role = LocalSettings::NR_SERVER;
		NEW( menu, Host, m_mainmenu );
		NextMenu( menu );
	}
	else {
		m_mainmenu->m_settings.local.network_role = LocalSettings::NR_CLIENT;
		NEW( menu, Join, m_mainmenu );
		NextMenu( menu );
	}*/

	m_file_browser->SelectCurrentValueOrItem();
}

/*const std::string LoadMapFile::GetChoice() const {
	return m_choices->GetValue();
}

void LoadMapFile::SetChoice( const std::string& choice ) {
	m_choices->SetValue( choice );
}*/


}
}
