#pragma once

#include <SFML/Graphics.hpp>

#include "mem.h"
#include "resources.h"

using sf::Texture;
using sf::Sprite;

class TextureManager : public Manager<Texture>
{
private:
	const Resource::Folder* _folder;

public:
	TextureManager(const Resource::Folder& folder = Resource::TEXTURES, TextureManager* parent = nullptr);
	virtual ~TextureManager();

	Texture* load(const std::string& id, const Path& file, const sf::IntRect& area);
	Texture* load(const std::string& id, const std::string& file, const sf::IntRect& area);
};
