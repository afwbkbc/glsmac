#pragma once

#include <vector>

#include "UIObject.h"

#include "ui/event/UIEvent.h"

using namespace std;

namespace ui {
namespace object {

CLASS( UIContainer, UIObject )

	UIContainer( const string& class_name = "" );
	
	virtual void AddChild( UIObject *object );
	virtual void RemoveChild( UIObject *object );

	virtual void Create();
	virtual void Destroy();
	virtual void Iterate();
	virtual void Realign();
	virtual void Redraw();

	void UpdateZIndex();
	
	void SetOverflow( const overflow_t overflow );
	void ProcessEvent( event::UIEvent* event );
	
	void AddStyleModifier( const Style::modifier_t modifier );
	void RemoveStyleModifier( const Style::modifier_t modifier );

	void BlockEvents();
	void UnblockEvents();
	
protected:
	virtual void ApplyStyle();
	virtual void ReloadStyle();
	
	void SetEventContexts( event_context_t contexts );
	void SetOverriddenEventContexts( event_context_t contexts );
	
	std::vector<UIObject *> m_child_objects = {};
	
	const string Subclass( const string& class_name ) const;
	
private:
	void CreateChild( UIObject *object );
	void DestroyChild( UIObject *object );
};

} /* namespace object */
} /* namespace ui */
