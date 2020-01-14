#pragma once

#include <SFML/Graphics.hpp>

#include "game_objects.h"
#include "types.h"

namespace WindowStyle = sf::Style;

class GameController : public GameObjectRepository<>
{
private:
	bool _close;
	sf::RenderWindow _window;

	std::string _name;
	sf::VideoMode _vmode;
	uint32 _wstyle;
	bool _showFps;

	sf::Clock _elapsedClock;
	sf::Time _elapsedTime;

public:
	GameController(const std::string& name);
	~GameController();

	void setVideoMode(const sf::VideoMode& mode, bool apply = true);
	void setStyle(const uint32 style, bool apply = true);
	void showFps(bool enabled);

	const sf::VideoMode& getVideoMode() const;
	uint32 getStyle() const;
	bool isFullscreen() const;

	void start();

	void close();

	void resetWindow();

private:
	void loop();

	void init();
	void update();
	void render();
	void processEvents();


private:
	class FPSManager
	{
	private:
		sf::Font _font;

		uint32 _fps;
		uint32 _framesComputed;
		sf::Time _remainingToUpdateFps;

		sf::Text _fpsText;

	public:
		FPSManager();
		~FPSManager();

		void update(const delta_t& delta);
		void render(sf::RenderTarget* canvas);

	private:
		static const sf::Time SecondUnit;
	};
	FPSManager _fpsManager;
};
