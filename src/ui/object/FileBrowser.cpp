#include "FileBrowser.h"

#include "util/FS.h"

namespace ui {
namespace object {

void FileBrowser::Create() {
	UIContainer::Create();
	
	NEW( m_file_list, TextView, "PopupFileList" );
		m_file_list->SetType( TextView::TT_EXTENDED );
		m_file_list->On( UIEvent::EV_CHANGE, EH( this ) {
			m_input->SetValue( data->value.text.ptr ? *data->value.text.ptr : "" );
			return true;
		});
		m_file_list->On( UIEvent::EV_SELECT, EH( this ) {
			ASSERT( data->value.text.ptr, "null text ptr in select event data" );
			SelectItem( *data->value.text.ptr );
			return true;
		});
	AddChild( m_file_list );
	
	NEW( m_input, Input, "PopupFileListInput" );
		m_input->SetZIndex( 0.8f ); // TODO: fix z index bugs
		m_input->SetMaxLength( 128 ); // TODO: make scrollable horizontally when overflowed
	AddChild( m_input );
	
	ChangeDirectory( util::FS::GetCurrentDirectory() );
}

void FileBrowser::Destroy() {
	RemoveChild( m_file_list );
	RemoveChild( m_input );
	
	m_current_directory.clear();
	
	UIContainer::Destroy();
}

void FileBrowser::ChangeDirectory( const std::string& directory ) {
	if ( m_current_directory != directory ) {
		m_current_directory = directory;
	
		Log( "Changing directory to: " + m_current_directory );
		
		m_file_list->Clear();
		
		const std::string cls = "PopupFileListItem";
		
		const auto items = util::FS::ListDirectory( m_current_directory, true );
		
		m_file_list->SetLinesLimit( items.size() + 1 ); // need to fit all items
		
		// directory up
		if ( m_current_directory != "" ) {
			m_file_list->AddLine( "..", cls + "DirUp" );
		}
		
		// directories
		for ( auto& item : items ) {
			if ( util::FS::IsDirectory( item ) ) {
				m_file_list->AddLine( item.substr( m_current_directory.size() + 1 ), cls + "Dir" );
			}
		}
		
		// files
		for ( auto& item : items ) {
			if ( util::FS::IsFile( item ) ) {
				m_file_list->AddLine( item.substr( m_current_directory.size() + 1 ), cls + "File" );
			}
		}
	}
}

void FileBrowser::SelectItem( const std::string& item ) {
	const std::string path = m_current_directory + "/" + item;
	
	if ( item == ".." ) {
		Log( "Selected directory up" );
		const auto pos = m_current_directory.rfind( "/" ); // TODO: check on windows
		if ( pos == std::string::npos ) {
			ChangeDirectory( "/" ); // TODO: check on windows
		}
		else {
			ChangeDirectory( m_current_directory.substr( 0, pos ) );
		}
	}
	else {
		if ( !util::FS::Exists( path ) ) {
			return; // maybe file was removed while application is running // TODO: inotify
		}
		if ( util::FS::IsFile( path ) ) {
			Log( "Selected file: " + path );
		}
		else if ( util::FS::IsDirectory( path ) ) {
			Log( "Selected directory: " + path );
			ChangeDirectory( path );
		}
		//Trigger( UIEvent::EV_SELECT, data );
	}
}

}
}
