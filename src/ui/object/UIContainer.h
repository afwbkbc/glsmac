#pragma once

#include <vector>

#include "UIObject.h"

#include "ui/event/UIEvent.h"

namespace ui {
namespace object {

class UIContainer : public UIObject {
public:
	void AddChild( UIObject *object );
	void RemoveChild( UIObject *object );

	virtual void Create();
	virtual void Destroy();
	virtual void Iterate();
	virtual void Realign();
	virtual void Redraw();

	virtual void SetZIndex( float z_index );
	
	void SetOverflow( const overflow_t overflow );
	void SendEvent( const event::UIEvent* event );
	
protected:
	std::vector<UIObject *> m_child_objects = {};
	
private:
	void CreateChild( UIObject *object );
	void DestroyChild( UIObject *object );
	const float GetChildZIndex() const;
};

} /* namespace object */
} /* namespace ui */
