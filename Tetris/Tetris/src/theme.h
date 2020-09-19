#pragma once

#include "game_basics.h"
#include "sprites.h"


enum class CellColor
{
	Empty,
	Red,
	Orange,
	Yellow,
	Green,
	Cyan,
	Blue,
	Purple,
	Gray
};

namespace utils
{
	constexpr Size cell_color_count = static_cast<Size>(CellColor::Gray) + 1;

	constexpr Offset cellcolor_id(CellColor color) { return static_cast<Offset>(color); }

	constexpr CellColor id_to_cellcolor(Offset id) { return static_cast<CellColor>(utils::clamp<Offset, Offset, Offset>(id, 0, cell_color_count - 1)); }

	constexpr CellColor id_to_noempty_cellcolor(Offset id) { return static_cast<CellColor>(utils::clamp<Offset, Offset, Offset>(id, 1, cell_color_count - 1)); }
}

class Theme
{
private:
	TextureManager _textures = global::textures.createChild(resource::themes);

	String _folderName;
	String _name;

	Texture* _cellColors[utils::cell_color_count - 1] = {};
	Texture* _ghostColors[utils::cell_color_count - 1] = {};

public:
	Theme() = default;
	Theme(const Theme&) = delete;
	Theme(Theme&&) noexcept = delete;
	~Theme() = default;

	Theme& operator= (const Theme&) = delete;
	Theme& operator= (Theme&&) noexcept = delete;

	inline const Texture* cellColorTexture(CellColor cell) { return cell == CellColor::Empty ? nullptr : _cellColors[utils::cellcolor_id(cell) - 1]; }
	inline const Texture* ghostColorTexture(CellColor cell) { return cell == CellColor::Empty ? nullptr : _ghostColors[utils::cellcolor_id(cell) - 1]; }

private:
	Path _getPath(const String& filename);
	Texture& _loadTexture(const TextureInfo& textureInfo, const String& name);

	Json _loadJson(const String& name) const;
	void _load(const String& name, const Json& json);

	void _loadCellColors(const Json& json, bool ghost);
	void _loadCellColors(const Json& json, bool ghost, const std::vector<std::pair<const char*, Offset>>& ids);

public:
	inline void load(const String& name) { _load(name, _loadJson(name)); }
};

namespace global
{
	extern Theme theme;
}
