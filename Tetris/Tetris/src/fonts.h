#pragma once

#include "game_basics.h"

using sf::Font;

class FontManager : public SingleTypeManager<Font>
{
public:
	Font& load(const String& filename, const String& name);

	void loadAll();
};

namespace global
{
	extern FontManager fonts;
}
