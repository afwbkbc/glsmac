#pragma once

#include <vector>
#include <string>

#include "common/Module.h"

#include "Types.h"

namespace resource {

CLASS( ResourceManager, common::Module )

	ResourceManager();

	void Init( std::vector< std::string > possible_smac_paths );

	const resource_t GetResource( const std::string& filename ) const;
	const std::string& GetPath( const resource_t res ) const;
	const std::string& GetCustomPath( const std::string& path );

private:

	const std::unordered_map< resource_t, std::string > m_resources_to_filenames = {};
	std::unordered_map< std::string, resource_t > m_filenames_to_resources = {};

	typedef std::unordered_map< std::string, std::string > extension_path_map_t;

	typedef uint8_t path_modifier_t;
	path_modifier_t PM_NONE = 0;
	path_modifier_t PM_SPACES_TO_UNDERSCORES = 1 << 0;

	std::string m_smac_path = "";
	extension_path_map_t m_extension_path_map = {};
	path_modifier_t m_path_modifiers = PM_NONE;
	std::unordered_map< resource_t, std::string > m_resource_paths = {};
	std::unordered_map< std::string, std::string > m_custom_resource_paths = {};

	const std::string GetFixedPath( const std::string& file, const extension_path_map_t& extension_path_map, const path_modifier_t path_modifiers );

	const bool CheckFiles( const std::string& path, const std::vector< std::string >& files );

	const bool ResolveBuiltins( const std::string& path, const extension_path_map_t& extension_path_map, const path_modifier_t path_modifiers );

};

}
