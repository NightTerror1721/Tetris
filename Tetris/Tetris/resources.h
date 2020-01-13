#pragma once

#include <filesystem>
#include <functional>
#include <string>

#include <SFML/Graphics/Font.hpp>

#include "json.h"

namespace fs = std::filesystem;

using Path = fs::path;

namespace Resource
{
	class Folder
	{
	private:
		Path _base;

	public:
		Folder(const std::string& spath);
		Folder(const Folder& base, const std::string& spath);

		void readAndInject(JsonSerializable& object, const Path& file) const;
		void readAndInject(JsonSerializable& object, const std::string& file) const;
		void readAndInject(JsonSerializable& object, const char* file) const;

		void extractAndWrite(const JsonSerializable& object, const Path& file) const;
		void extractAndWrite(const JsonSerializable& object, const std::string& file) const;
		void extractAndWrite(const JsonSerializable& object, const char* file) const;

		bool readFile(const Path& file, std::function<void(std::ifstream&)>& action) const;
		bool readFile(const std::string& file, std::function<void(std::ifstream&)>& action) const;
		bool readFile(const char* file, std::function<void(std::ifstream&)>& action) const;

		bool writeFile(const Path& file, std::function<void(std::ofstream&)>& action) const;
		bool writeFile(const std::string& file, std::function<void(std::ofstream&)>& action) const;
		bool writeFile(const char* file, std::function<void(std::ofstream&)>& action) const;

		Path getPath(const Path& file, bool& error) const;
		Path getPath(const std::string& file, bool& error) const;
		Path getPath(const char* file, bool& error) const;

		Path getPath(const Path& file) const;
		Path getPath(const std::string& file) const;
		Path getPath(const char* file) const;

	private:
		Path _file(const Path& other, bool check_exists, bool& error) const;
	};

	extern const Folder ROOT;
	extern const Folder FONT;
}

/* Special functions */
namespace Resource
{
	sf::Font loadFont(const std::string& fontName);
}
