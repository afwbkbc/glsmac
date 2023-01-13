#pragma once

#include "base/Base.h"

#include <unordered_map>
#include <string>

#include "Style.h"

using namespace std;

namespace ui {
namespace theme {

MAJOR_CLASS( Theme, base::Base )

	virtual ~Theme();
	
	void AddStyle( Style* style );
	const Style* GetStyle( const string class_name ) const;
	
	void Finalize();
	
protected:
	unordered_map< string, Style* > m_styles = {};
	bool m_is_finalized = false;
};

}
}
