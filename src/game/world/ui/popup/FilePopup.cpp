#include "FilePopup.h"

#include "util/FS.h"

#include "engine/Engine.h"

namespace game {
namespace world {
namespace ui {
namespace popup {

FilePopup::FilePopup(
	World* world,
	const std::string& title,
	const file_mode_t file_mode,
	const std::string& default_directory,
	const std::string& file_extension,
	const std::string& default_filename
)
	: OkCancelPopup( world )
	, m_title( title )
	, m_file_mode( file_mode )
	, m_default_directory( util::FS::GetAbsolutePath( default_directory ) )
	, m_file_extension( file_extension )
	, m_default_filename( default_filename )
{
	// TODO: load from styles?
	SetWidth( 400 );
	SetHeight( 600 );
	SetTitleText( m_title );
}

void FilePopup::Create() {
	OkCancelPopup::Create();
	
	NEW( m_file_browser, ::ui::object::FileBrowser );
		// TODO: determine position from style
		m_file_browser->SetTop( 36 );
		m_file_browser->SetLeft( 6 );
		m_file_browser->SetBottom( 66 );
		m_file_browser->SetRight( 6 );
		m_file_browser->SetDefaultDirectory( m_default_directory );
		m_file_browser->SetFileExtension( m_file_extension );
		m_file_browser->SetDefaultFileName( m_default_filename );
		m_file_browser->On( UIEvent::EV_SELECT, EH( this ) {
			const auto& path = *data->value.text.ptr;
			switch ( m_file_mode ) {
				case FM_READ: {
					if ( !util::FS::FileExists( path ) ) {
						g_engine->GetUI()->Error(
							"File " + util::FS::GetBaseName( path ) + " does not exist!"
						);
					}
					else {
						OnFileSelect( path );
					}
					break;
				}
				case FM_WRITE: {
					if ( util::FS::FileExists( path ) ) {
						g_engine->GetUI()->Confirm(
							"File " + util::FS::GetBaseName( path ) + " already exists! Overwrite?",
							UH( this, path ) {
								OnFileSelect( path );
							}
						);
					}
					else {
						OnFileSelect( path );
					}
					break;
				}
				default: {
					ASSERT( false, "unknown file mode " + std::to_string( m_file_mode ) );
				}
			}
			return true;
		});
	AddChild( m_file_browser );
}

void FilePopup::Destroy() {
	RemoveChild( m_file_browser );
	
	OkCancelPopup::Destroy();
}

void FilePopup::OnOk() {
	switch ( m_file_mode ) {
		case FM_READ: {
			m_file_browser->SelectCurrentValue();
			break;
		}
		case FM_WRITE: {
			m_file_browser->SelectCurrentValue();
			break;
		}
		default: {
			ASSERT( false, "unknown file mode " + std::to_string( m_file_mode ) );
		}
	}
}

}
}
}
}
