#include <cmath>
#include <algorithm>

#include "MainMenuTask.h"

#include "engine/Engine.h"

#include "scene/actor/TextActor.h"

using namespace std;
using namespace ui::object;
using namespace loader::texture;

namespace task {
namespace mainmenu {

#define RANDNUM ( (float)rand() / (float)RAND_MAX )

void MainMenuTask::Start() {
	
	// background
	m_background = new Image( "xopeningb.pcx" );
	m_background->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_BOTTOM );
	m_background->SetZIndex( 0.5 );
	g_engine->GetUI()->AddObject( m_background );

	// menu entries	
	m_menu_item_textures = {
		// UPPER TOP
		g_engine->GetTextureLoader()->LoadTexture( "console_x.pcx", 708, 1, 715, 354, TextureLoader::LT_ROTATE | TextureLoader::LT_FLIPV | TextureLoader::LT_CONTRAST, 1.5 ),
		// LOWER TOP
		g_engine->GetTextureLoader()->LoadTexture( "console_x.pcx", 708, 1, 715, 354, TextureLoader::LT_ROTATE | TextureLoader::LT_FLIPV | TextureLoader::LT_CONTRAST, 0.8 ),
		// BOTTOM
		g_engine->GetTextureLoader()->LoadTexture( "console_x.pcx", 699, 1, 706, 293, TextureLoader::LT_ROTATE | TextureLoader::LT_FLIPV | TextureLoader::LT_CONTRAST, 0.2 ),
		// LEFT
		g_engine->GetTextureLoader()->LoadTexture( "interface.pcx", 472, 756, 476, 790, TextureLoader::LT_CONTRAST, 0.3 ),
		// RIGHT
		g_engine->GetTextureLoader()->LoadTexture( "interface.pcx", 299, 756, 303, 790, TextureLoader::LT_CONTRAST, 0.2 ),
		// BODY
		g_engine->GetTextureLoader()->LoadTexture( "palette.pcx", 450, 28, 450, 28, TextureLoader::LT_ALPHA, 0.8 )
	};
	
	m_menu_item_font = g_engine->GetFontLoader()->LoadFont( "arialnb.ttf", 20 );
	
	m_menu_choices.clear();
	ShowNextMenu();

}

void MainMenuTask::OnMenuClick( const string& choice ) {
	m_menu_choices.push_back( choice );
	ShowNextMenu();
}

void MainMenuTask::Stop() {
	
	g_engine->GetUI()->RemoveObject( m_menu );
	g_engine->GetUI()->RemoveObject( m_background );
	
	for (auto& t : m_menu_item_textures) {
		delete t;
	}
}

void MainMenuTask::Iterate() {

}

void MainMenuTask::ShowMenu( const vector<string>& choices ) {
	if (m_menu) {
		g_engine->GetUI()->RemoveObject( m_menu );
	}
	m_menu = new Menu( this );
	for (auto& c : choices) {
		m_menu->AddItem(c);
	}
	g_engine->GetUI()->AddObject( m_menu );
}

static vector<string> s_main_menu = {
	"START GAME",
	"QUICK START",
	"SCENARIO",
	"LOAD GAME",
	"MULTIPLAYER",
	"VIEW CREDITS",
	"EXIT GAME",
};

static vector<string> s_planetsize_menu = {
	"TINY PLANET",
	"SMALL PLANET",
	"STANDARD PLANET",
	"LARGE PLANET",
	"HUGE PLANET",
};

static vector<string> s_difficulty_menu = {
	"CITIZEN",
	"SPECIALIST",
	"TALENT",
	"LIBRARIAN",
	"THINKER",
	"TRANSCEND",
};

static vector<string> s_rules_menu = {
	"PLAY WITH STANDARD RULES",
	"PLAY WITH CURRENT RULES",
	"CUSTOMIZE RULES",
};

static vector<string> s_customize_menu_1 = {
	"30-50% OF SURFACE",
	"50-70% OF SURFACE",
	"70-90% OF SURFACE",
};

static vector<string> s_customize_menu_2 = {
	"STRONG",
	"AVERAGE",
	"WEAK",
};

static vector<string> s_customize_menu_3 = {
	"RARE",
	"AVERAGE",
	"ABUNDANT",
};

static vector<string> s_customize_menu_4 = {
	"SPARSE",
	"AVERAGE",
	"DENSE",
};

void MainMenuTask::ShowNextMenu() {
	if (m_menu_choices.empty()) {
		ShowMenu( s_main_menu );
	}
	else {
		const string& choice = m_menu_choices.back();
		if (choice == "START GAME") {
			ShowMenu({
				"MAKE RANDOM MAP",
				"CUSTOMIZE RANDOM MAP",
				"THE MAP OF PLANET",
				"HUGE MAP OF PLANET",
				"LOAD MAP FILE",
			});
		}
		else if (choice == "MAKE RANDOM MAP" || choice == "CUSTOMIZE RANDOM MAP") {
			ShowMenu( s_planetsize_menu );
		}
		else if (choice == "THE MAP OF PLANET" || choice == "HUGE MAP OF PLANET") {
			ShowMenu( s_difficulty_menu );
		}
		else if (find(s_planetsize_menu.begin(), s_planetsize_menu.end(), choice) != s_planetsize_menu.end()) {
			// HACK: check what was before
			const string& prev_choice = m_menu_choices.at( m_menu_choices.size() - 2 );
			if (prev_choice == "MAKE RANDOM MAP") {
				ShowMenu( s_difficulty_menu );
			}
			else if (prev_choice == "CUSTOMIZE RANDOM MAP") {
				ShowMenu( s_customize_menu_1 );
			}
		}
		else if (find(s_customize_menu_1.begin(), s_customize_menu_1.end(), choice) != s_customize_menu_1.end()) {
			ShowMenu( s_customize_menu_2 );
		}
		else if (find(s_customize_menu_2.begin(), s_customize_menu_2.end(), choice) != s_customize_menu_2.end()) {
			ShowMenu( s_customize_menu_3 );
		}
		else if (find(s_customize_menu_3.begin(), s_customize_menu_3.end(), choice) != s_customize_menu_3.end()) {
			ShowMenu( s_customize_menu_4 );
		}
		else if (find(s_customize_menu_4.begin(), s_customize_menu_4.end(), choice) != s_customize_menu_4.end()) {
			ShowMenu( s_difficulty_menu );
		}
		else if (find(s_difficulty_menu.begin(), s_difficulty_menu.end(), choice) != s_difficulty_menu.end()) {
			ShowMenu( s_rules_menu );
		}
		else if (choice == "SCENARIO") {
			ShowMenu({
				"PLAY SCENARIO",
				"CREATE SCENARIO",
				"LOAD MAP FILE",
				"EDIT SCENARIO",
			});
		}
		else if (choice == "EXIT GAME") {
			g_engine->ShutDown();
		}
		else {
			ShowMenu( s_main_menu );
		}
	}
}

} /* namespace mainmenu */
} /* namespace task */
