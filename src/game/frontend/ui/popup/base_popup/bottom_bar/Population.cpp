#include "Population.h"

#include "ui/object/Surface.h"
#include "game/frontend/Game.h"
#include "game/frontend/base/Base.h"
#include "game/frontend/base/PopDef.h"
#include "game/frontend/base/BaseManager.h"
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

	m_pops.clear();
	m_pops.reserve( pops_count );

	std::unordered_map< std::string, std::vector< size_t > > pops_by_def = {};
	for ( size_t i = 0 ; i < pops_count ; i++ ) {
		const auto& pop = pops.at( i );
		auto it = pops_by_def.find( pop.m_def->m_name );
		if ( it == pops_by_def.end() ) {
			it = pops_by_def.insert(
				{
					pop.m_def->m_name,
					{}
				}
			).first;
		}
		it->second.push_back( i );
	}
	const auto& order = m_game->GetBM()->GetPopDefOrder();
	size_t left = 0;
	for ( const auto& id : order ) {
		const auto& it = pops_by_def.find( id );
		if ( it != pops_by_def.end() ) {
			for ( const auto& i : it->second ) {
				const auto& pop = pops.at( i );
				NEWV( icon, ::ui::object::Surface, SubClass( "Icon" ) );
				icon->SetLeft( ( left++ ) * w );
				// TODO: SetTexture doesn't work if used before AddChild, fix
				AddChild( icon );
				icon->SetTexture( pop.GetTexture() );
				m_pops.push_back(
					{
						i,
						icon
					}
				);
			}
		}
	}
	ASSERT_NOLOG( m_pops.size() == pops_count, "resulting pops size mismatch" );
}

void Population::HideIcons() {
	for ( const auto& pop : m_pops ) {
		RemoveChild( pop.icon );
	}
	m_pops.clear();
}

}
}
}
}
}
}
