#pragma once

#include <vector>

#include "UIObject.h"

#include "ui/event/UIEvent.h"

namespace ui {
namespace object {

CLASS( UIContainer, UIObject )

	UIContainer( const std::string& class_name = "" );

	void AddChild( UIObject* object );
	void RemoveChild( UIObject* object );

	virtual void Create() override;
	virtual void Destroy() override;
	virtual void Iterate() override;
	virtual void RealignNow() override;
	virtual void Redraw() override;

	void UpdateZIndex() override;

	// for now this only affects OVERFLOW_HIDDEN frame
	void SetPadding( const coord_t px );
	const coord_t GetPadding() const;

	void SetOverflow( const overflow_t overflow ) override;
	void ProcessEvent( event::UIEvent* event ) override;

	virtual const object_area_t GetInternalObjectArea() override;

	void AddStyleModifier( const Style::modifier_t modifier ) override;
	void RemoveStyleModifier( const Style::modifier_t modifier ) override;

	virtual void SetAreaLimits( const coord_box_t limits ) override;
	virtual void SetAreaLimitsMaybe( const coord_box_t limits ) override; // set limits for self and for children that aren't limited by object(s)
	virtual void SetAreaLimitsByObject( UIObject* source_object ) override; // make sure source object lives longer than this one!
	virtual void ClearAreaLimits() override;

	void BlockEvents() override;
	void UnblockEvents() override;

	void SetEventContexts( event_context_t contexts ) override;
	void AddEventContexts( event_context_t contexts ) override;

	virtual void ShowActors() override;
	virtual void HideActors() override;

	void GrowFromObjectMaybe( UIObject* object );
	void ShrinkToFitMaybe();

	void UpdateAreaLimits();

protected:
	virtual void ApplyStyle() override;
	virtual void ReloadStyle() override;

	void SetOverriddenEventContexts( event_context_t contexts ) override;
	void AddOverriddenEventContexts( event_context_t contexts ) override;

	std::vector< UIObject* > m_child_objects = {};

	const std::string Subclass( const std::string& class_name ) const;

	coord_t m_padding = 0;

private:
	void CreateChild( UIObject* object );
	void DestroyChild( UIObject* object );

	void GrowFromObject( UIObject* object );

	void ShrinkToFit();

};

} /* namespace object */
} /* namespace ui */
