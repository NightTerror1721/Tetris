#include "theme.h"


Theme global::theme;

Music& Theme::loadScenarioMusic(Music& music)
{
	if (_scenarioMusic.custom)
	{
		musics::load(music, _scenarioMusic.info, _getFolder());
	}
	else
	{
		if(!_scenarioMusic.name.empty())
			global::musics::load(music, _scenarioMusic.name);
	}

	return music;
}



Path Theme::_getPath(const String& filename)
{
	return Path{ _folderName } / filename;
}

resource::Folder Theme::_getFolder() const
{
	return resource::themes.folder(_folderName);
}

Texture& Theme::_loadTexture(const TextureInfo& textureInfo, const String& name)
{
	TextureInfo info = textureInfo;
	info.name = name;
	info.file = _getPath(info.file).string();
	return _textures.load(info);
}

void Theme::_load(const String& name, const Json& json)
{
	_folderName = name;

	if (utils::has(json, "name"))
		_name = utils::opt<String>(json, "name", name);
	else _name = name;

	_loadCellColors(json, false);
	_loadCellColors(json, true);

	_loadScenarioMusic(json);
}

Json Theme::_loadJson(const String& name) const
{
	resource::Folder themeFolder{ resource::themes, name };
	Json json;

	themeFolder.readJson("config.json", json);
	return json;
}

void Theme::_loadCellColors(const Json& json, bool ghost)
{
	const char* const obj_name = ghost ? "ghost_colors" : "cell_colors";

	if (!utils::has(json, obj_name))
		return;

	const Json& base = json[obj_name];
	if (!base.is_object())
		return;

	_loadCellColors(base, ghost, {
		{ "red", 0 },
		{ "orange", 1 },
		{ "yellow", 2 },
		{ "green", 3 },
		{ "cyan", 4 },
		{ "blue", 5 },
		{ "purple", 6 },
		{ "gray", 7 }
	});
}

void Theme::_loadCellColors(const Json& json, bool ghost, const std::vector<std::pair<const char*, Offset>>& ids)
{
	sf::Texture** colors = ghost ? _ghostColors : _cellColors;
	String name_prefix = ghost ? "cell.ghost." : "cell.";
	for (const auto& id : ids)
	{
		if (!utils::has(json, id.first))
			continue;

		colors[id.second] = &_loadTexture(TextureInfo::read(json[id.first]), name_prefix + id.first);
	}
}

void Theme::_loadScenarioMusic(const Json& json)
{
	if (!utils::has(json, "music"))
		return;

	const Json& base = json["music"];
	if (!base.is_object())
		return;

	utils::destroy(_scenarioMusic);
	utils::construct(_scenarioMusic);

	_scenarioMusic.custom = utils::opt<bool>(base, "custom", false);
	if (_scenarioMusic.custom)
	{
		_scenarioMusic.info = musics::readInfo(base);
	}
	else
	{
		_scenarioMusic.name = utils::opt<String>(base, "name", "");
	}
}
