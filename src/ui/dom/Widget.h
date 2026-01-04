#pragma once

#include "Surface.h"

#include "ui/Types.h"

namespace scene::actor {
class Mesh;
}

namespace ui {

class Geometry;
class UI;

namespace dom {

class Widget : public Surface {
public:

	Widget( DOM_ARGS );

	void Show() override;
	void Hide() override;

private:

	widget_type_t m_type = WT_NONE;
	void Enable( const widget_type_t type );
	void Disable();

	bool m_is_attached = false;
	void Attach();
	void Detach();

private:
	friend class ui::UI;
	types::texture::Texture* const GetTexture();

};

}
}
