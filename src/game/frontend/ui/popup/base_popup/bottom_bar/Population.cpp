#include "Population.h"

#include "ui/object/Surface.h"
#include "game/frontend/base/Base.h"
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
	auto icon_class = SubClass(
		base->GetFaction()->m_is_progenitor
			? "IconProgenitor"
			: "IconHuman"
	);
	const auto population = base->GetPopulation();
	float w = 40.0f;
	if ( w * ( population - 1 ) > 390.0f ) {
		w = 390.0f / ( population - 1 );
	}
	m_icons.reserve( population );
	for ( size_t i = 0 ; i < population ; i++ ) {
		NEWV( icon, ::ui::object::Surface, icon_class );
		icon->SetLeft( i * w );
		AddChild( icon );
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
