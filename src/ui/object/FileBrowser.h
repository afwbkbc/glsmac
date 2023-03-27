#pragma once

#include <vector>

#include "UIContainer.h"

#include "TextView.h"
#include "Input.h"

namespace ui {
namespace object {

CLASS( FileBrowser, UIContainer )
	
	FileBrowser( const std::string& class_name = "" ) : UIContainer( class_name ) {}

	// directory to show first
	void SetDefaultDirectory( const std::string& default_directory );

	// force specific file extension
	void SetFileExtension( const std::string& file_extension );
	
	// specify default file name (without extension)
	void SetDefaultFileName( const std::string& default_filename );
	
	// whether to allow selecting existing files only, or any file
	void SetExistingOnly( bool existing_only );

	void Create();
	void Destroy();

	const std::string& GetSelectedItem() const;
	void SelectCurrentItem();
	
private:
	std::string m_default_directory = "";
	std::string m_file_extension = "";
	std::string m_default_filename = "";
	
	std::string m_current_directory = "";
	void ChangeDirectory( std::string directory );
	
	bool m_existing_only = false;
	
	std::vector< std::string > m_selection_stack = {};
	
	void SelectItem( std::string item );
	bool m_is_typing = false;
	
	TextView* m_file_list = nullptr;
	Input* m_input = nullptr;
	
};

}
}
