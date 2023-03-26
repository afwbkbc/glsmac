#pragma once

#include "UIContainer.h"

#include "TextView.h"

#include "Input.h"

namespace ui {
namespace object {

CLASS( FileBrowser, UIContainer )
	
	FileBrowser( const std::string& class_name = "" ) : UIContainer( class_name ) {}
	
	void Create();
	void Destroy();

private:
	std::string m_current_directory = "";
	void ChangeDirectory( const std::string& directory );
	
	void SelectItem( const std::string& item );
	
	TextView* m_file_list = nullptr;
	Input* m_input = nullptr;
	
};

}
}
