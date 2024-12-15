#pragma once

#include "common/Common.h"
#include "gse/Wrappable.h"
#include "gse/type/Object.h"

namespace ui {

namespace dom {
class Root;
}

CLASS2( UI, common::Class, gse::Wrappable )

	UI();
	~UI();

	WRAPDEFS_PTR( UI );

private:
	dom::Root* m_root;
};

}
