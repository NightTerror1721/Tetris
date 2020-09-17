#include "game_controller.h"

GameController global::game{ "!Kram Tetris" };



GameController::GameController(const std::string& name) :
	GameObjectContainer{},
	_close{ true },
	_window{},
	_deltaClock{},
	_phAccumulator{},
	_phUps{},
	_name{ name },
	_vmode{ 640, 480 },
	_wstyle{ WindowStyle::Default }
{}
GameController::~GameController() {}

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

void GameController::videoMode(sf::VideoMode mode, bool apply)
{
	_vmode = mode;
	if (apply)
		resetWindow();
}
void GameController::style(WindowStyle style, bool apply)
{
	_wstyle = style;
	if (apply)
		resetWindow();
}
void GameController::resetWindow()
{
	if (_close)
		return;

	if (_window.isOpen())
		close();
	_window.create(_vmode, _name.c_str(), static_cast<UInt32>(_wstyle));
	_window.setVerticalSyncEnabled(true);
	_window.setActive(true);
}

bool GameController::fullscreen() const { return (_wstyle & WindowStyle::Fullscreen) == WindowStyle::None; }

void GameController::loop()
{
	while (!_close)
	{
		processEvents();

		while (_phAccumulator > _phUps)
		{
			sf::Time delta = _deltaClock.restart();
			_phAccumulator -= _phUps;
			update(delta);
		}

		render();
	}
}

void GameController::init()
{
	//Properties::load();
	//pylib::loadResourceCaches();
	resetWindow();
}
void GameController::update(const sf::Time& delta)
{
	if (!_close)
		for (GameObject& obj : *this)
			obj.update(delta);
}
void GameController::render()
{
	if (!_close)
	{
		_window.clear();
		for (GameObject& obj : *this)
			obj.render(_window);
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
			for (GameObject& obj : *this)
				obj.dispatchEvent(event);
		}
	}
}

void GameController::onCreate(GameObject& obj)
{
}
void GameController::onDestroy(GameObject& obj)
{
}
