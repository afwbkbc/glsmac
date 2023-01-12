#pragma once

#include "UIObject.h"

#include <string>

#include "types/Font.h"
#include "scene/actor/TextActor.h"

using namespace std;
using namespace types;

namespace ui {
namespace object {

CHILD_CLASS( Label, UIObject )
	
	Label(Font* m_font, const string& text, const Color& color);
	
	void SetText( const string& text );
	void SetTextColor( const Color& color );

	void Create();
	void Destroy();
	void Align();
	void Draw();
	
protected:
	
	Font *m_font = nullptr;
	string m_text = "";
	Color m_color = Color::TRANSPARENT();
	
	scene::actor::TextActor *m_actor = nullptr;
	
		
};

}
}
