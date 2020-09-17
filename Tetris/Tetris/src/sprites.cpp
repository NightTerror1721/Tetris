#include "sprites.h"


TextureManager global::textures;

TextureManager::TextureManager(TextureManager& parent, const resource::Folder& directory) :
	SingleTypeManager{ &parent },
	_dir{ directory }
{}

TextureManager::TextureManager(const resource::Folder& directory) :
	SingleTypeManager{},
	_dir{ directory }
{}

Texture& TextureManager::_load(const Path path, const String& name, const IntRect& area)
{
	erase(name);

	Texture* t = create(name);

	if (!t)
		throw std::exception{ "An error has been ocurred during texture creation." };

	if (!t->loadFromFile(_dir.pathOf(path).string(), area))
		throw std::exception{ "An error has been ocurred during texture load." };

	return *t;
}

Texture& TextureManager::load(const TextureInfo& tinfo)
{
	return _load(tinfo.file, tinfo.name, { 
		static_cast<int>(tinfo.x),
		static_cast<int>(tinfo.y),
		static_cast<int>(tinfo.width),
		static_cast<int>(tinfo.height)
	});
}


Json TextureInfo::serialize() const
{
	Json json = Json::object();

	json["file"] = file;
	json["name"] = name;
	json["x"] = x;
	json["y"] = y;
	json["width"] = width;
	json["height"] = height;

	return json;
}
void TextureInfo::deserialize(const Json& json)
{
	if (json.is_object())
	{
		file = utils::opt<String>(json, "file", "");
		name = utils::opt<String>(json, "name", "");
		x = utils::opt<Offset>(json, "x", 0);
		y = utils::opt<Offset>(json, "y", 0);
		width = utils::opt<Size>(json, "width", 0);
		height = utils::opt<Size>(json, "height", 0);
	}
	else if (json.is_string())
	{
		file = json.get<String>();
		name = "";
		x = y = width = height = 0;
	}
}

TextureInfo TextureInfo::read(const Json& json)
{
	TextureInfo info;
	info.deserialize(json);
	return info;
}
