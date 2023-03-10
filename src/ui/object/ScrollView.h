#pragma once

#include <vector>

#include "Panel.h"

namespace ui {
namespace object {

CLASS( ScrollView, Panel )
	
	ScrollView( const std::string& class_name = "" );

	virtual void Create();
	virtual void Destroy();

	void AddChild( UIObject* object );
	void RemoveChild( UIObject* object );
	
	void SetWidth( const coord_t px );
	void SetHeight( const coord_t px );
	
	void SetInternalWidth( const coord_t px );
	void SetInternalHeight( const coord_t px );

private:
	
	vertex_t m_internal_size = {};
	void UpdateInternalSize();
	
	std::vector< UIObject* > m_to_add = {};
	std::vector< UIObject* > m_to_remove = {};
	
	Panel* m_body = nullptr;
	
};

}
}
