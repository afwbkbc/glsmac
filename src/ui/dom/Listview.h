#pragma once

#include "Scrollview.h"

#include <map>

namespace ui {
namespace dom {

class Listview : public Scrollview {
public:
	Listview( DOM_ARGS_TD( "listview" ) );

private:

	size_t m_itemsize = 0;
	bool m_was_visible = false;
	float m_oldmax = 0.0f;

	enum scroll_type_t {
		ST_VERTICAL,
		ST_HORIZONTAL,
	};
	scroll_type_t m_scroll_type = ST_VERTICAL;

	std::map< id_t, Object* > m_children = {};

	void SetItemSize( GSE_CALLABLE, const int item_size );
	void SetScrollType( const scroll_type_t type );

	void Realign();
};

}
}
