#include "game_loop.h"

#include "resources.h"

GameController::GameController(const std::string& name) :
	GameObjectRepository{ true },
	_close{ true },
	_window{},
	_name{ name },
	_vmode{ 640, 480 },
	_wstyle{ WindowStyle::Default },
	_showFps{ false },
	_elapsedClock{},
	_elapsedTime{},
	_fpsManager{}
{}
GameController::~GameController()
{
	close();
}

void GameController::setVideoMode(const sf::VideoMode& mode, bool apply)
{
	_vmode = mode;
	if (apply)
		resetWindow();
}
void GameController::setStyle(const uint32 style, bool apply)
{
	_wstyle = style;
	if (apply)
		resetWindow();
}
void GameController::showFps(bool enabled)
{
	_showFps = enabled;
}

const sf::VideoMode& GameController::getVideoMode() const { return _vmode; }
uint32 GameController::getStyle() const { return _wstyle; }
bool GameController::isFullscreen() const { return _wstyle & WindowStyle::Fullscreen; }

void GameController::start()
{
	if (_close)
	{
		_close = false;
		init();
		loop();
	}
}

void GameController::close()
{
	if (!_close)
	{
		_close = true;
		_window.close();
	}
}

void GameController::resetWindow()
{
	if (_close)
		return;

	if (_window.isOpen())
		_window.close();
	_window.create(_vmode, _name.c_str(), _wstyle);
	//_window.setFramerateLimit(144);
	_window.setVerticalSyncEnabled(true);
	_window.setActive(true);
}


void GameController::loop()
{
	while (!_close)
	{
		_elapsedTime = _elapsedClock.restart();
		processEvents();
		update();
		render();
	}
}

void GameController::init()
{
	resetWindow();
}
void GameController::update()
{
	if (!_close)
	{
		/* Compute FPS */
		_fpsManager.update(_elapsedTime);

		/* Update GameObjects */

	}
}
void GameController::render()
{
	if (!_close)
	{
		_window.clear();

		if (_showFps)
			_fpsManager.render(&_window);
		_window.display();
	}
}
void GameController::processEvents()
{
	if (!_close)
	{
		sf::Event event;
		while (_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				_close = true;
				return;
			}
		}
	}
}

/* FPS Render */
GameController::FPSManager::FPSManager() :
	_font{ Resource::loadFont("arial") },
	_fps{},
	_framesComputed{},
	_remainingToUpdateFps{},
	_fpsText{}
{
	_fpsText.setFont(_font);
	_fpsText.setCharacterSize(24U);
	_fpsText.setString("0 fps");
	_fpsText.setPosition({ 10, 20 });
}
GameController::FPSManager::~FPSManager() {}

void GameController::FPSManager::update(const delta_t& delta)
{
	_remainingToUpdateFps += delta;
	_framesComputed += 1;
	if (_remainingToUpdateFps >= SecondUnit)
	{
		double temp = SecondUnit.asSeconds() / _framesComputed;
		_fps = static_cast<uint32>(1.0 / temp);
		_remainingToUpdateFps = sf::Time::Zero;
		_framesComputed = 0;
		_fpsText.setString(std::to_string(_fps) + " fps");
	}
}
void GameController::FPSManager::render(sf::RenderTarget* canvas)
{
	canvas->draw(_fpsText);
}

const sf::Time GameController::FPSManager::SecondUnit{ sf::seconds(1) };
