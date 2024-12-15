#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "common/Common.h"

namespace ui_legacy {
namespace theme {

class Style;
class StyleSheet;

CLASS( Theme, common::Class )

	virtual ~Theme() = default;

	void AddStyle( Style* style );
	void AddStyleSheet( StyleSheet* stylesheet );
	const Style* GetStyle( const std::string class_name ) const;

	void Finalize();

protected:
	std::unordered_map< std::string, Style* > m_styles = {};
	std::vector< std::string > m_styles_order = {};
	bool m_is_finalized = false;
};

}
}
