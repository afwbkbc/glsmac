#pragma once

#include <vector>
#include <unordered_map>
#include <atomic>
#include <string>

// TODO: move to config
extern const size_t g_max_fps;

namespace common {
class Thread;
}

namespace config {
class Config;
}

namespace error_handler {
class ErrorHandler;
}

namespace logger {
class Logger;
}

namespace resource {
class ResourceManager;
}

namespace loader {
namespace font {
class FontLoader;
}
namespace texture {
class TextureLoader;
}
namespace sound {
class SoundLoader;
}
namespace txt {
class TXTLoaders;
}
}

namespace input {
class Input;
}

namespace graphics {
class Graphics;
}

namespace audio {
class Audio;
}

namespace network {
class Network;
}

namespace game::backend {
class Game;
}

namespace scheduler {
class Scheduler;
}

namespace ui_legacy {
class UI;
}

namespace gc {
class GC;
}

namespace engine {

class Engine {
public:

	Engine(
		config::Config* config,
		error_handler::ErrorHandler* error_handler,
		const std::vector< logger::Logger* >& loggers,
		resource::ResourceManager* resource_manager,
		loader::font::FontLoader* font_loader,
		loader::texture::TextureLoader* texture_loader,
		loader::sound::SoundLoader* sound_loader,
		loader::txt::TXTLoaders* txt_loaders,
		scheduler::Scheduler* scheduler,
		input::Input* input,
		graphics::Graphics* graphics,
		audio::Audio* audio,
		network::Network* network,
		ui_legacy::UI* ui,
		game::backend::Game* game
	);

	~Engine();
	int Run();
	void ShutDown();

	config::Config* GetConfig() const { return m_config; }
	resource::ResourceManager* GetResourceManager() const { return m_resource_manager; }
	loader::font::FontLoader* GetFontLoader() const { return m_font_loader; }
	loader::texture::TextureLoader* GetTextureLoader() const { return m_texture_loader; }
	loader::sound::SoundLoader* GetSoundLoader() const { return m_sound_loader; }
	loader::txt::TXTLoaders* GetTXTLoaders() const { return m_txt_loaders; }
	input::Input* GetInput() const { return m_input; }
	graphics::Graphics* GetGraphics() const { return m_graphics; }
	audio::Audio* GetAudio() const { return m_audio; }
	network::Network* GetNetwork() const { return m_network; }
	scheduler::Scheduler* GetScheduler() const { return m_scheduler; }
	ui_legacy::UI* GetUI() const { return m_ui; }
	game::backend::Game* GetGame() const { return m_game; }
	gc::GC* GetGC() const { return m_gc; }

	void Log( const std::string& text ) const;

protected:

	std::atomic< bool > m_is_shutting_down = false;

	std::vector< common::Thread* > m_threads = {};

	config::Config* const m_config = nullptr;
	error_handler::ErrorHandler* m_error_handler = nullptr;
	const std::vector< logger::Logger* > m_loggers = {};
	resource::ResourceManager* m_resource_manager = nullptr;
	loader::font::FontLoader* m_font_loader = nullptr;
	loader::texture::TextureLoader* m_texture_loader = nullptr;
	loader::sound::SoundLoader* m_sound_loader = nullptr;
	loader::txt::TXTLoaders* m_txt_loaders = nullptr;
	scheduler::Scheduler* m_scheduler = nullptr;
	input::Input* m_input = nullptr;
	graphics::Graphics* m_graphics = nullptr;
	audio::Audio* m_audio = nullptr;
	network::Network* m_network = nullptr;
	ui_legacy::UI* m_ui = nullptr;
	game::backend::Game* m_game = nullptr;
	gc::GC* m_gc = nullptr;

};

}

extern engine::Engine* g_engine;
