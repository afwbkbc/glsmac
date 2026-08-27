#include "Tile.h"

#include "gse/value/Object.h"
#include "gse/value/Array.h"
#include "gse/value/Int.h"
#include "gse/value/Bool.h"
#include "gse/value/Null.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"

#include "Tiles.h"
#include "game/backend/Game.h"
#include "game/backend/map/Map.h"
#include "game/backend/unit/Unit.h"
#include "game/backend/base/Base.h"
#include "game/backend/base/Pop.h"
#include "game/backend/map/tile/TileManager.h"
#include "game/backend/resource/ResourceManager.h"
#include "game/backend/slot/Slot.h"
#include "game/backend/State.h"
#include "game/backend/Player.h"

#include "util/String.h"

namespace game {
namespace backend {
namespace map {
namespace tile {

const std::unordered_map< direction_t, std::string > Tile::s_direction_str = {
#define X( _x ) { D_##_x, #_x },
	{
		X( W )
		X( NW )
		X( N )
		X( NE )
		X( E )
		X( SE )
		X( S )
		X( SW )
	}
#undef X
};

static const std::unordered_map< std::string, terraforming_t > s_terraforming_by_name = {
#define X_TERRAFORMING( _x, _i ) { util::String::GetLowerCase( #_x ), TERRAFORMING_ ## _x },
	X_TERRAFORMINGS
#undef X_TERRAFORMING
};

static const std::unordered_map< std::string, feature_t > s_feature_by_name = {
#define X_FEATURE( _x, _i ) { util::String::GetLowerCase( #_x ), FEATURE_ ## _x },
	X_FEATURES
#undef X_FEATURE
};

static const std::unordered_map< std::string, bonus_t > s_bonus_by_name = {
	{ "none", BONUS_NONE },
#define X_BONUS( _x, _i ) { util::String::GetLowerCase( #_x ), BONUS_ ## _x },
	X_BONUSES
#undef X_BONUS
};

static const std::unordered_map< terraforming_t, std::string > s_terraforming_names = {
	{ TERRAFORMING_NONE, "none" },
#define X_TERRAFORMING( _x, _i ) { TERRAFORMING_ ## _x, util::String::GetLowerCase( #_x ) },
	X_TERRAFORMINGS
#undef X_TERRAFORMING
};

const std::string& Tile::GetDirectionString( const direction_t direction ) {
	const auto& s = s_direction_str.find( direction );
	ASSERT( s != s_direction_str.end(), "unknown direction: " + std::to_string( direction ) );
	return s->second;
}

Tile* Tile::GetNeighbour( const direction_t direction ) {
	switch ( direction ) {
		case D_NONE: return this;
		case D_W: return W;
		case D_NW: return NW;
		case D_N: return N;
		case D_NE: return NE;
		case D_E: return E;
		case D_SE: return SE;
		case D_S: return S;
		case D_SW: return SW;
		default:
			THROW( "unknown tile direction: " + std::to_string( direction ) );
	}
}

void Tile::Update() {

	*elevation.center = ( *elevation.left + *elevation.top + *elevation.right + *elevation.bottom ) / 4;

	const auto sea_level = tiles && tiles->GetMap()
		? tiles->GetMap()->GetSeaLevel()
		: ELEVATION_LEVEL_COAST;
	uint8_t corners_in_water = *elevation.center < sea_level
		? 1
		: 0;
	for ( auto& c : elevation.corners ) {
		if ( *c < sea_level ) {
			corners_in_water++;
		}
	}

	is_water_tile = corners_in_water > 2;
}

void Tile::RefreshWrappers() {
	std::lock_guard guard( m_wrapobjs_mutex );
	for ( auto* const wrapobj : m_wrapobjs ) {
		const auto f_get_property = [ wrapobj ]( const std::string& name, const gse::value_type_t type ) -> gse::Value* {
			const auto it = wrapobj->value.find( name );
			ASSERT( it != wrapobj->value.end(), "tile wrapper has no " + name + " property" );
			ASSERT( it->second->type == type, "tile wrapper " + name + " property has an invalid type" );
			return it->second;
		};

		( (gse::value::Bool*)f_get_property( "is_water", gse::VT_BOOL ) )->value = is_water_tile;
		( (gse::value::Bool*)f_get_property( "is_land", gse::VT_BOOL ) )->value = !is_water_tile;
		( (gse::value::Int*)f_get_property( "moisture", gse::VT_INT ) )->value = moisture;
		( (gse::value::Int*)f_get_property( "rockiness", gse::VT_INT ) )->value = rockiness;
		( (gse::value::Int*)f_get_property( "elevation", gse::VT_INT ) )->value = *elevation.center;
		( (gse::value::Int*)f_get_property( "sea_level", gse::VT_INT ) )->value =
			tiles && tiles->GetMap()
				? tiles->GetMap()->GetSeaLevel()
				: ELEVATION_LEVEL_COAST;

		auto* const wrapped_features = (gse::value::Object*)f_get_property( "features", gse::VT_OBJECT );
#define X_FEATURE( _x, _i ) \
		{ \
			const auto& flag_it = wrapped_features->value.find( util::String::GetLowerCase( #_x ) ); \
			ASSERT( flag_it != wrapped_features->value.end(), "tile wrapper has no feature flag" ); \
			ASSERT( flag_it->second->type == gse::VT_BOOL, "tile feature flag is not a bool" ); \
			( (gse::value::Bool*)flag_it->second )->value = ( features & FEATURE_ ## _x ) != 0; \
		}
		X_FEATURES
#undef X_FEATURE

		auto* const wrapped_bonuses = (gse::value::Object*)f_get_property( "bonuses", gse::VT_OBJECT );
#define X_BONUS( _x, _i ) \
		{ \
			const auto& flag_it = wrapped_bonuses->value.find( util::String::GetLowerCase( #_x ) ); \
			ASSERT( flag_it != wrapped_bonuses->value.end(), "tile wrapper has no bonus flag" ); \
			ASSERT( flag_it->second->type == gse::VT_BOOL, "tile bonus flag is not a bool" ); \
			( (gse::value::Bool*)flag_it->second )->value = bonus == BONUS_ ## _x; \
		}
		X_BONUSES
#undef X_BONUS

		auto* const wrapped_terraforming = (gse::value::Object*)f_get_property( "terraforming", gse::VT_OBJECT );
#define X_TERRAFORMING( _x, _i ) \
		{ \
			const auto& flag_it = wrapped_terraforming->value.find( util::String::GetLowerCase( #_x ) ); \
			ASSERT( flag_it != wrapped_terraforming->value.end(), "tile wrapper has no terraforming flag" ); \
			ASSERT( flag_it->second->type == gse::VT_BOOL, "tile terraforming flag is not a bool" ); \
			( (gse::value::Bool*)flag_it->second )->value = ( terraforming & TERRAFORMING_ ## _x ) != 0; \
		}
		X_TERRAFORMINGS
#undef X_TERRAFORMING
	}
}

void Tile::Clear() {
	for ( auto& c : elevation.corners ) {
		*c = 0;
	}
	moisture = rockiness = bonus = features = terraforming = is_water_tile = 0;
}

const bool Tile::IsAdjactentTo( const Tile* other ) const {
	for ( const auto& n : neighbours ) {
		if ( n == other ) {
			return true;
		}
	}
	return false;
}

const types::Buffer Tile::Serialize() const {
	types::Buffer buf;

	buf.WriteInt( coord.x );
	buf.WriteInt( coord.y );

	const auto center = (
		*elevation.left +
		*elevation.top +
		*elevation.right +
		*elevation.bottom
	) / 4;
	buf.WriteInt( center );
	buf.WriteInt( *elevation.left );
	buf.WriteInt( *elevation.top );
	buf.WriteInt( *elevation.right );
	buf.WriteInt( *elevation.bottom );

	buf.WriteInt( moisture );
	buf.WriteInt( rockiness );
	buf.WriteInt( bonus );

	buf.WriteInt( features );
	buf.WriteInt( terraforming );

	return buf;
}

void Tile::Deserialize( types::Buffer buf ) {
	if (
		!elevation.center ||
		!elevation.left ||
		!elevation.top ||
		!elevation.right ||
		!elevation.bottom
	) {
		THROW( "cannot deserialize an unlinked tile" );
	}

	const auto x = buf.ReadInt< size_t >( "tile x coordinate" );
	const auto y = buf.ReadInt< size_t >( "tile y coordinate" );

	const auto center = buf.ReadInt< elevation_t >( "tile center elevation" );
	const auto left = buf.ReadInt< elevation_t >( "tile left elevation" );
	const auto top = buf.ReadInt< elevation_t >( "tile top elevation" );
	const auto right = buf.ReadInt< elevation_t >( "tile right elevation" );
	const auto bottom = buf.ReadInt< elevation_t >( "tile bottom elevation" );
	if (
		center < ELEVATION_MIN || center > ELEVATION_MAX ||
		left < ELEVATION_MIN || left > ELEVATION_MAX ||
		top < ELEVATION_MIN || top > ELEVATION_MAX ||
		right < ELEVATION_MIN || right > ELEVATION_MAX ||
		bottom < ELEVATION_MIN || bottom > ELEVATION_MAX
	) {
		THROW( "invalid serialized tile elevation" );
	}
	if ( center != ( left + top + right + bottom ) / 4 ) {
		THROW( "serialized tile center elevation does not match its corners" );
	}

	const auto serialized_moisture = buf.ReadInt< moisture_t >( "tile moisture" );
	const auto serialized_rockiness = buf.ReadInt< rockiness_t >( "tile rockiness" );
	const auto serialized_bonus = buf.ReadInt< bonus_t >( "tile bonus" );
	if ( serialized_moisture > MOISTURE_RAINY ) {
		THROW( "invalid serialized tile moisture" );
	}
	if ( serialized_rockiness > ROCKINESS_ROCKY ) {
		THROW( "invalid serialized tile rockiness" );
	}
	if ( serialized_bonus > BONUS_MINERALS ) {
		THROW( "invalid serialized tile bonus" );
	}

	const auto serialized_features = buf.ReadInt< feature_t >( "tile features" );
	const auto serialized_terraforming = buf.ReadInt< terraforming_t >( "tile terraforming" );
	if ( serialized_features & static_cast< feature_t >( ~FEATURE_ALL ) ) {
		THROW( "invalid serialized tile features" );
	}
	if ( serialized_terraforming & static_cast< terraforming_t >( ~TERRAFORMING_ALL ) ) {
		THROW( "invalid serialized tile terraforming" );
	}
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized tile" );
	}

	coord = { x, y };
	*elevation.center = center;
	*elevation.left = left;
	*elevation.top = top;
	*elevation.right = right;
	*elevation.bottom = bottom;
	moisture = serialized_moisture;
	rockiness = serialized_rockiness;
	bonus = serialized_bonus;
	features = serialized_features;
	terraforming = serialized_terraforming;

	Update();
}

WRAPIMPL_SERIALIZE( Tile )
	buf->WriteInt( obj->coord.x );
	buf->WriteInt( obj->coord.y );
}

WRAPIMPL_DESERIALIZE( Tile )
	const auto tile_x = buf->ReadInt< size_t >( "tile reference x coordinate" );
	const auto tile_y = buf->ReadInt< size_t >( "tile reference y coordinate" );
	const auto* const map = game->GetMap();
	if (
		tile_x >= map->GetWidth() ||
		tile_y >= map->GetHeight() ||
		( tile_x & 1 ) != ( tile_y & 1 )
	) {
		THROW( "invalid serialized tile reference" );
	}
	const auto& tile = map->GetTile( tile_x, tile_y );
	return tile->Wrap( GSE_CALL );
}

const std::string Tile::ToString() const {
	return "@[ " + std::to_string( coord.x ) + " " + std::to_string( coord.y ) + " ]";
}

feature_t Tile::GetFeatureFromString( const std::string& name ) {
	const auto& it = s_feature_by_name.find( util::String::GetLowerCase( name ) );
	return it == s_feature_by_name.end()
		? FEATURE_NONE
		: it->second;
}

bonus_t Tile::GetBonusFromString( const std::string& name ) {
	const auto& it = s_bonus_by_name.find( util::String::GetLowerCase( name ) );
	return it == s_bonus_by_name.end()
		? BONUS_NONE
		: it->second;
}

terraforming_t Tile::GetTerraformingFromString( const std::string& name ) {
	if ( util::String::GetLowerCase( name ) == "none" ) {
		return TERRAFORMING_NONE;
	}
	const auto& it = s_terraforming_by_name.find( util::String::GetLowerCase( name ) );
	return it == s_terraforming_by_name.end()
		? TERRAFORMING_NONE
		: it->second;
}

const std::string& Tile::GetTerraformingString( const terraforming_t value ) {
	const auto& it = s_terraforming_names.find( value );
	if ( it == s_terraforming_names.end() ) {
		THROW( "unknown single terraforming value: " + std::to_string( value ) );
	}
	return it->second;
}

void Tile::SetFeatures( GSE_CALLABLE, const feature_t value ) {
	if ( value & static_cast< feature_t >( ~FEATURE_ALL ) ) {
		GSE_ERROR( gse::EC.INVALID_CALL, "Invalid tile features value: " + std::to_string( value ) );
	}
	tiles->GetMap()->GetGame()->CheckRW( GSE_CALL );
	if ( features != value ) {
		features = value;
		RefreshWrappers();
		tiles->GetMap()->RefreshTile( this );
	}
}

void Tile::SetBonus( GSE_CALLABLE, const bonus_t value ) {
	if ( value > BONUS_MINERALS ) {
		GSE_ERROR( gse::EC.INVALID_CALL, "Invalid tile bonus value: " + std::to_string( value ) );
	}
	tiles->GetMap()->GetGame()->CheckRW( GSE_CALL );
	if ( bonus != value ) {
		bonus = value;
		RefreshWrappers();
		tiles->GetMap()->RefreshTile( this );
	}
}

void Tile::SetTerraforming( GSE_CALLABLE, const terraforming_t value ) {
	if ( value & static_cast< terraforming_t >( ~TERRAFORMING_ALL ) ) {
		GSE_ERROR( gse::EC.INVALID_CALL, "Invalid tile terraforming value: " + std::to_string( value ) );
	}
	tiles->GetMap()->GetGame()->CheckRW( GSE_CALL );
	if ( terraforming != value ) {
		terraforming = value;
		RefreshWrappers();
		tiles->GetMap()->RefreshTile( this );
	}
}

bool Tile::HasWorkingPopLink() const {
	return const_cast< Tile* >( this )->CustomHas( "working_pop" );
}

base::Pop* Tile::GetWorkingPop() const {
	auto* const value = const_cast< Tile* >( this )->CustomGet( "working_pop" );
	if ( !value ) {
		return nullptr;
	}
	auto* const dereferenced = value->type == gse::VT_OBJECT
		? value
		: value->Deref();
	if (
		dereferenced->type != gse::VT_OBJECT ||
		( (gse::value::Object*)dereferenced )->object_class != base::Pop::WRAP_CLASS
	) {
		return nullptr;
	}
	return (base::Pop*)( (gse::value::Object*)dereferenced )->wrapobj;
}

void Tile::SetWorkingPop( GSE_CALLABLE, base::Pop* const pop ) {
	if ( !pop ) {
		GSE_ERROR( gse::EC.INVALID_CALL, "working population is null" );
	}
	auto* const value = CustomGet( "working_pop" );
	if ( value ) {
		auto* const dereferenced = value->Deref();
		if (
			dereferenced->type != gse::VT_OBJECT ||
			( (gse::value::Object*)dereferenced )->object_class != base::Pop::WRAP_CLASS ||
			( (gse::value::Object*)dereferenced )->wrapobj != pop
		) {
			GSE_ERROR( gse::EC.GAME_ERROR, "tile already has another working population" );
		}
		return;
	}
	CustomSet( "working_pop", pop->Wrap( GSE_CALL ) );
}

void Tile::UnsetWorkingPop( GSE_CALLABLE, const base::Pop* const pop ) {
	auto* const value = CustomGet( "working_pop" );
	if (
		!value ||
		value->type != gse::VT_OBJECT ||
		( (gse::value::Object*)value )->wrapobj != pop
	) {
		GSE_ERROR( gse::EC.GAME_ERROR, "tile working population does not match" );
	}
	CustomUnset( "working_pop" );
}

#define GETN( _n ) \
{ \
	"get_" #_n, \
	NATIVE_CALL( this ) { return _n->Wrap( GSE_CALL ); } ) \
}

WRAPIMPL_BEGIN( Tile )
	WRAPIMPL_PROPS
	WRAPIMPL_CUSTOM_SETTERS
		{
			"x",
			VALUE( gse::value::Int,, coord.x )
		},
		{
			"y",
			VALUE( gse::value::Int,, coord.y )
		},
		{
			"is_locked",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS(0);
				return VALUE( gse::value::Bool,, m_is_locked );
			} )
		},
		{
			"is_water",
			VALUE( gse::value::Bool,, is_water_tile )
		},
		{
			"is_land",
			VALUE( gse::value::Bool,, !is_water_tile )
		},
		{
			"moisture",
			VALUE( gse::value::Int,, moisture )
		},
		{
			"rockiness",
			VALUE( gse::value::Int,, rockiness )
		},
		{
			"elevation",
			VALUE( gse::value::Int,, *elevation.center )
		},
		{
			"sea_level",
			VALUE( gse::value::Int,, tiles && tiles->GetMap()
				? tiles->GetMap()->GetSeaLevel()
				: ELEVATION_LEVEL_COAST )
		},
		GETN( W ),
		GETN( NW ),
		GETN( N ),
		GETN( NE ),
		GETN( E ),
		GETN( SE ),
		GETN( S ),
		GETN( SW ),
		{
			"is_adjactent_to",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE_UNWRAP( other, 0, Tile );
				return VALUE( gse::value::Bool,, IsAdjactentTo( other ) );
			} )
		},
		{
			"get_surrounding_tiles",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );
				gse::value::array_elements_t result = {};
				for ( const auto& n : neighbours ) {
					result.push_back( n->Wrap( GSE_CALL ) );
				}
				return VALUE( gse::value::Array,, result );
			} )
		},
		{ "features", GetFeatures( GSE_CALL ) },
		{ "bonuses", GetBonuses( GSE_CALL ) },
		{ "terraforming", GetTerraformings( GSE_CALL ) },
		{
			"update_features",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( changes, 0, Object );
				auto updated = features;
				for ( const auto& change : changes ) {
					const auto flag = GetFeatureFromString( change.first );
					if ( flag == FEATURE_NONE || change.second->type != gse::VT_BOOL ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "Invalid feature update: " + change.first );
					}
					if ( ( (gse::value::Bool*)change.second )->value ) {
						updated |= flag;
					}
					else {
						updated &= static_cast< feature_t >( ~flag );
					}
				}
				SetFeatures( GSE_CALL, updated );
				return VALUE( gse::value::Undefined );
			} )
		},
		{
			"set_bonus",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( name, 0, String );
				const auto value = GetBonusFromString( name );
				if ( value == BONUS_NONE && util::String::GetLowerCase( name ) != "none" ) {
					GSE_ERROR( gse::EC.INVALID_CALL, "Invalid tile bonus: " + name );
				}
				SetBonus( GSE_CALL, value );
				return VALUE( gse::value::Undefined );
			} )
		},
		{
			"update_terraforming",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( changes, 0, Object );
				auto updated = terraforming;
				for ( const auto& change : changes ) {
					const auto flag = GetTerraformingFromString( change.first );
					if ( flag == TERRAFORMING_NONE || change.second->type != gse::VT_BOOL ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "Invalid terraforming update: " + change.first );
					}
					if ( ( (gse::value::Bool*)change.second )->value ) {
						updated |= flag;
					}
					else {
						updated &= static_cast< terraforming_t >( ~flag );
					}
				}
				SetTerraforming( GSE_CALL, updated );
				return VALUE( gse::value::Undefined );
			} )
		},
		{
			"get_units",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS_MAX( 1 );
				bool include_embarked = false;
				if ( !arguments.empty() ) {
					N_GETVALUE( requested_include_embarked, 0, Bool );
					include_embarked = requested_include_embarked;
				}
				gse::value::array_elements_t result = {};
				for ( auto& it : units ) {
					if ( include_embarked || it.second->m_transport_id == 0 ) {
						result.push_back( it.second->Wrap( GSE_CALL ) );
					}
				}
				return VALUE( gse::value::Array,, result );
			} )
		},
		{
			"get_base",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );
				if ( base ) {
					return base->Wrap( GSE_CALL );
				}
				else {
					return VALUE( gse::value::Null );
				}
			} )
		},
		{
			"get_resources",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS_MAX( 1 );
				slot::Slot* slot;
				if ( arguments.size() > 0 ) {
					N_GETVALUE_UNWRAP( player, 0, Player );
					slot = player->GetSlot();
				}
				else {
					slot = tiles->GetMap()->GetGame()->GetPlayer()->GetSlot();
				}
				return GetResourcesAsValue( GSE_CALL, slot );
			} )
		},
	};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( Tile )

void Tile::Lock( const size_t initiator_slot ) {
	ASSERT( !m_is_locked, "tile already locked" );
	m_lock_initiator_slot = initiator_slot;
	m_is_locked = true;
}
void Tile::Unlock() {
	ASSERT( m_is_locked, "tile not locked" );
	m_is_locked = false;
}
const bool Tile::IsLocked() const {
	return m_is_locked;
}
const bool Tile::IsLockedBy( const size_t initiator_slot ) const {
	return m_is_locked && m_lock_initiator_slot == initiator_slot;
}

const Tile::resources_t Tile::GetResources( GSE_CALLABLE, slot::Slot* const slot ) {
	auto* const result = GetResourcesAsValue( GSE_CALL, slot );
	resources_t resources = {};
	for ( const auto& it : ((gse::value::Object*)result)->value ) {
		resources.insert_or_assign( it.first, ((gse::value::Int*)it.second)->value );
	}
	return resources;
}

gse::Value* const Tile::GetFeatures( GSE_CALLABLE ) const {
	gse::value::object_properties_t result = {};
#define X_FEATURE( _x, _i ) \
	result.insert_or_assign(   \
		util::String::GetLowerCase( # _x ), \
		VALUE( gse::value::Bool,, features & backend::map::tile::FEATURE_ ## _x ) \
	);
X_FEATURES
#undef X_FEATURE
	return VALUE( gse::value::Object,, GSE_CALL_NOGC, result );
}

gse::Value* const Tile::GetBonuses( GSE_CALLABLE ) const {
	gse::value::object_properties_t result = {};
#define X_BONUS( _x, _i ) \
	result.insert_or_assign(   \
		util::String::GetLowerCase( # _x ), \
		VALUE( gse::value::Bool,, bonus == backend::map::tile::BONUS_ ## _x ) \
	);
	X_BONUSES
#undef X_BONUS
	return VALUE( gse::value::Object,, GSE_CALL_NOGC, result );
}

gse::Value* const Tile::GetTerraformings( GSE_CALLABLE ) const {
	gse::value::object_properties_t result = {};
#define X_TERRAFORMING( _x, _i ) \
	result.insert_or_assign( \
		util::String::GetLowerCase( #_x ), \
		VALUE( gse::value::Bool,, terraforming & TERRAFORMING_ ## _x ) \
	);
	X_TERRAFORMINGS
#undef X_TERRAFORMING
	return VALUE( gse::value::Object,, GSE_CALL_NOGC, result );
}

gse::value::Object* const Tile::GetResourcesAsValue( GSE_CALLABLE, slot::Slot* const slot ) {
	ASSERT( tiles, "tiles not set" );
	auto* const game = tiles->GetMap()->GetGame();
	return GetResourcesFromCallback( GSE_CALL, game->GetTM(), game->GetRM(),"get_tile_resources", ARGS_F( this, &slot ) {
		{
			"tile",
			Wrap( GSE_CALL )
		},
		{
			"player",
			slot->Wrap( GSE_CALL )
		},
	}; } );
}

}
}
}
}
