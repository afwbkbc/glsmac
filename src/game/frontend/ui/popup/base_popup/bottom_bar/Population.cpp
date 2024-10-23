#include "Population.h"

#include "ui/object/Surface.h"
#include "game/frontend/base/Base.h"
#include "game/frontend/base/PopDef.h"
#include "game/frontend/faction/Faction.h"

namespace game {
namespace frontend {
namespace ui {
namespace popup {
namespace base_popup {
namespace bottom_bar {

Population::Population( Game* game )
	: BBSection( game, "Population" ) {
	m_config.no_inner_border = true;
}

void Population::Create() {
	BBSection::Create();

}

void Population::Destroy() {
	HideIcons();

	BBSection::Destroy();
}

void Population::Update( base::Base* base ) {
	// TODO: use actual pop objects
	HideIcons();
	const auto& pops = base->GetPops();
	const size_t pops_count = pops.size();
	float w = 40.0f;
	if ( w * ( pops_count - 1 ) > 390.0f ) {
		w = 390.0f / ( pops_count - 1 );
	}
	m_icons.reserve( pops_count );
	for ( size_t i = 0 ; i < pops_count ; i++ ) {
		const auto& pop = pops.at( i );
		NEWV( icon, ::ui::object::Surface, SubClass( "Icon" ) );
		icon->SetLeft( i * w );
		// TODO: SetTexture doesn't work if used before AddChild, fix
		AddChild( icon );
		icon->SetTexture( pop.GetTexture() );
		m_icons.push_back( icon );
	}
}

void Population::HideIcons() {
	for ( const auto& icon : m_icons ) {
		RemoveChild( icon );
	}
	m_icons.clear();
}

}
}
}
}
}
}
