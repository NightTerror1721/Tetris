#pragma once

#include "common.h"
#include "game_basics.h"

using sf::Sprite;
using sf::Texture;


struct TextureInfo : public utils::JsonSerializable
{
	String file, name;
	Offset x, y;
	Size width, height;

	Json serialize() const override;
	void deserialize(const Json& json) override;

	static TextureInfo read(const Json& json);
};



class TextureManager : public SingleTypeManager<Texture>
{
private:
	resource::Folder _dir;

private:
	TextureManager(TextureManager& parent, const resource::Folder& directory);

public:
	TextureManager(const resource::Folder& directory = resource::textures);
	TextureManager(const TextureManager&) = delete;
	TextureManager(TextureManager&&) noexcept = default;
	~TextureManager() = default;

	TextureManager& operator= (const TextureManager&) = delete;
	TextureManager& operator= (TextureManager&&) noexcept = default;


	inline TextureManager createChild(const resource::Folder& directory = resource::textures) { return { *this, directory }; }

	inline Texture& load(const String& filename, const String& name, const IntRect& area = {})
	{
		return _load(filename, name, area);
	}
	inline Texture& load(const Path& path, const String& name, const IntRect& area = {})
	{
		return _load(path, name, area);
	}

	Texture& load(const TextureInfo& tinfo);

	inline Texture& load(const TextureInfo& tinfo, const String& name) { TextureInfo info = tinfo; return info.name = name, load(info); }

private:
	Texture& _load(const Path path, const String& name, const IntRect& area);
};

namespace global
{
	extern TextureManager textures;
}




