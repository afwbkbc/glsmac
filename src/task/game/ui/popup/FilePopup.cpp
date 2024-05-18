#include "FilePopup.h"

#include "util/FS.h"

#include "engine/Engine.h"
#include "ui/object/FileBrowser.h"
#include "ui/UI.h"

namespace task {
namespace game {
namespace ui {
namespace popup {

FilePopup::FilePopup(
	Game* game,
	const std::string& title,
	const file_mode_t file_mode,
	const std::string& default_directory,
	const std::string& file_extension,
	const std::string& default_filename
)
	: OkCancelPopup( game )
	, m_title( title )
	, m_file_mode( file_mode )
	, m_default_directory( util::FS::GetAbsolutePath( default_directory ) )
	, m_file_extension( file_extension )
	, m_default_filename( default_filename ) {
	SetTitleText( m_title );
	// TODO: load from styles?
	SetWidth( 500 );
	SetHeight( 600 );
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
	m_file_browser->SetDefaultFilename( m_default_filename );
	m_file_browser->On(
		::ui::event::EV_SELECT, EH( this ) {
			const auto& path = *data->value.change.text;
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
					THROW( "unknown file mode " + std::to_string( m_file_mode ) );
				}
			}
			return true;
		}
	);
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
			THROW( "unknown file mode " + std::to_string( m_file_mode ) );
		}
	}
}

}
}
}
}
