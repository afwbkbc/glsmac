#pragma once

#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <atomic>

#include "common/Module.h"

#include "common/Mutex.h"

namespace graphics {
class Graphics;
}

namespace scene {
class Scene;
namespace actor {
class Mesh;
class Text;
}
}

namespace types {
class Font;
}

namespace console {

CLASS( Console, common::Module )
public:

	Console();

	void Start() override;
	void Stop() override;
	void Iterate() override;

private:

	graphics::Graphics* m_graphics = nullptr;

	void Enable();
	void Disable();

	types::Font* m_font = nullptr;

	// not using ui::UI for performance and independence reasons

	std::atomic< bool > m_is_enabled = false;

	scene::Scene* m_scene = nullptr;
	struct {
		scene::actor::Mesh* background = nullptr;
		std::unordered_map< scene::actor::Text*, size_t > lines = {};
	} m_actors = {};

	size_t m_lines_on_screen = 0;
	float m_lines_diff = 0.0f;
	float m_padding_diff = 0.0f;

	struct line_t {
		std::string text;
		scene::actor::Text* actor;
	};

	common::Mutex m_history_mutex;
	size_t m_next_history_id = 1;
	std::map< size_t, line_t > m_history = {};
	std::vector< size_t > m_history_actor_to_add = {};
	std::vector< scene::actor::Text* > m_history_actor_to_remove = {};

	void AddLine( const std::string& text );

	void Realign();

	scene::actor::Text* const AddLineActor( const std::string& text, const float y );

	void ShowLines();
	void HideLines();

};

}
