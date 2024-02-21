#include "FileBrowser.h"

#include "util/FS.h"

namespace ui {
namespace object {

void FileBrowser::SetDefaultDirectory( const std::string& default_directory ) {
	ASSERT( !m_file_list, "can't change default directory after initialization" );
	ASSERT( util::FS::IsAbsolutePath( default_directory ), "default directory must be absolute path" );
	m_default_directory = default_directory;
}

// force specific file extension
void FileBrowser::SetFileExtension( const std::string& file_extension ) {
	ASSERT( !m_file_list, "can't change file extension after initialization" );
	if ( !file_extension.empty() ) {
		ASSERT( file_extension[ 0 ] == '.', "file extension must start with ." );
	}
	m_file_extension = file_extension;
}

void FileBrowser::SetDefaultFilename( const std::string& default_filename ) {
	ASSERT( !m_file_list, "can't change default filename after initialization" );
	m_default_filename = default_filename;
}

void FileBrowser::SetExistingOnly( bool existing_only ) {
	m_existing_only = existing_only;
}

void FileBrowser::Create() {
	ASSERT( !m_default_directory.empty(), "file browser default directory not set" );

	util::FS::CreateDirectoryIfNotExists( m_default_directory );

	UIContainer::Create();

	NEW( m_file_list, TextView, "PopupFileList" );
	m_file_list->SetType( TextView::TT_EXTENDED );
	m_file_list->On(
		UIEvent::EV_CHANGE, EH( this ) {
			const std::string v = data->value.change.text
				? *data->value.change.text
				: "";
			m_input->SetHint( v );
			if ( !m_is_typing ) {
				m_input->SetValue(
					v != util::FS::GetUpDirString()
						? v
						: ""
				);
			}
			return true;
		}
	);
	m_file_list->On(
		UIEvent::EV_SELECT, EH( this ) {
			ASSERT( data->value.change.text, "null text ptr in select event data" );
			SelectItem( *data->value.change.text );
			return true;
		}
	);
	AddChild( m_file_list );

	NEW( m_input, Input, "PopupFileListInput" );
	m_input->SetZIndex( 0.8f ); // TODO: fix z index bugs`
	m_input->SetMaxLength( 128 ); // TODO: make scrollable horizontally when overflowed
	m_input->On(
		UIEvent::EV_CHANGE, EH( this ) {
			ASSERT( data->value.change.text, "input changed but test ptr is null" );
			m_is_typing = true; // prevent input value change while typing
			if ( m_file_list->SelectByMask( *data->value.change.text ) == m_input->GetValue().size() ) { // full match
				m_input->SetHint( m_file_list->GetSelectedText() );
				m_input->SetValue( m_file_list->GetSelectedText().substr( 0, m_input->GetValue().size() ) ); // fix case
			}
			else {
				// no full match, hint is invalid
				const auto& v = m_input->GetValue();
				bool hint_set = false;
				if (
					!v.empty() &&
						!m_file_extension.empty()
					) {
					// hint extension if possible/applicable
					const auto pos = v.find( '.' );
					if ( pos == std::string::npos ) {
						m_input->SetHint( v + m_file_extension );
						hint_set = true;
					}
					else {
						// compare ending of input with beginning of extension
						const auto ext_len = v.size() - pos;
						if ( ext_len <= m_file_extension.size() && !memcmp( v.c_str() + pos, m_file_extension.c_str(), ext_len ) ) {
							// matching extension is typed
							m_input->SetHint( v + m_file_extension.substr( ext_len ) );
							hint_set = true;
						}
					}
				}

				if ( !hint_set ) {
					m_input->SetHint( "" );
				}
			}
			m_is_typing = false;
			return true;
		}
	);
	AddChild( m_input );

	On(
		UIEvent::EV_KEY_DOWN, EH( this ) {
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
					case UIEvent::K_TAB: {
						const auto& hint = m_input->GetHint();
						if ( !hint.empty() ) {
							m_input->SetValue( hint );
						}
						break;
					}
					case UIEvent::K_ENTER: {
						SelectCurrentValue();
						break;
					}
					default: {
						return false;
					}
				}
				return true;
			}
			return false;
		}
	);

	m_selection_stack.clear();
	size_t oldpos = 0, pos = 0;
	const auto sep = util::FS::PATH_SEPARATOR;
	while ( ( pos = m_default_directory.find( sep, oldpos ) ) != std::string::npos ) {
		m_selection_stack.push_back( m_default_directory.substr( oldpos, pos - oldpos ) );
		//Log( "Path part: " + m_selection_stack.back() );
		oldpos = pos + 1;
	}
	m_selection_stack.push_back( m_default_directory.substr( oldpos ) );
	//Log( "Path part: " + m_selection_stack.back() );

	ChangeDirectory( m_default_directory );

	m_input->SetValue( m_default_filename );
	m_file_list->SelectLine( m_input->GetValue() );
}

void FileBrowser::Destroy() {
	RemoveChild( m_file_list );
	RemoveChild( m_input );

	m_current_directory.clear();

	UIContainer::Destroy();
}

const std::string FileBrowser::GetSelectedFile() const {
	const auto sep = util::FS::PATH_SEPARATOR;
	const auto& value = m_input->GetValue();
	return
#ifdef _WIN32
		( m_current_directory.empty() && util::FS::IsWindowsDriveLabel( value ) )
			? value
			: m_current_directory + sep + value
#else
		m_current_directory + sep + value
#endif
		;
}

void FileBrowser::SelectCurrentValue() {
	SelectItem( m_input->GetValue() );
}

void FileBrowser::SelectCurrentValueOrItem() {
	const auto sep = util::FS::PATH_SEPARATOR;
#ifdef _WIN32
	if ( !m_current_directory.empty() )
#endif
	{
		const auto& value = m_input->GetValue();
		const auto& item = m_file_list->GetSelectedText();
		if (
			!util::FS::FileExists( m_current_directory + sep + value ) &&
				util::FS::FileExists( m_current_directory + sep + item )
			) {
			Log( "Updating value from list: " + value + " -> " + item );
			m_input->SetValue( item );
			m_input->SetHint( "" );
		}
	}
	SelectCurrentValue();
}

void FileBrowser::ChangeDirectory( std::string directory ) {

	ASSERT( util::FS::IsAbsolutePath( directory ), "directory must be absolute path" );

	// remove trailing slash
	const char sep = util::FS::PATH_SEPARATOR;
	while ( !directory.empty() && directory[ directory.size() - 1 ] == sep ) {
		directory.pop_back();
	}

	if ( m_current_directory != directory ) {
		m_current_directory = directory;

		Log( "Changing directory to: " + m_current_directory );

		m_file_list->Clear();

		const std::string cls = "PopupFileListItem";

		std::vector< std::string > items;
		uint8_t item_prefix_size = 1;
#ifdef _WIN32
		if ( m_current_directory.empty() ) {
			items = util::FS::GetWindowsDrives();
			item_prefix_size = 0;
		}
		else
#endif
		{
			items = util::FS::ListDirectory( m_current_directory, true );
		}

		m_file_list->SetLinesLimit( items.size() + 1 ); // need to fit all items

		// directory up
		if ( m_current_directory != "" ) {
			m_file_list->AddLine( util::FS::GetUpDirString(), cls + "DirUp" );
		}

		// directories
		for ( auto& item : items ) {
			//Log( "  Directory item: " + item );
			if ( util::FS::IsDirectory( item ) ) {
				m_file_list->AddLine( item.substr( m_current_directory.size() + item_prefix_size ), cls + "Dir" );
			}
		}

		// files
		for ( auto& item : items ) {
			if ( util::FS::IsFile( item ) ) {
				if ( !m_file_extension.empty() && util::FS::GetExtension( item ) != m_file_extension ) {
					continue; // wrong/missing extension
				}
				m_file_list->AddLine( item.substr( m_current_directory.size() + item_prefix_size ), cls + "File" );
			}
		}

		m_input->SetValue( "" );
		m_input->SetHint( util::FS::GetUpDirString() );
	}
}

void FileBrowser::SelectItem( std::string item ) {

	if ( item.empty() ) {
		item = m_file_list->GetSelectedText();
		m_input->SetValue( item );
	}

	const auto sep = util::FS::PATH_SEPARATOR;
	std::string path = GetSelectedFile();

	if ( item == util::FS::GetUpDirString() ) {
		//Log( "Selected directory up" );
		const auto pos = m_current_directory.rfind( sep );
		if ( pos == std::string::npos ) {
			ChangeDirectory( std::string( 1, sep ) );
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
		else {
			if ( !m_file_extension.empty() && util::FS::GetExtension( path ) == "" ) {
				path += m_file_extension;
			}
			if (
#ifdef _WIN32
				!m_current_directory.empty() && // can't read/write files outside of drives
#endif
				(
					util::FS::IsFile( path ) ||
						(
							!m_existing_only &&
								!util::FS::Exists( path )
						)
				)
				) {
				//Log( "Selected file: " + path );
				UIEvent::event_data_t data = {};
				data.value.change.text = &path;
				Trigger( UIEvent::EV_SELECT, &data );

			}
		}
	}
}

}
}
