#include <iostream>

#include "config/Config.h"
#ifdef _WIN32
#include "error_handler/Win32ErrorHandler.h"
#else
#include "error_handler/StdoutErrorHandler.h"
#endif
#include "logger/StdoutLogger.h"
#include "loader/font/FreeTypeFontLoader.h"
#include "loader/texture/SDL2ImageTextureLoader.h"
#include "renderer/OpenGLRenderer.h"
#include "scheduler/SimpleScheduler.h"
#include "ui/DefaultUI.h"

#include "task/test/ManagerTestTask.h"

#include "task/IntroTask.h"

#include "engine/Engine.h"
#include "task/test/RendererTestTask.h"
#include "task/test/FontTestTask.h"
#include "task/test/UITestTask.h"
#include "task/test/WorldTestTask.h"
#include "task/test/ManagerTestTask.h"



#ifdef _WIN32
int WINAPI WinMain (HINSTANCE h, HINSTANCE hh, LPSTR cmd, int show) {
#else
int main(const int argc, const char *argv[]) {
#endif

	config::Config config( argc, argv );
	
#ifdef _WIN32
	error_handler::Win32ErrorHandler error_handler;
#else
	error_handler::StdoutErrorHandler error_handler;
#endif
	
	loader::font::FreeTypeFontLoader font_loader( "res/fonts" );
	loader::texture::SDL2ImageTextureLoader texture_loader( "res/textures" );
	logger::StdoutLogger logger;
	renderer::OpenGLRenderer renderer( "OpenSMAC", 640, 480, false, 90.0f);
	ui::DefaultUI ui;

	task::Task *task;

	scheduler::SimpleScheduler scheduler;
	
	// simple test
	//scheduler.AddTask( new task::test::FontTestTask );
	//scheduler.AddTask( new task::test::RendererTestTask );
	//scheduler.AddTask( new task::test::UITestTask );
	//scheduler.AddTask( new task::test::WorldTestTask );
	
	// stress-test
	/*for (size_t i = 0 ; i < 100 ; i++) {
		scheduler.AddTask( new task::test::ManagerTestTask );
	}*/
	
	// production
	scheduler.AddTask( new task::IntroTask );

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

	return engine.Run();
}
