#pragma once

#include "base/Base.h"

#include <unordered_map>

#include "ui/theme/Style.h"
#include "ui/object/UIObject.h"

using namespace std;

namespace ui {
namespace theme {

CLASS( StyleSheet, base::Base )
	
	virtual void AddStyles() = 0;

	virtual ~StyleSheet();

	void AddStyle( const string& style_name, const Style::includes_t& includes, const Style::style_handler_t handler );
	void AddStyle( const string& style_name, const Style::style_handler_t handler );
	
	typedef unordered_map< string, Style* > styles_t;
	const vector< Style* > GetStyles();
	Style* GetStylePtr( const string& style_name ) const;
	
private:
	styles_t m_styles = {};
	vector< string > m_styles_order = {};
	bool m_is_initialized = false;
};

}
}
