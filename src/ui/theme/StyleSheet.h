#pragma once

#include <unordered_map>

#include "base/Base.h"

#include "ui/theme/Style.h"
#include "ui/object/UIObject.h"

namespace ui {
namespace theme {

CLASS( StyleSheet, base::Base )
	
	virtual void AddStyles() = 0;

	virtual ~StyleSheet();

	void AddStyle( const std::string& style_name, const Style::includes_t& includes, const Style::style_handler_t handler );
	void AddStyle( const std::string& style_name, const Style::style_handler_t handler );
	
	typedef std::unordered_map< std::string, Style* > styles_t;
	const std::vector< Style* > GetStyles();
	Style* GetStylePtr( const std::string& style_name ) const;
	
private:
	styles_t m_styles = {};
	std::vector< std::string > m_styles_order = {};
	bool m_is_initialized = false;
};

}
}
