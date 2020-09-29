#include "audio.h"


SoundManager global::sounds;

sf::SoundBuffer& SoundManager::_getBuffer(const Path& path)
{
	String tag = path.string();
	auto it = _buffers.find(tag);

	if (it != _buffers.end())
		return it->second;

	auto result = _buffers.emplace(std::piecewise_construct,
		std::make_tuple(tag),
		std::make_tuple()
	);
	sf::SoundBuffer& buffer = result.first->second;

	if (!buffer.loadFromFile(resource::sound.pathOf(path).string()))
		throw std::exception{ "An error has been ocurred during sound load." };

	return buffer;
}

Sound& SoundManager::_load(const Path& filepath, const String& name)
{
	erase(name);

	Sound* s = create(name);
	if (!s)
		throw std::exception{ "An error has been ocurred during sound creation." };

	sf::SoundBuffer& buffer = _getBuffer(filepath);
	s->setBuffer(buffer);

	return *s;
}

void SoundManager::loadAll()
{
	Json json;
	resource::sound.readJson("config.json", json);

	if (!json.is_object())
		return;

	for (auto it = json.begin(); it != json.end(); it++)
	{
		const Json& value = *it;
		if (!value.is_string())
			continue;

		try { load(value.get<String>(), it.key()); }
		catch (const std::exception& ex) { std::cerr << ex.what() << std::endl; }
	}
}







void SoundController::update()
{
	for(auto& handle : _sounds)
		if (handle.second.play)
		{
			handle.second.sound->play();
			handle.second.play = false;
		}
}

void SoundController::play(const String& soundName)
{
	auto it = _sounds.find(soundName);
	if (it != _sounds.end())
		it->second.play = true;
	else
	{
		if (!global::sounds.has(soundName))
			return;

		Sound* s = &global::sounds[soundName];
		auto result = _sounds.emplace(std::piecewise_construct,
			std::make_tuple(soundName),
			std::make_tuple()
		);

		result.first->second.sound = s;
		result.first->second.play = true;
	}
}








namespace global::musics
{
	std::map<String, MusicInfo> musics;

	bool readMusicInfo(MusicInfo& info, const Json& json)
	{
		if (!utils::has(json, "file"))
			return false;

		const Json& jfile = json["file"];
		if (!jfile.is_string())
			return false;

		info.file = jfile.get<String>();
		info.loop = utils::opt<bool>(json, "loop", false);

		if (info.loop)
		{
			info.start = utils::opt<UInt64>(json, "start", 0);
			info.end = utils::opt<UInt64>(json, "end", 0);
		}

		return true;
	}

	void insertMusicToGlobal(const String& name, const Json& json)
	{
		MusicInfo info;
		if (!readMusicInfo(info, json))
			return;

		musics.insert({ name, std::move(info) });
	}

	void loadMusic(Music& music, const MusicInfo& info, const resource::Folder* folder = nullptr)
	{
		const resource::Folder& base = folder ? *folder : resource::music;

		if (!music.openFromFile(base.pathOf(info.file).string()))
			throw std::exception{ "An error has been ocurred during music load." };
	}


	void prepareCache()
	{
		Json json;
		resource::music.readJson("config.json", json);

		for (auto it = json.begin(); it != json.end(); ++it)
		{
			if (!it.value().is_object())
				continue;

			insertMusicToGlobal(it.key(), it.value());
		}
	}

	Music& load(Music& music, const String& name)
	{
		auto it = musics.find(name);
		if (it == musics.end())
			return music;

		const MusicInfo& info = it->second;

		loadMusic(music, info);
		return music;
	}
}

namespace musics
{
	MusicInfo readInfo(const Json& json)
	{
		MusicInfo info;
		global::musics::readMusicInfo(info, json);
		return info;
	}

	Music& load(Music& music, const MusicInfo& info, const resource::Folder& folder)
	{
		global::musics::loadMusic(music, info, &folder);
		return music;
	}
}
