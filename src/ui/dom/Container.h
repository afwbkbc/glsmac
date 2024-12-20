#pragma once

#include "Area.h"

namespace ui {
namespace dom {

#define FACTORY( _name, _class ) Factory( ctx, call_si, _name, [ this ]( GSE_CALLABLE, const properties_t& p ) { return new _class( ctx, call_si, m_ui, this, p ); })

class Container : public Area {
public:
	Container( DOM_ARGS_T );
	virtual ~Container();

protected:
	void Factory( GSE_CALLABLE, const std::string& name, const std::function< Object*( GSE_CALLABLE, const properties_t& ) >& f );

private:
	std::unordered_map< id_t, Object* > m_children = {};

};

}
}
