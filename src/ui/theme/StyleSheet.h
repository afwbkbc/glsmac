#pragma once

#include <unordered_map>
#include <vector>

#include "common/Common.h"

#include "Types.h"

namespace ui {
namespace theme {

class Style;

CLASS( StyleSheet, common::Class )

	virtual void AddStyles() = 0;

	virtual ~StyleSheet();

	void AddStyle( const std::string& style_name, const includes_t& includes, const style_handler_t handler );
	void AddStyle( const std::string& style_name, const style_handler_t handler );

	typedef std::unordered_map< std::string, Style* > styles_t;
	const std::vector< Style* > GetStyles();
	Style* GetStylePtr( const std::string& style_name ) const;

protected:
	void SetPrefix( const std::string& prefix );

private:
	styles_t m_styles = {};
	std::vector< std::string > m_styles_order = {};
	bool m_is_initialized = false;

	std::string m_prefix = "";
};

}
}
