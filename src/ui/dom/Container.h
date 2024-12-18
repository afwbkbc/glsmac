#pragma once

#include "Object.h"

namespace ui {
namespace dom {

#define FACTORY( _name, _class ) Factory( ctx, call_si, _name, [ this ]( GSE_CALLABLE, const properties_t& p ) { return new _class( ctx, call_si, m_ui, this, p ); })

class Container : public Object {
public:
	Container( GSE_CALLABLE, UI* const ui, const std::string& tag, Object* const parent, const properties_t& properties );
	virtual ~Container();

	Geometry* const GetGeometry() const override;

protected:
	void Factory( GSE_CALLABLE, const std::string& name, const std::function< Object*( GSE_CALLABLE, const properties_t& ) >& f );

private:
	std::unordered_map< id_t, Object* > m_children = {};

private:
	friend class Root; // Root will resize it on window resize
	Geometry* m_geometry = nullptr;
	
};

}
}
