#pragma once

#include <vector>

#include "UIContainer.h"

#include "TextView.h"
#include "Input.h"

namespace ui {
namespace object {

CLASS( FileBrowser, UIContainer )
	
	FileBrowser( const std::string& class_name = "" ) : UIContainer( class_name ) {}
	
	// whether to allow selecting existing files only, or any file
	void SetExistingOnly( bool existing_only );

	void Create();
	void Destroy();

	const std::string& GetSelectedItem() const;
	void SelectCurrentItem();
	
private:
	std::string m_current_directory = "";
	void ChangeDirectory( const std::string& directory );
	
	bool m_existing_only = false;
	
	std::vector< std::string > m_selection_stack = {};
	
	void SelectItem( std::string item );
	bool m_is_typing = false;
	
	TextView* m_file_list = nullptr;
	Input* m_input = nullptr;
	
};

}
}
