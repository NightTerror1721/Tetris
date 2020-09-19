#include "scenario.h"

#define _cell(_Row, _Column) _cells[(_Row) * columns + (_Column)]


Cell::Cell(CellColor color) :
	RectangleShape{},
	_color{},
	_row{ 0 },
	_column{ 0 }
{
	sf::RectangleShape::setPosition(0, 0);
	sf::RectangleShape::setSize({ static_cast<float>(width), static_cast<float>(height) });

	_color = color;
	sf::RectangleShape::setTexture(global::theme.cellColorTexture(color), true);
}

void Cell::changeColor(CellColor color)
{
	if (_color != color)
	{
		_color = color;
		sf::RectangleShape::setTexture(global::theme.cellColorTexture(color), true);
	}
}

void Cell::setPosition(int row, int column)
{
	_row = utils::clamp(row, 0, Field::rows - 1);
	_column = utils::clamp(column, 0, Field::columns - 1);

	sf::RectangleShape::setPosition(
		static_cast<float>(_column * Cell::width),
		static_cast<float>(((Field::visible_rows - 1) * Cell::height) - (_row * Cell::height))
	);
}


Field::Field() :
	Frame{
		{ Field::width, Field::height },
		{ static_cast<float>(Field::columns * Cell::width), static_cast<float>(Field::visible_rows * Cell::height) }
	},
	_cells{}
{

	for (Offset idx = 0; idx < Field::cellCount; ++idx)
		_cells[idx].setPosition(idx / Field::columns, idx % Field::columns);
}

void Field::render(sf::RenderTarget& canvas, const Tetromino* tetromino)
{
	clearCanvas();

	sf::RenderTarget& frameCanvas = Frame::canvas();

	for (int idx = 0; idx < Field::visibleCellCount; idx++)
		_cells[idx].render(frameCanvas);

	if (tetromino)
		tetromino->render(frameCanvas);

	renderCanvas(canvas);
}

void Field::update(const sf::Time& delta)
{

}

bool Field::collide(const Tetromino& tetromino)
{
	auto idxs = tetromino.cellsIndex();
	for (int idx : idxs)
		if (idx >= 0 && idx < Field::cellCount && _cells[idx])
			return true;
	return false;
}

bool Field::isTopOut(const Tetromino& tetromino)
{
	auto idxs = tetromino.cellsIndex();
	for (int idx : idxs)
		if (idx >= Field::cellCount)
			return true;
	return false;
}

bool Field::isBottomOut(const Tetromino& tetromino)
{
	auto idxs = tetromino.cellsIndex();
	for (int idx : idxs)
		if (idx < 0)
			return true;
	return false;
}

void Field::insert(const Tetromino& tetromino)
{
	auto idxs = tetromino.cellsIndex();
	auto color = tetromino.color();

	for (int idx : idxs)
		_cells[idx].changeColor(color);
}






void Tetromino::render(sf::RenderTarget& canvas) const
{
	for (int i = 0; i < Tetromino::cellCount; i++)
		if (_cells[i])
			_cells[i].render(canvas);
}

void Tetromino::build(Type type)
{
	#define mat(_Row, _Col) matrix[(_Row) * Tetromino::columns + (_Col)]
	CellColor matrix[Tetromino::cellCount]{
		CellColor::Empty, CellColor::Empty, CellColor::Empty, CellColor::Empty,
		CellColor::Empty, CellColor::Empty, CellColor::Empty, CellColor::Empty,
		CellColor::Empty, CellColor::Empty, CellColor::Empty, CellColor::Empty,
		CellColor::Empty, CellColor::Empty, CellColor::Empty, CellColor::Empty
	};

	_validIdx = false;

	switch (type)
	{
		default:
		case Type::I:
			mat(1, 0) = CellColor::Cyan;
			mat(1, 1) = CellColor::Cyan;
			mat(1, 2) = CellColor::Cyan;
			mat(1, 3) = CellColor::Cyan;
			_type = Type::I;
			break;

		case Type::O:
			mat(1, 1) = CellColor::Yellow;
			mat(1, 2) = CellColor::Yellow;
			mat(2, 1) = CellColor::Yellow;
			mat(2, 2) = CellColor::Yellow;
			_type = type;
			break;

		case Type::T:
			mat(1, 0) = CellColor::Purple;
			mat(1, 1) = CellColor::Purple;
			mat(1, 2) = CellColor::Purple;
			mat(2, 1) = CellColor::Purple;
			_type = type;
			break;

		case Type::J:
			mat(1, 0) = CellColor::Blue;
			mat(1, 1) = CellColor::Blue;
			mat(1, 2) = CellColor::Blue;
			mat(2, 0) = CellColor::Blue;
			_type = type;
			break;

		case Type::L:
			mat(1, 0) = CellColor::Orange;
			mat(1, 1) = CellColor::Orange;
			mat(1, 2) = CellColor::Orange;
			mat(2, 2) = CellColor::Orange;
			_type = type;
			break;

		case Type::S:
			mat(0, 0) = CellColor::Green;
			mat(0, 1) = CellColor::Green;
			mat(1, 1) = CellColor::Green;
			mat(1, 2) = CellColor::Green;
			_type = type;
			break;

		case Type::Z:
			mat(0, 1) = CellColor::Red;
			mat(0, 2) = CellColor::Red;
			mat(1, 0) = CellColor::Red;
			mat(1, 1) = CellColor::Red;
			_type = type;
			break;
	}

	for (int i = 0; i < cellCount; i++)
		_cells[i].changeColor(matrix[i]);

	#undef mat
}

void Tetromino::setPosition(int row, int column)
{
	_validIdx = false;

	_row = row;
	_column = column;

	for (int i = 0; i < Tetromino::cellCount; i++)
		_cells[i].setPosition(row + (i / Tetromino::rows), column + (i % Tetromino::columns));
}

void Tetromino::move(int rowDelta, int columnDelta) { setPosition(_row + rowDelta, _column + columnDelta); }

void Tetromino::leftRotate()
{
	--_rotation;

	if (_type == Type::O)
		return;

	_validIdx = false;

	if (_type == Type::I)
	{
		CellColor matrix[Tetromino::cellCount]{
			_cell(3, 0).color(), _cell(2, 0).color(), _cell(1, 0).color(), _cell(0, 0).color(),
			_cell(3, 1).color(), _cell(2, 1).color(), _cell(1, 1).color(), _cell(0, 1).color(),
			_cell(3, 2).color(), _cell(2, 2).color(), _cell(1, 2).color(), _cell(0, 2).color(),
			_cell(3, 3).color(), _cell(2, 3).color(), _cell(1, 3).color(), _cell(0, 3).color()
		};
		for (int i = 0; i < cellCount; i++)
			_cells[i].changeColor(matrix[i]);
	}
	else
	{
		CellColor matrix[Tetromino::cellCount]{
			_cell(2, 0).color(), _cell(1, 0).color(), _cell(0, 0).color(), CellColor::Empty,
			_cell(2, 1).color(), _cell(1, 1).color(), _cell(0, 1).color(), CellColor::Empty,
			_cell(2, 2).color(), _cell(1, 2).color(), _cell(0, 2).color(), CellColor::Empty,
			CellColor::Empty, CellColor::Empty, CellColor::Empty, CellColor::Empty
		};
		for (int i = 0; i < cellCount; i++)
			_cells[i].changeColor(matrix[i]);
	}
}
void Tetromino::rightRotate()
{
	++_rotation;

	if (_type == Type::O)
		return;

	_validIdx = false;

	if (_type == Type::I)
	{
		CellColor matrix[Tetromino::cellCount]{
			_cell(0, 3).color(), _cell(1, 3).color(), _cell(2, 3).color(), _cell(3, 3).color(),
			_cell(0, 2).color(), _cell(1, 2).color(), _cell(2, 2).color(), _cell(3, 2).color(),
			_cell(0, 1).color(), _cell(1, 1).color(), _cell(2, 1).color(), _cell(3, 1).color(),
			_cell(0, 0).color(), _cell(1, 0).color(), _cell(2, 0).color(), _cell(3, 0).color()
		};
		for (int i = 0; i < cellCount; i++)
			_cells[i].changeColor(matrix[i]);
	}
	else
	{
		CellColor matrix[Tetromino::cellCount]{
			_cell(0, 2).color(), _cell(1, 2).color(), _cell(2, 2).color(), CellColor::Empty,
			_cell(0, 1).color(), _cell(1, 1).color(), _cell(2, 1).color(), CellColor::Empty,
			_cell(0, 0).color(), _cell(1, 0).color(), _cell(2, 0).color(), CellColor::Empty,
			CellColor::Empty, CellColor::Empty, CellColor::Empty, CellColor::Empty
		};
		for (int i = 0; i < cellCount; i++)
			_cells[i].changeColor(matrix[i]);
	}
}

std::array<int, 4> Tetromino::cellsIndex() const
{
	if (!_validIdx)
	{
		for (int idx = 0, count = 0; idx < Tetromino::cellCount && count < 4; idx++)
			if (_cells[idx])
				_idx[count++] = (_row + (idx / Tetromino::columns)) * Field::columns + (_column + (idx % Tetromino::columns));
		_validIdx = true;
	}
	return { _idx[0], _idx[1], _idx[2], _idx[3] };
}

CellColor Tetromino::color() const
{
	switch (_type)
	{
		case Type::I: return CellColor::Cyan;
		case Type::O: return CellColor::Yellow;
		case Type::T: return CellColor::Purple;
		case Type::J: return CellColor::Blue;
		case Type::L: return CellColor::Orange;
		case Type::S: return CellColor::Green;
		case Type::Z: return CellColor::Red;
		default: return CellColor::Gray;
	}
}





void GravityClock::setGravityLevel(unsigned int level)
{
	level = std::max(1U, level);

	/* Tetris Worlds gravity formula */
	double time = std::pow(0.8 - (static_cast<double>(level - 1) * 0.0007), static_cast<double>(level - 1));
	Int64 microTime = static_cast<Int64>(time * 1000000);

	_waiting = sf::microseconds(std::max(microTime, min_waiting_time));
	reset();
}

void GravityClock::updateWaiting(const sf::Time& delta)
{
	_remaining -= delta;
}

void GravityClock::updateFreezing(const sf::Time& delta)
{
	if (_freezing > sf::Time::Zero)
		_freezing -= delta;
	else _freezing = sf::Time::Zero;
}

void GravityClock::registerDrop()
{
	_remaining += _waiting;
	if (_remaining > _waiting)
		_remaining = _waiting;
}




#pragma warning(push)
#pragma warning(disable : 6385)
Tetromino::Type TetrominoBag::take()
{
	if (_remaining < 1)
		_generate();
	return _bag[--_remaining];
}
#pragma warning(pop)

void TetrominoBag::_generate()
{
	std::array<Tetromino::Type, Tetromino::type_count> types{
		Tetromino::Type::I,
		Tetromino::Type::O,
		Tetromino::Type::T,
		Tetromino::Type::J,
		Tetromino::Type::L,
		Tetromino::Type::S,
		Tetromino::Type::Z
	};

	auto seed = utils::system_time();
	std::shuffle(types.begin(), types.end(), std::default_random_engine{ static_cast<unsigned int>(seed) });

	std::memcpy(_bag, types.data(), sizeof(_bag));
	_remaining = sizeof(_bag) / sizeof(_bag[0]);
}





TetrominoManager::TetrominoManager() :
	Frame{
		{ static_cast<unsigned int>(Tetromino::width), static_cast<unsigned int>(Tetromino::height * TetrominoManager::next_count) },
		{ static_cast<float>(TetrominoManager::width), static_cast<float>(TetrominoManager::height) }
	},
	_bag{},
	_next{}
{
	for (int i = 0; i < TetrominoManager::next_count; i++)
		generate();
}

Tetromino TetrominoManager::next()
{
	generate();

	Tetromino next = _next.front();
	_next.pop_front();

	return next;
}

void TetrominoManager::render(sf::RenderTarget& canvas)
{
	clearCanvas();

	for (auto& t : _next)
		t.render(Frame::canvas());

	renderCanvas(canvas);
}

void TetrominoManager::generate()
{
	for (auto& t : _next)
		t.move(Tetromino::rows, 0);

	_next.emplace_back();

	Tetromino& t = _next.back();
	t.setPosition(0, 0);
	t.build(_bag.take());
}








Scenario::Scenario() :
	Frame{
		{ Scenario::width, Scenario::height },
		{ static_cast<float>(Scenario::width), static_cast<float>(Scenario::height) }
	},
	_field{},
	_nextTetrominos{},
	_currentTetromino{},
	_currentTetrominoState{ TetrominoState::None },
	_gravity{},
	_state{ State::Stopped }
{
	_field.setPosition({
		static_cast<float>((Scenario::width / 2) - (Field::width / 2)),
		static_cast<float>((Scenario::height / 2) - (Field::height / 2))
	});

	_nextTetrominos.setPosition({
		static_cast<float>(Scenario::width - TetrominoManager::width - Scenario::next_border),
		static_cast<float>(_field.getPosition().y)
	});

	_gravity.setGravityLevel(8);
}

void Scenario::render(sf::RenderTarget& canvas)
{
	clearCanvas();
	auto& fcanvas = Frame::canvas();

	_field.render(fcanvas, _currentTetrominoState == TetrominoState::None ? nullptr : &_currentTetromino);
	_nextTetrominos.render(fcanvas);

	renderCanvas(canvas);
}

void Scenario::update(const sf::Time& delta)
{
	_updateCurrentTetromino(delta);
}

void Scenario::dispatchEvent(const sf::Event& event)
{

}

void Scenario::_spawnTetromino()
{
	_currentTetromino = _nextTetrominos.next();

	/* Try to situate into origin */
	_currentTetromino.setPosition(Field::rows - 4, (Field::columns / 2) - (Tetromino::columns / 2));
	if (_field.collide(_currentTetromino))
	{
		/* Try to situate into origin */
		_currentTetromino.move(1, 0);
		if (_field.collide(_currentTetromino))
		{
			/* Try to situate into origin */
			_currentTetromino.move(1, 0);
			if (_field.collide(_currentTetromino))
			{
				_currentTetrominoState = TetrominoState::None;
				_state = State::GameOver;
				return;
			}
		}
	}

	_gravity.reset();
	_currentTetrominoState = TetrominoState::Dropping;
}

void Scenario::_updateCurrentTetromino(const sf::Time& delta)
{
	switch (_currentTetrominoState)
	{
		case TetrominoState::Dropping:
			_gravity.updateWaiting(delta);
			if (!_gravity.isWaiting())
			{
				do {
					_gravity.registerDrop();
					_dropCurrentTetromino();
				} while (_currentTetrominoState == TetrominoState::Dropping && !_gravity.isWaiting());
			}
			break;

		case TetrominoState::Frozen:
			_gravity.updateFreezing(delta);
			if (!_gravity.isFrozen())
			{
				_gravity.resetFreezing();
				_currentTetrominoState = TetrominoState::Inserting;
			}
			break;

		case TetrominoState::Inserting:
			_gravity.updateFreezing(delta);
			if (!_gravity.isFrozen())
			{
				_field.insert(_currentTetromino);
				_currentTetrominoState = TetrominoState::None;
			}
			break;

		case TetrominoState::None:
			_spawnTetromino();
			break;
	}
}

void Scenario::_dropCurrentTetromino()
{
	Tetromino tryer = _currentTetromino;

	tryer.moveDown();
	if (_field.collide(tryer) || _field.isBottomOut(tryer))
	{
		_currentTetrominoState = TetrominoState::Frozen;
		_gravity.resetFreezing();
		return;
	}

	_currentTetromino.moveDown();
}
