#pragma once

#include "UIObject.h"

#include <string>

#include "types/Font.h"
#include "scene/actor/Text.h"

using namespace std;
using namespace types;

namespace ui {
namespace object {

CLASS( Label, UIObject )
	
	Label( const string& class_name = "" );
	
	void SetText( const string& text );
	void SetTextColor( const Color& color );
	void SetFont( Font* font );

	void Create();
	void Destroy();
	void Align();
	
protected:
	
	void ApplyStyle();
	
	Font *m_font = nullptr;
	string m_text = "";
	Color m_color = { 0.0, 0.0, 0.0, 0.0 };
	
	scene::actor::Text *m_actor = nullptr;
	
		
};

}
}
