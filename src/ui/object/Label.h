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
	
	Label( const string& text = "" );
	
	void SetText( const string& text );
	void SetTextColor( const Color& color );
	void SetFont( Font* font );

	void Create();
	void Destroy();
	void Align();
	void Draw();
	
protected:
	
	void ApplyStyle();
	
	Font *m_font = nullptr;
	string m_text = "";
	Color m_color = { 0.0, 0.0, 0.0, 0.0 };
	
	scene::actor::TextActor *m_actor = nullptr;
	
private:
	void UpdateActor();
		
};

}
}
