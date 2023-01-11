#pragma once

#include "UIObject.h"

#include <string>

#include "types/Font.h"
#include "scene/actor/TextActor.h"

using namespace std;

namespace ui {
namespace object {

CHILD_CLASS( Label, UIObject )
	
	Label(types::Font* m_font, const string& text, const types::Color& color);
	
	void Create();
	void Destroy();
	void Align();
	void Draw();
	
protected:
	
	types::Font *m_font;
	const string m_text;
	const types::Color m_color;
	
	scene::actor::TextActor *m_actor;
	
		
};

}
}
