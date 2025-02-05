#pragma once

#include "Drawable.h"

namespace scene::actor {
class Text;
}

namespace ui {

namespace geometry {
class Text;
}

namespace dom {

class Text : public Drawable {
public:
	Text( DOM_ARGS );

	void SetText( const std::string& text );

	enum transform_t {
		T_NONE,
		T_UPPERCASE,
		T_LOWERCASE,
	};
	void SetTransform( const transform_t transform );

protected:
	virtual ~Text();

private:
	std::string m_text = "";
	scene::actor::Text* m_actor = nullptr;
	std::string m_fontname = "";
	uint16_t m_fontsize = 32;

	transform_t m_transform = T_NONE;

	void UpdateFont();
};

}
};
