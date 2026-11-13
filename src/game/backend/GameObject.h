#pragma once

// TODO: every object belonging to Game should inherit this

#define WRAPIMPL_GAME_OBJECT \
    { \
        "set", \
        NATIVE_CALL( this ) {\
            ASSERT( m_game, "game not set" ); \
            m_game->CheckRW( GSE_CALL ); /* to ensure all changes are done within events and synced properly in multiplayer */ \
            N_EXPECT_ARGS( 2 ); \
            N_GETVALUE( key, 0, String ); \
            auto* const value = arguments[ 1 ]; \
            CustomSet( key, value ); \
            return VALUE( gse::value::Undefined ); \
        } ) \
    }, \
    { \
        "unset", \
        NATIVE_CALL( this ) { \
            ASSERT( m_game, "game not set" ); \
            m_game->CheckRW( GSE_CALL ); /* to ensure all changes are done within events and synced properly in multiplayer */ \
            N_EXPECT_ARGS( 1 ); \
            N_GETVALUE( key, 0, String ); \
            CustomUnset( key ); \
            return VALUE( gse::value::Undefined ); \
        } ) \
    }, \
    { \
        "has", \
        NATIVE_CALL( this ) { \
            N_EXPECT_ARGS( 1 ); \
            N_GETVALUE( key, 0, String );\
            return VALUE( gse::value::Bool,, CustomHas( key ) ); \
        } ) \
    }, \
    { \
        "get", \
        NATIVE_CALL( this ) { \
            N_EXPECT_ARGS( 1 ); \
            N_GETVALUE( key, 0, String );\
            auto* const value = CustomGet( key ); \
            if ( value ) { \
                return value; \
            } \
            else { \
                return VALUE( gse::value::Undefined ); \
            } \
        } ) \
    },

namespace game {
namespace backend {

namespace map::tile {
class Tiles;
}

class Game;

class GameObject {
public:

	GameObject( const Game* const game );

protected:
	friend class map::tile::Tiles;
	const Game* m_game = nullptr;

};

}
}
