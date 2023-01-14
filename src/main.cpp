#include <thread>

#include "config/Config.h"
#ifdef _WIN32
#include "error_handler/Win32ErrorHandler.h"
#else
#include "error_handler/StdoutErrorHandler.h"
#endif
#include "logger/StdoutLogger.h"
#include "loader/font/FreeTypeFontLoader.h"
#include "loader/texture/SDL2ImageTextureLoader.h"
#include "renderer/opengl/OpenGLRenderer.h"
#include "scheduler/SimpleScheduler.h"
#include "ui/DefaultUI.h"

#include "game/intro/Intro.h"

#include "engine/Engine.h"

#include "version.h"
#include "tmp/last_commit.h"

// TODO: move to config
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define WINDOW_VSYNC true

#ifdef _WIN32
int WINAPI WinMain (HINSTANCE h, HINSTANCE hh, LPSTR cmd, int show) {
#else
int main(const int argc, const char *argv[]) {
#endif

	int result = EXIT_FAILURE;

	// logger needs to be outside of scope to be destroyed last
	logger::StdoutLogger logger;
	{
		config::Config config( argc, argv );
		
#ifdef _WIN32
		error_handler::Win32ErrorHandler error_handler;
#else
		error_handler::StdoutErrorHandler error_handler;
#endif

		loader::font::FreeTypeFontLoader font_loader;

		loader::texture::SDL2ImageTextureLoader texture_loader;
		texture_loader.SetTransparentColor(types::Color::RGBA(255, 0, 255, 255));

		auto title = (string) "GLSMAC " + GLSMAC_VERSION + "-" + GLSMAC_LAST_COMMIT;
#if DEBUG
		title += "-debug";
#elif PORTABLE
		title += "-portable";
#endif
		renderer::opengl::OpenGLRenderer renderer( title, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_VSYNC, 90.0f);
		ui::DefaultUI ui;

		scheduler::SimpleScheduler scheduler;

		// game entry point
		NEWV( task, game::intro::Intro );
		scheduler.AddTask( task );

		engine::Engine engine(
			&config,
			&error_handler,
			&logger,
			&font_loader,
			&texture_loader,
			&scheduler,
			&renderer,
			&ui
		);

		result = engine.Run(); 
	}

	return result;
}
