#pragma once

// top level game object
// passed to scripts and everywhere, contains global state

#include "common/Common.h"
#include "gse/Bindings.h"
#include "gse/Wrappable.h"

#include "gse/GSE.h"
#include "gse/type/Object.h"

namespace ui {
class UI;
}

namespace task::main {
class Main;
}

CLASS3( GLSMAC, common::Class, gse::Bindings, gse::Wrappable )
	GLSMAC();
	~GLSMAC();

	void Iterate();

	WRAPDEFS_PTR( GLSMAC )

private:
	gse::GSE* m_gse = nullptr;
	gse::context::GlobalContext* m_ctx = nullptr;

	std::vector< gse::Value > m_main_callables = {};

	ui::UI* m_ui = nullptr;

private:
	friend class task::main::Main;
	void RunMain();

private:
	friend class gse::GSE;
	void AddToContext( gse::context::Context* ctx ) override;

};
