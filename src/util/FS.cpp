#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

#include "FS.h"

using namespace std;

namespace util {

	const std::string FS::GetCurrentDirectory() {
		return std::filesystem::current_path().string();
	}

	const bool FS::Exists(const string& path) {
		return std::filesystem::exists(path);
	}

	const bool FS::IsFile(const string& path) {
		return std::filesystem::is_regular_file(path);
	}

	const bool FS::FileExists(const string& path) {
		return Exists(path) && IsFile(path);
	}

	const bool FS::IsDirectory(const string& path) {
		return std::filesystem::is_directory(path);
	}

	const bool FS::DirectoryExists(const string& path) {
		return Exists(path) && IsDirectory(path);
	}

	void FS::CreateDirectoryIfNotExists(const string& path) {
		if (!DirectoryExists(path)) {
			std::filesystem::create_directory(path);
		}
	}

	std::vector< std::string > FS::ListDirectory(const std::string& directory, const bool return_absolute_paths) {
		std::vector< std::string > result = {};

		try {

			std::vector< std::filesystem::path > items;
			std::copy(std::filesystem::directory_iterator(!directory.empty() ? directory : "/"), std::filesystem::directory_iterator(), std::back_inserter(items));
			std::sort(items.begin(), items.end());

			for (const auto& item : items) {
				const auto item_str = item.string();
				ASSERT_NOLOG(item_str.substr(0, directory.size() + 1) == directory + "/", "unexpected path in directory list results: " + item_str);
				result.push_back(return_absolute_paths ? item_str : item_str.substr(directory.size() + 1));
			}
		}
		catch (std::filesystem::filesystem_error& e) {
			// permission denied etc
			// TODO: display error?
			return result;
		}

		return result;
	}

	const string FS::ReadFile(const string& path) {
		ASSERT_NOLOG(FileExists(path), "file \"" + path + "\" does not exist or is not a file");
		stringstream data;
		ifstream in(path);
		data << in.rdbuf();
		in.close();
		return data.str();
	}

	const void FS::WriteFile(const string& path, const string& data) {
		ofstream out(path);
		out << data;
		out.close();
	}

}

