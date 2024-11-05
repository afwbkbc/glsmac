#include "Binding.h"

#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Object.h"
#include "gse/type/Array.h"
#include "gse/type/String.h"
#include "gse/type/Int.h"
#include "gse/type/Float.h"
#include "gse/type/Undefined.h"
#include "gse/type/Bool.h"
#include "Bindings.h"
#include "game/backend/State.h"
#include "engine/Engine.h"
#include "loader/txt/TXTLoaders.h"
#include "loader/txt/FactionTXTLoader.h"
#include "loader/texture/TextureLoader.h"
#include "types/texture/Texture.h"
#include "game/backend/Faction.h"
#include "game/backend/Factions.h"

#include "types/Color.h"

namespace game {
namespace backend {
namespace bindings {

BINDING_IMPL( factions ) {
	auto* factions = m_bindings->GetState()->GetFactions();
	const gse::type::object_properties_t properties = {
		{
			"import_base_names",
			NATIVE_CALL() {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( filename, 0, String );
				const auto& data = g_engine->GetTXTLoaders()->factions->GetFactionData( filename );
				std::vector< gse::Value > land_names = {};
				land_names.reserve( data.bases_names.land.size() );
				for ( const auto& name : data.bases_names.land ) {
					land_names.push_back( VALUE( gse::type::String, name ) );
				}
				std::vector< gse::Value > water_names = {};
				water_names.reserve( data.bases_names.water.size() );
				for ( const auto& name : data.bases_names.water ) {
					water_names.push_back( VALUE( gse::type::String, name ) );
				}
				const auto properties = gse::type::object_properties_t{
					{ "land", VALUE( gse::type::Array, land_names ) },
					{ "water", VALUE( gse::type::Array, water_names ) },
				};
				return VALUE( gse::type::Object, properties );
			} )
		},
		{
			"import_colors",
			NATIVE_CALL() {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( filename, 0, String );
				const auto* texture = g_engine->GetTextureLoader()->LoadCustomTexture( filename );
				const auto properties = gse::type::object_properties_t{
					{ "faction", types::Color::FromRGBA( texture->GetPixel( 4, 739 ) ).Wrap() },
					{ "faction_shadow", types::Color::FromRGBA( texture->GetPixel( 4, 747 ) ).Wrap() },
					{ "text", types::Color::FromRGBA( texture->GetPixel( 4, 755 ) ).Wrap() },
					{ "text_shadow", types::Color::FromRGBA( texture->GetPixel( 4, 763 ) ).Wrap() },
					{ "border", types::Color::FromRGBA( texture->GetPixel( 161, 749 ) ).Wrap() },
					{ "border_alpha", types::Color::FromRGBA( texture->GetPixel( 161, 757 ) ).Wrap() },
					{ "vehicle", types::Color::FromRGBA( texture->GetPixel( 435, 744 ) ).Wrap() },
				};
				return VALUE( gse::type::Object, properties );
			} )
		},
	};
	return VALUE( gse::type::Object, properties );
}

}
}
}
