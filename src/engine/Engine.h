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
#include "renderer/Renderer.h"
#include "scheduler/Scheduler.h"
#include "ui/UI.h"

namespace engine {

MAJOR_CLASS( Engine, base::Base );
	Engine(
		config::Config *config,
		error_handler::ErrorHandler *error_handler,
		logger::Logger *logger,
		loader::font::FontLoader *font_loader,
		loader::texture::TextureLoader *texture_loader,
		scheduler::Scheduler *scheduler,
		renderer::Renderer *renderer,
		ui::UI *ui
	);
	~Engine();

	int Run();
	void ShutDown();

	config::Config *GetConfig() const { return m_config; }
	logger::Logger *GetLogger() const { return m_logger; }
	loader::font::FontLoader *GetFontLoader() const { return m_font_loader; }
	loader::texture::TextureLoader *GetTextureLoader() const { return m_texture_loader; }
	renderer::Renderer *GetRenderer() const { return m_renderer; }
	scheduler::Scheduler *GetScheduler() const { return m_scheduler; }
	ui::UI *GetUI() const { return m_ui; }

protected:

	struct {
		base::Thread main;
		base::Thread loaders;
	} m_threads;

	config::Config *const m_config;
	error_handler::ErrorHandler *m_error_handler;
	logger::Logger *m_logger;
	loader::font::FontLoader *m_font_loader;
	loader::texture::TextureLoader *m_texture_loader;
	scheduler::Scheduler *m_scheduler;
	renderer::Renderer *m_renderer;
	ui::UI *m_ui;
	
	
};

} /* namespace engine */

extern engine::Engine *g_engine;
