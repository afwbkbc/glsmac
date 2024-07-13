#include "yaml-cpp/yaml.h"

#include "Account.h"

#include "engine/Engine.h"
#include "config/Config.h"

#include "util/FS.h"
#include "util/UUID.h"

namespace game {
namespace backend {

Account::Account() {
	if ( util::FS::FileExists( GetPath() ) ) {
		Load();
	}
	else {
		Create();
	}
}

const std::string& Account::GetGSID() const {
	return m_gsid;
}

const std::string& Account::GetLastPlayerName() const {
	return m_last_values.player_name;
}

void Account::SetLastPlayerName( const std::string& value ) {
	m_last_values.player_name = value;
	Save();
}

const std::string& Account::GetLastGameName() const {
	return m_last_values.game_name;
}

void Account::SetLastGameName( const std::string& value ) {
	m_last_values.game_name = value;
	Save();
}

const std::string& Account::GetLastRemoteAddress() const {
	return m_last_values.remote_address;
}

void Account::SetLastRemoteAddress( const std::string& value ) {
	m_last_values.remote_address = value;
	Save();
}

const std::string Account::GetPath() const {
	return g_engine->GetConfig()->GetPrefix() + "accounts.yml";
}

void Account::Create() {
	m_gsid = util::UUID::Generate();
	Log( "Creating local account " + m_gsid );
	Save();
}

void Account::Save() {
	YAML::Node root; // in future there can be multiple local accounts
	YAML::Node account;
	account[ "gsid" ] = m_gsid;
	YAML::Node last_values;
	last_values[ "player_name" ] = m_last_values.player_name;
	last_values[ "game_name" ] = m_last_values.game_name;
	last_values[ "remote_address" ] = m_last_values.remote_address;
	account[ "last_values" ] = last_values;
	root[ m_gsid ] = account; // gsids are keys
	util::FS::WriteFile( GetPath(), Dump( root ) );
}

void Account::Load() {
	ASSERT( util::FS::FileExists( GetPath() ), "tried to load account but file does not exist" );
	bool load_successful = false; // if file contains unparseable garbage or wrong fields - we'll need to create new account
	bool need_update = false; // if accounts.yml is outdated then we'll load what we can and then save in new format

	YAML::Node root = YAML::LoadFile( GetPath() );
	for ( const auto& it : root ) {
		const auto& gsid = it.first.as< std::string >();
		const auto& account = it.second;
		if ( gsid.empty() || !account[ "gsid" ] ) {
			// invalid account info
			continue;
		}

		m_gsid = account[ "gsid" ].as< std::string >();
		if ( gsid != m_gsid ) {
			// key is gsid too and should match gsid
			continue;
		}

		// other fields are optional and will be added if missing

		if ( !account[ "last_values" ] ) {
			need_update = true;
		}
		else {
			const auto& last_values = account[ "last_values" ];
#define x( _field ) \
            if ( last_values[ #_field ] ) { \
                m_last_values._field = last_values[ #_field ].as< std::string >(); \
            } \
            else { \
                need_update = true; \
            }
			x( player_name )
			x( game_name )
			x( remote_address )
#undef x
		}

		load_successful = true;
		break; // only single account for now
	}
	if ( load_successful ) {
		Log( "Loaded local account " + m_gsid );
		if ( need_update ) {
			Log( "Updating accounts file" );
			Save();
		}
	}
	else {
		Log( "Accounts file corrupted or invalid" );
		Create();
	}
}

}
}
