#include "Module.h"

#include "ui_legacy/UI.h"

namespace ui_legacy {
namespace module {

Module::Module( UI* ui )
	: m_ui( ui ) {
	//
}

void Module::Activate() {
	ASSERT( !m_is_active, "module already active" );
	m_ui->ActivateModule( this );
	m_is_active = true;
}

void Module::Deactivate() {
	ASSERT( m_is_active, "module not active" );
	m_ui->DeactivateModule( this );
	m_is_active = false;
}

}
}
