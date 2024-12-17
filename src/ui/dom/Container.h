#pragma once

#include "Object.h"

namespace ui {
namespace dom {

#define FACTORY( _name, _class ) Factory( _name, [ this ]( const properties_t& p ) { return new _class( this, p ); })

class Container : public Object {
public:
	Container( const std::string& cls, Object* const parent, const properties_t& properties );
	virtual ~Container();

protected:
	void Factory( const std::string& name, const std::function< Object*( const properties_t& ) >& f );

private:
	std::unordered_map< id_t, Object* > m_children = {};

};

}
}
