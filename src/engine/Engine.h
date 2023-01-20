#pragma once

#include <vector>
#include <unordered_map>

#include "base/Base.h"
#include "base/Module.h"
#include "base/Thread.h"

#include "config/Config.h"
#include "error_handler/ErrorHandler.h"
#include "logger/Logger.h"

#include "loader/font/FontLoader.h"
#include "loader/texture/TextureLoader.h"
#include "loader/sound/SoundLoader.h"

#include "input/Input.h"
#include "graphics/Graphics.h"
#include "audio/Audio.h"

#include "scheduler/Scheduler.h"

#include "ui/UI.h"

// TODO: move to config
extern const size_t g_max_fps;

namespace engine {

MAJOR_CLASS( Engine, base::Base );
	Engine(
		config::Config *config,
		error_handler::ErrorHandler *error_handler,
		logger::Logger *logger,
		loader::font::FontLoader *font_loader,
		loader::texture::TextureLoader *texture_loader,
		loader::sound::SoundLoader *sound_loader,
		scheduler::Scheduler *scheduler,
		input::Input *input,
		graphics::Graphics *graphics,
		audio::Audio *audio,
		ui::UI *ui
	);
	~Engine();

	int Run();
	void ShutDown();

	config::Config *GetConfig() const { return m_config; }
	logger::Logger *GetLogger() const { return m_logger; }
	loader::font::FontLoader *GetFontLoader() const { return m_font_loader; }
	loader::texture::TextureLoader *GetTextureLoader() const { return m_texture_loader; }
	loader::sound::SoundLoader* GetSoundLoader() const { return m_sound_loader; }
	input::Input *GetInput() const { return m_input; }
	graphics::Graphics *GetGraphics() const { return m_graphics; }
	audio::Audio *GetAudio() const { return m_audio; }
	scheduler::Scheduler *GetScheduler() const { return m_scheduler; }
	ui::UI *GetUI() const { return m_ui; }

protected:

	struct {
		base::Thread main;
		base::Thread loaders;
	} m_threads;

	config::Config *const m_config = nullptr;
	error_handler::ErrorHandler *m_error_handler = nullptr;
	logger::Logger *m_logger = nullptr;
	loader::font::FontLoader *m_font_loader = nullptr;
	loader::texture::TextureLoader *m_texture_loader = nullptr;
	loader::sound::SoundLoader *m_sound_loader = nullptr;
	scheduler::Scheduler *m_scheduler = nullptr;
	input::Input *m_input = nullptr;
	graphics::Graphics *m_graphics = nullptr;
	audio::Audio *m_audio = nullptr;
	ui::UI *m_ui = nullptr;
	
#if DEBUG
	bool m_is_debug_mode = false;
#endif
};

} /* namespace engine */

extern engine::Engine *g_engine;
