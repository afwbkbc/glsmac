#pragma once

#include "base/Base.h"

#include <unordered_map>
#include <string>

#include "Style.h"
#include "StyleSheet.h"

using namespace std;

namespace ui {
namespace theme {

CLASS( Theme, base::Base )

	void AddStyle( Style* style );
	void AddStyleSheet( StyleSheet* stylesheet );
	const Style* GetStyle( const string class_name ) const;
	
	void Finalize();
	
protected:
	unordered_map< string, Style* > m_styles = {};
	vector< string > m_styles_order = {};
	bool m_is_finalized = false;
};

}
}
