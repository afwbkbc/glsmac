#include "FileBrowser.h"

#include "util/FS.h"

namespace ui {
namespace object {

void FileBrowser::SetExistingOnly( bool existing_only ) {
	m_existing_only = existing_only;
}

void FileBrowser::Create() {
	UIContainer::Create();
	
	NEW( m_file_list, TextView, "PopupFileList" );
		m_file_list->SetType( TextView::TT_EXTENDED );
		m_file_list->On( UIEvent::EV_CHANGE, EH( this ) {
			m_input->SetValue( data->value.list_item.text_ptr ? *data->value.list_item.text_ptr : "" );
			return true;
		});
		m_file_list->On( UIEvent::EV_SELECT, EH( this ) {
			ASSERT( data->value.list_item.text_ptr, "null text ptr in select event data" );
			SelectItem( *data->value.list_item.text_ptr );
			return true;
		});
	AddChild( m_file_list );
	
	NEW( m_input, Input, "PopupFileListInput" );
		m_input->SetZIndex( 0.8f ); // TODO: fix z index bugs
		m_input->SetMaxLength( 128 ); // TODO: make scrollable horizontally when overflowed
	AddChild( m_input );
	
	On( UIEvent::EV_KEY_DOWN, EH( this ) {
		if ( !data->key.modifiers ) {
			switch ( data->key.code ) {
				case UIEvent::K_UP: {
					m_file_list->SelectPreviousLine();
					break;
				}
				case UIEvent::K_DOWN: {
					m_file_list->SelectNextLine();
					break;
				}
				case UIEvent::K_PAGEUP: {
					m_file_list->SelectPreviousPage();
					break;
				}
				case UIEvent::K_PAGEDOWN: {
					m_file_list->SelectNextPage();
					break;
				}
				case UIEvent::K_HOME: {
					m_file_list->SelectFirstLine();
					break;
				}
				case UIEvent::K_END: {
					m_file_list->SelectLastLine();
					break;
				}
				case UIEvent::K_ENTER: {
					SelectCurrentItem();
					break;
				}
				default: {
					return false;
				}
			}
			return true;
		}
		return false;
	});
	
	ChangeDirectory( util::FS::GetCurrentDirectory() );
}

void FileBrowser::Destroy() {
	RemoveChild( m_file_list );
	RemoveChild( m_input );
	
	m_current_directory.clear();
	
	UIContainer::Destroy();
}

const std::string& FileBrowser::GetSelectedItem() const {
	ASSERT( m_input, "file browser input not initialized" );
	return m_input->GetValue();
}

void FileBrowser::SelectCurrentItem() {
	SelectItem( GetSelectedItem() );
}

void FileBrowser::ChangeDirectory( const std::string& directory ) {
	if ( m_current_directory != directory ) {
		m_current_directory = directory;
	
		//Log( "Changing directory to: " + m_current_directory );
		
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
	const std::string sep = util::FS::GetPathSeparator();
	const std::string path = m_current_directory + sep + item;
	
	if ( item == ".." ) {
		//Log( "Selected directory up" );
		const auto pos = m_current_directory.rfind( sep ); // TODO: check on windows
		if ( pos == std::string::npos ) {
			ChangeDirectory( sep ); // TODO: check on windows
		}
		else {
			ChangeDirectory( m_current_directory.substr( 0, pos ) );
		}
		if ( !m_selection_stack.empty() ) {
			m_file_list->SelectLine( m_selection_stack.back() );
			m_selection_stack.pop_back();
		}
	}
	else {
		if ( m_existing_only && !util::FS::Exists( path ) ) {
			return; // maybe file was removed while application is running // TODO: inotify
		}
		if ( util::FS::IsDirectory( path ) ) {
			//Log( "Selected directory: " + path );
			m_selection_stack.push_back( item );
			ChangeDirectory( path );
		}
		else if (
			util::FS::IsFile( path ) ||
			( !m_existing_only && !util::FS::Exists( path ) )
		) {
			//Log( "Selected file: " + path );
			UIEvent::event_data_t data = {};
			data.value.list_item.text_ptr = &path;
			Trigger( UIEvent::EV_SELECT, &data );
			
		}
	}
}

}
}
