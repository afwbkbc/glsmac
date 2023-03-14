#pragma once

#include <vector>

#include "UIObject.h"

#include "ui/event/UIEvent.h"

namespace ui {
namespace object {

CLASS( UIContainer, UIObject )

	UIContainer( const std::string& class_name = "" );
	
	void AddChild( UIObject *object );
	void RemoveChild( UIObject *object );

	virtual void Create();
	virtual void Destroy();
	virtual void Iterate();
	virtual void RealignNow();
	virtual void Redraw();

	void UpdateZIndex();
	
	// for now this only affects OVERFLOW_HIDDEN frame
	void SetPadding( const coord_t px );
	const coord_t GetPadding() const;
	
	void SetOverflow( const overflow_t overflow );
	void ProcessEvent( event::UIEvent* event );
	
	virtual const object_area_t GetInternalObjectArea();
	
	void AddStyleModifier( const Style::modifier_t modifier );
	void RemoveStyleModifier( const Style::modifier_t modifier );

	virtual void SetAreaLimits( const coord_box_t limits );
	virtual void SetAreaLimitsByObject( UIObject* source_object ); // make sure source object lives longer than this one!
	
	void BlockEvents();
	void UnblockEvents();
	
	void SetEventContexts( event_context_t contexts );
	void AddEventContexts( event_context_t contexts );
	
	virtual void ShowActors();
	virtual void HideActors();
	
protected:
	virtual void ApplyStyle();
	virtual void ReloadStyle();
	
	void SetOverriddenEventContexts( event_context_t contexts );
	void AddOverriddenEventContexts( event_context_t contexts );
	
	std::vector<UIObject *> m_child_objects = {};
	
	const std::string Subclass( const std::string& class_name ) const;
	
	coord_t m_padding = 0;
	
private:
	void CreateChild( UIObject *object );
	void DestroyChild( UIObject *object );
};

} /* namespace object */
} /* namespace ui */
