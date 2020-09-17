#include "theme.h"


Theme global::theme;

Path Theme::_getPath(const String& filename)
{
	return Path{ _folderName } / filename;
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

	_loadCellColors(json);
}

Json Theme::_loadJson(const String& name) const
{
	resource::Folder themeFolder{ resource::themes, name };
	Json json;

	themeFolder.readJson("config.json", json);
	return json;
}

void Theme::_loadCellColors(const Json& json)
{
	if (!utils::has(json, "cell_colors"))
		return;

	const Json& base = json["cell_colors"];
	if (!base.is_object())
		return;

	_loadCellColors(base, {
		{ "empty", 0 },
		{ "red", 1 },
		{ "orange", 2 },
		{ "yellow", 3 },
		{ "green", 4 },
		{ "cyan", 5 },
		{ "blue", 6 },
		{ "purple", 7 },
		{ "gray", 8 }
	});
}

void Theme::_loadCellColors(const Json& json, const std::vector<std::pair<const char*, Offset>>& ids)
{
	for (const auto& id : ids)
	{
		if (!utils::has(json, id.first))
			continue;

		_cellColors[id.second] = &_loadTexture(TextureInfo::read(json[id.first]), id.first);
	}
}
