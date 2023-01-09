#pragma once

#include <vector>

#include "base/Base.h"
#include "base/Module.h"
#include "base/Thread.h"

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

	logger::Logger *GetLogger() { return m_logger; }
	loader::font::FontLoader *GetFontLoader() { return m_font_loader; }
	loader::texture::TextureLoader *GetTextureLoader() { return m_texture_loader; }
	renderer::Renderer *GetRenderer() { return m_renderer; }
	scheduler::Scheduler *GetScheduler() { return m_scheduler; }
	ui::UI *GetUI() { return m_ui; }

protected:

	struct {
		base::Thread main;
	} m_threads;

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
