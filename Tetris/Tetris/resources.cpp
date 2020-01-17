#include "resources.h"

#include <fstream>
#include <iostream>


const Resource::Folder Resource::ROOT{ "data" };
const Resource::Folder Resource::FONT{ Resource::ROOT, "font" };
const Resource::Folder Resource::TEXTURES{ Resource::ROOT, "textures" };
const Resource::Folder Resource::THEMES{ Resource::TEXTURES, "themes" };


Resource::Folder::Folder(const std::string& spath) :
	_base{ spath }
{
	fs::create_directories(_base);
}
Resource::Folder::Folder(const Folder& base, const std::string& spath) :
	_base{ base._base / Path{spath} }
{
	fs::create_directories(_base);
}

void Resource::Folder::readAndInject(JsonSerializable& object, const Path& file) const
{
	bool error = false;
	Path path = _file(file, true, error);
	if (error)
		return;

	try { json_utils::read_and_inject(object, path); }
	catch (JsonException ex) { std::cerr << ex.what() << std::endl; }
	catch (...) {}
}
void Resource::Folder::readAndInject(JsonSerializable& object, const std::string& file) const
{
	readAndInject(object, Path{ file });
}
void Resource::Folder::readAndInject(JsonSerializable& object, const char* file) const
{
	readAndInject(object, Path{ file });
}

void Resource::Folder::extractAndWrite(const JsonSerializable& object, const Path& file) const
{
	bool error = false;
	Path path = _file(file, false, error);
	if (error)
		return;

	try { json_utils::extract_and_write(object, path); }
	catch (JsonException ex) { std::cerr << ex.what() << std::endl; }
	catch (...) {}
}
void Resource::Folder::extractAndWrite(const JsonSerializable& object, const std::string& file) const
{
	extractAndWrite(object, Path{ file });
}
void Resource::Folder::extractAndWrite(const JsonSerializable& object, const char* file) const
{
	extractAndWrite(object, Path{ file });
}

bool Resource::Folder::readFile(const Path& file, std::function<void(std::ifstream&)>& action) const
{
	bool error = false;
	Path path = _file(file, true, error);
	if (error)
		return false;

	std::ifstream f{ path, std::ios::in };
	action(f);
	f.close();
	return true;
}
bool Resource::Folder::readFile(const std::string& file, std::function<void(std::ifstream&)>& action) const
{
	return readFile(Path{ file }, action);
}
bool Resource::Folder::readFile(const char* file, std::function<void(std::ifstream&)>& action) const
{
	return readFile(Path{ file }, action);
}

bool Resource::Folder::writeFile(const Path& file, std::function<void(std::ofstream&)>& action) const
{
	bool error = false;
	Path path = _file(file, false, error);
	if (error)
		return false;

	std::ofstream f{ path, std::ios::out };
	action(f);
	f.close();
	return true;
}
bool Resource::Folder::writeFile(const std::string& file, std::function<void(std::ofstream&)>& action) const
{
	return writeFile(Path{ file }, action);
}
bool Resource::Folder::writeFile(const char* file, std::function<void(std::ofstream&)>& action) const
{
	return writeFile(Path{ file }, action);
}

Path Resource::Folder::getPath(const Path& file, bool& error) const { return _file(file, true, error); }
Path Resource::Folder::getPath(const std::string& file, bool& error) const { return _file(Path{ file }, true, error); }
Path Resource::Folder::getPath(const char* file, bool& error) const { return _file(Path{ file }, true, error); }

Path Resource::Folder::getPath(const Path& file) const { bool error; return _file(file, false, error); }
Path Resource::Folder::getPath(const std::string& file) const { bool error; return _file(Path{ file }, false, error); }
Path Resource::Folder::getPath(const char* file) const { bool error; return _file(Path{ file }, false, error); }

Path Resource::Folder::_file(const Path& other, bool check_exists, bool& error) const
{
	Path fpath = _base / other;
	error = check_exists && (!fs::exists(fpath) || !fs::is_regular_file(fpath));
	return std::move(fpath);
}


sf::Font Resource::loadFont(const std::string& fontName)
{
	sf::Font font;
	font.loadFromFile(Resource::FONT.getPath(fontName + ".ttf").string());
	return std::move(font);
}
