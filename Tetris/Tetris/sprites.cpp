#include "sprites.h"

TextureManager::TextureManager(const Resource::Folder& folder, TextureManager* parent) :
	Manager{ parent },
	_folder{ &folder }
{}
TextureManager::~TextureManager() {}

Texture* TextureManager::load(const std::string& id, const Path& file, const sf::IntRect& area)
{
	Texture* t = create<Texture>(id);
	if (t)
	{
		bool error;
		Path path = _folder->getPath(file, error);
		return !error && t->loadFromFile(path.string(), area) ? t : nullptr;
	}
	return nullptr;
}
Texture* TextureManager::load(const std::string& id, const std::string& file, const sf::IntRect& area)
{
	return load(id, Path{ file }, area);
}
