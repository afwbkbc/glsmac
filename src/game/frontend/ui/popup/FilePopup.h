#pragma once

#include "OkCancelPopup.h"

namespace ui::object {
class FileBrowser;
}

namespace game {
namespace frontend {
namespace ui {
namespace popup {

CLASS( FilePopup, OkCancelPopup )

	enum file_mode_t {
		FM_NONE,
		FM_READ,
		FM_WRITE
	};

	FilePopup(
		Game* game,
		const std::string& title,
		const file_mode_t file_mode,
		const std::string& default_directory = "",
		const std::string& file_extension = "",
		const std::string& default_filename = ""
	);

	void Create() override;
	void Destroy() override;

protected:
	void OnOk() override;

	virtual void OnFileSelect( const std::string& path ) {}

private:
	const std::string m_title = "";
	const file_mode_t m_file_mode = FM_NONE;
	const std::string m_default_directory = "";
	const std::string m_file_extension = "";
	const std::string m_default_filename = "";

	::ui::object::FileBrowser* m_file_browser = nullptr;

};

}
}
}
}
