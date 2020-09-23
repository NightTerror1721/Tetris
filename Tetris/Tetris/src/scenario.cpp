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

void Cell::ghostify()
{
	if(_color != CellColor::Empty)
		sf::RectangleShape::setTexture(global::theme.ghostColorTexture(_color), true);
}

void Cell::setPosition(int row, int column)
{
	//_row = utils::clamp(row, 0, Field::rows - 1);
	//_column = utils::clamp(column, 0, Field::columns - 1);

	_row = row;
	_column = column;

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

void Field::render(sf::RenderTarget& canvas, const Tetromino* tetromino, const Tetromino* ghost)
{
	clearCanvas();

	sf::RenderTarget& frameCanvas = Frame::canvas();

	for (int idx = 0; idx < Field::visibleCellCount; idx++)
		_cells[idx].render(frameCanvas);

	if (tetromino)
	{
		tetromino->render(frameCanvas);
		if (ghost)
			ghost->render(frameCanvas);
	}

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

bool Field::isLeftOut(const Tetromino& tetromino)
{
	auto cells = tetromino.cellsAsVector();
	for (const auto& cell : cells)
		if (cell.x < 0)
			return true;
	return false;
}

bool Field::isRightOut(const Tetromino& tetromino)
{
	auto cells = tetromino.cellsAsVector();
	for (const auto& cell : cells)
		if (cell.x >= Field::columns)
			return true;
	return false;
}

bool Field::isInside(const Tetromino& tetromino)
{
	auto cells = tetromino.cellsAsVector();
	for (const auto& cell : cells)
		if (cell.x < 0 || cell.x >= Field::columns ||
			cell.y < 0 || cell.y >= Field::rows)
			return false;
	return true;
}

void Field::insert(const Tetromino& tetromino)
{
	auto idxs = tetromino.cellsIndex();
	auto color = tetromino.color();

	for (int idx : idxs)
		_cells[idx].changeColor(color);
}

bool Field::eraseIfComplete(int row)
{
	row = utils::clamp(row, 0, Field::rows);
	for (int column = 0; column < Field::columns; column++)
		if (_cells[row * Field::columns + column].empty())
			return false;

	for (int column = 0; column < Field::columns; column++)
		_cells[row * Field::columns + column].changeColor(CellColor::Empty);
	return true;
}

void Field::dropRows(int bottomRow)
{
	bottomRow = utils::clamp(bottomRow, 0, Field::rows);

	/* Check if bottomRow is empty. If not, return */
	for (int column = 0; column < Field::columns; column++)
		if (!_cells[bottomRow * Field::columns + column].empty())
			return;

	for (int row = bottomRow + 1; row < Field::rows; row++)
	{
		bool start = false;
		for (int column = 0; column < Field::columns; column++)
		{
			int idx = row * Field::columns + column;
			if (!_cells[idx].empty())
			{
				start = true;
				_cells[bottomRow * Field::columns + column].changeColor(_cells[idx].color());
				_cells[idx].changeColor(CellColor::Empty);
			}
		}
		if (start)
			bottomRow++;
	}
}






void Tetromino::render(sf::RenderTarget& canvas) const
{
	for (int i = 0; i < Tetromino::cellCount; i++)
		if (_cells[i])
			_cells[i].render(canvas);
}

void Tetromino::build(Type type) { build(TetrominoView{ type }); }

void Tetromino::build(const TetrominoView& view)
{
	_validIdx = false;
	_validVecs = false;

	_type = view.type;

	for (int i = 0; i < cellCount; i++)
		_cells[i].changeColor(view.cells[i]);
}

void Tetromino::ghostify()
{
	for (int i = 0; i < Tetromino::cellCount; i++)
		_cells[i].ghostify();
}

void Tetromino::setPosition(int row, int column)
{
	_validIdx = false;
	_validVecs = false;

	_row = row;
	_column = column;

	for (int i = 0; i < Tetromino::cellCount; i++)
		_cells[i].setPosition(row + (i / Tetromino::rows), column + (i % Tetromino::columns));
}

void Tetromino::move(int rowDelta, int columnDelta) { setPosition(_row + rowDelta, _column + columnDelta); }

void Tetromino::moveToOrigin() { setPosition(Field::rows - 1, 0); }

void Tetromino::leftRotate()
{
	--_rotation;

	if (_type == Type::O)
		return;

	_validIdx = false;
	_validVecs = false;

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
	_validVecs = false;

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

void Tetromino::kick(RotationState prevState, unsigned int tryId)
{
	auto prevFactors = _kickFactors(tryId, _type, prevState) - _kickFactors(tryId, _type, _rotation);
	move(prevFactors.y, prevFactors.x);
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

std::array<Vec2i, 4> Tetromino::cellsAsVector() const
{
	if (!_validVecs)
	{
		for (int idx = 0, count = 0; idx < Tetromino::cellCount && count < 4; idx++)
			if (_cells[idx])
				_vecs[count++] = { _cells[idx].column(), _cells[idx].row() };
	}
	return { _vecs[0], _vecs[1], _vecs[2], _vecs[3] };
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

Vec2i Tetromino::_kickFactors(unsigned int tryId, Type type, RotationState rstate)
{
	tryId = utils::clamp(tryId, 0, max_rotation_try);
	switch (tryId)
	{
		case 0: return {};
		case 1: switch (type) {
			case Type::O: return {};
			case Type::I: switch (rstate.state) {
				case RotationState::Origin: return { -1, 0 };
				case RotationState::Right: return { 1, 0 };
				case RotationState::Inverse: return { 2, 0 };
				case RotationState::Left: return { 0, 0 };
			} break;
			default: switch (rstate.state) {
				case RotationState::Origin: return { 0, 0 };
				case RotationState::Right: return { 1, 0 };
				case RotationState::Inverse: return { 0, 0 };
				case RotationState::Left: return { -1, 0 };
			} break;
		} break;
		case 2: switch (type) {
			case Type::O: return {};
			case Type::I: switch (rstate.state) {
				case RotationState::Origin: return { 2, 0 };
				case RotationState::Right: return { 1, 0 };
				case RotationState::Inverse: return { -1, 0 };
				case RotationState::Left: return { 0, 0 };
			} break;
			default: switch (rstate.state) {
				case RotationState::Origin: return { 0, 0 };
				case RotationState::Right: return { 1, -1 };
				case RotationState::Inverse: return { 0, 0 };
				case RotationState::Left: return { -1, -1 };
			} break;
		} break;
		case 3: switch (type) {
			case Type::O: return {};
			case Type::I: switch (rstate.state) {
				case RotationState::Origin: return { -1, 0 };
				case RotationState::Right: return { 1, 1 };
				case RotationState::Inverse: return { 2, -1 };
				case RotationState::Left: return { 0, -2 };
			} break;
			default: switch (rstate.state) {
				case RotationState::Origin: return { 0, 0 };
				case RotationState::Right: return { 0, 2 };
				case RotationState::Inverse: return { 0, 0 };
				case RotationState::Left: return { 0, 2 };
			} break;
		} break;
		case 4: switch (type) {
			case Type::O: return {};
			case Type::I: switch (rstate.state) {
				case RotationState::Origin: return { 2, 0 };
				case RotationState::Right: return { 1, -2 };
				case RotationState::Inverse: return { -1, -1 };
				case RotationState::Left: return { 0, 1 };
			} break;
			default: switch (rstate.state) {
				case RotationState::Origin: return { 0, 0 };
				case RotationState::Right: return { 1, 2 };
				case RotationState::Inverse: return { 0, 0 };
				case RotationState::Left: return { -1, 2 };
			} break;
		} break;
	}

	return {};
}






void TetrominoView::build(Tetromino::Type type_)
{
	using Type = Tetromino::Type;

	#define mat(_Row, _Col) this->cells[(_Row) * Tetromino::columns + (_Col)]
	std::memset(cells, 0, sizeof(cells));

	switch (type_)
	{
		default:
		case Type::I:
			mat(2, 0) = CellColor::Cyan;
			mat(2, 1) = CellColor::Cyan;
			mat(2, 2) = CellColor::Cyan;
			mat(2, 3) = CellColor::Cyan;
			type = Type::I;
			break;

		case Type::O:
			mat(1, 1) = CellColor::Yellow;
			mat(1, 2) = CellColor::Yellow;
			mat(2, 1) = CellColor::Yellow;
			mat(2, 2) = CellColor::Yellow;
			type = type_;
			break;

		case Type::T:
			mat(1, 0) = CellColor::Purple;
			mat(1, 1) = CellColor::Purple;
			mat(1, 2) = CellColor::Purple;
			mat(2, 1) = CellColor::Purple;
			type = type_;
			break;

		case Type::J:
			mat(1, 0) = CellColor::Blue;
			mat(1, 1) = CellColor::Blue;
			mat(1, 2) = CellColor::Blue;
			mat(2, 0) = CellColor::Blue;
			type = type_;
			break;

		case Type::L:
			mat(1, 0) = CellColor::Orange;
			mat(1, 1) = CellColor::Orange;
			mat(1, 2) = CellColor::Orange;
			mat(2, 2) = CellColor::Orange;
			type = type_;
			break;

		case Type::S:
			mat(1, 0) = CellColor::Green;
			mat(1, 1) = CellColor::Green;
			mat(2, 1) = CellColor::Green;
			mat(2, 2) = CellColor::Green;
			type = type_;
			break;

		case Type::Z:
			mat(1, 1) = CellColor::Red;
			mat(1, 2) = CellColor::Red;
			mat(2, 0) = CellColor::Red;
			mat(2, 1) = CellColor::Red;
			type = type_;
			break;
	}

	#undef mat
}

void TetrominoView::render(sf::RenderTarget& canvas, bool ghost, const Vec2f& position, const Vec2f& size)
{
	CellColor last = CellColor::Empty;
	Vec2f cell_size = { size.x / Tetromino::columns, size.y / Tetromino::rows };
	sf::RectangleShape shape{ cell_size };
	shape.setPosition(position);

	for(int row = 0; row < Tetromino::rows; row++)
		for (int column = 0; column < Tetromino::columns; column++)
		{
			shape.setPosition(position.x + (cell_size.x * column), position.y + (cell_size.y * (Tetromino::rows - row - 1)));

			CellColor color = cells[row * Tetromino::columns + column];
			if (color != last)
				shape.setTexture(color == CellColor::Empty ? nullptr : ghost ? global::theme.ghostColorTexture(color) : global::theme.cellColorTexture(color));

			if (color != CellColor::Empty)
				canvas.draw(shape);
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

void GravityClock::updateInserting(const sf::Time& delta)
{
	if (_inserting > sf::Time::Zero)
		_inserting -= delta;
	else _inserting = sf::Time::Zero;
}

void GravityClock::registerDrop()
{
	switch (_mode)
	{
		default:
		case Mode::Normal:
			_remaining += _waiting;
			if (_remaining > _waiting)
				_remaining = _waiting;
			break;

		case Mode::Soft:
			_remaining += sf::microseconds(GravityClock::soft_drop_time);
			if (_remaining > _waiting)
				_remaining = _waiting;
			break;

		case Mode::Hard:
			_remaining = sf::Time::Zero;
	}
}

void GravityClock::resetWaiting()
{
	switch (_mode)
	{
		default:
		case Mode::Normal:
			_remaining = _waiting;
			break;

		case Mode::Soft:
			_remaining = _waiting.asMicroseconds() < GravityClock::soft_drop_time
				? _waiting
				: sf::microseconds(GravityClock::soft_drop_time);
			break;

		case Mode::Hard:
			_remaining = sf::Time::Zero;
			break;
	}
}

void GravityClock::setMode(Mode mode)
{
	if (mode == _mode)
		return;

	_mode = mode;
	if (_remaining > sf::Time::Zero)
	{
		if (mode == Mode::Soft)
		{
			if (_remaining.asMicroseconds() > GravityClock::soft_drop_time)
				_remaining = sf::microseconds(GravityClock::soft_drop_time);
		}
		else if (mode == Mode::Hard)
			_remaining = sf::Time::Zero;
	}
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

	TetrominoView next_view = _next.front();
	_next.pop_front();

	Tetromino next;
	next.setPosition(0, 0);
	next.build(next_view);

	return next;
}

void TetrominoManager::render(sf::RenderTarget& canvas)
{
	clearCanvas();

	Vec2f pos;
	Vec2f size = { static_cast<float>(Tetromino::width), static_cast<float>(Tetromino::height) };

	for (auto& t : _next)
	{
		t.render(Frame::canvas(), false, pos, size);
		pos.y += Tetromino::height;
	}

	renderCanvas(canvas);
}

void TetrominoManager::generate()
{
	_next.emplace_back(_bag.take());
}







HoldManager::HoldManager() :
	Frame{
		{ static_cast<unsigned int>(Tetromino::width), static_cast<unsigned int>(Tetromino::height) },
		{ static_cast<float>(HoldManager::width), static_cast<float>(HoldManager::height) }
	},
	_tetromino{},
	_empty{ true },
	_lock{ false }
{}

void HoldManager::render(sf::RenderTarget& canvas)
{
	clearCanvas();

	if (!_empty)
		_tetromino.render(Frame::canvas(), false, {});

	renderCanvas(canvas);
}

void HoldManager::hold(Tetromino::Type type)
{
	if (_empty || !_lock)
	{
		_tetromino.build(type);
		_lock = !_empty;
		_empty = false;
	}
}








void ActionRepeatManager::update(const sf::Time& delta)
{
	if (_action == ScenarioAction::None)
		return;

	_delay -= delta;
	if (_delay < sf::Time::Zero)
	{
		_delay = sf::Time::Zero;
		_speed -= delta;
		if (_speed < sf::Time::Zero)
			_speed = sf::Time::Zero;
	}
}

void ActionRepeatManager::registerAction(ScenarioAction action)
{
	if (action == _action)
		return;

	_action = action;

	if (action != ScenarioAction::None)
	{
		_delay = sf::microseconds(ActionRepeatManager::auto_repeat_delay);
		_speed = sf::Time::Zero;
	}
}







Score::Score() :
	Frame{
		{ static_cast<unsigned int>(Score::width), static_cast<unsigned int>(Score::height) },
		{ static_cast<float>(Score::width), static_cast<float>(Score::height) }
	},
	_points{ 0 },
	_lines{ 0 },
	_level{ 1 },
	_tPoints{},
	_tLines{},
	_tLevel{},
	_remainingPoints{ 0 },
	_backToBack{ false },
	_font{ &global::fonts.get("arial") }
{
	_tPoints.setFont(*_font);
	_tLines.setFont(*_font);
	_tLevel.setFont(*_font);

	_updatePointsText();
	_updateLinesText();
	_updateLevelText();
}

void Score::_updatePointsText()
{
	_tPoints.setString(std::to_string(_points));
	_tPoints.setFillColor(sf::Color::White);
	utils::centrate_text(_tPoints, {}, { static_cast<float>(Score::display_width), static_cast<float>(Score::display_height) });
}

void Score::_updateLinesText()
{
	_tLines.setString(std::to_string(_lines));
	_tLines.setFillColor(sf::Color::White);
	utils::centrate_text(_tLines,
		{ 0, static_cast<float>(Score::display_height) },
		{ static_cast<float>(Score::display_width), static_cast<float>(Score::display_height) }
	);
}

void Score::_updateLevelText()
{
	_tLevel.setString(std::to_string(_level));
	_tLevel.setFillColor(sf::Color::White);
	utils::centrate_text(_tLevel,
		{ 0, static_cast<float>(Score::display_height * 2) },
		{ static_cast<float>(Score::display_width), static_cast<float>(Score::display_height) }
	);
}

void Score::render(sf::RenderTarget& canvas)
{
	clearCanvas();

	Frame::draw(_tPoints);
	Frame::draw(_tLines);
	Frame::draw(_tLevel);

	renderCanvas(canvas);
}

void Score::update(const sf::Time& delta)
{
	if (_remainingPoints > 0)
	{
		UInt64 speed = std::max(_remainingPoints * 5, 250ULL);
		UInt64 part = static_cast<UInt64>(static_cast<double>(delta.asSeconds()) * speed);
		if (part > _remainingPoints)
			part = _remainingPoints;

		_remainingPoints -= part;
		_points += part;

		_updatePointsText();
	}
}

void Score::addLines(UInt64 amount)
{
	_lines += amount;
	_updateLinesText();
}

void Score::increaseLevel()
{
	_level++;
	_updateLevelText();
}

void Score::_increasePoints(UInt64 amount)
{
	_remainingPoints += amount;
}

void Score::_increasePointsFromBase(int base, bool difficult)
{
	if (difficult && _backToBack)
		base = base * 3 / 2;

	_backToBack = difficult;

	UInt64 amount = static_cast<UInt64>(static_cast<unsigned int>(base) * _level);
	_remainingPoints += amount;
}







void TetrominoScenarioInfo::set(const Tetromino& tetromino, MoveType moveType)
{
	lastMove = moveType;
	type = tetromino.type();
	rotation = tetromino.rotationState();
}








Scenario::Scenario() :
	Frame{
		{ Scenario::width, Scenario::height },
		{ static_cast<float>(Scenario::width), static_cast<float>(Scenario::height) }
	},
	_field{},
	_hold{},
	_nextTetrominos{},
	_currentTetromino{},
	_currentTetrominoState{ TetrominoState::None },
	_bottomRowToErase{ -1 },
	_tetrominoInfo{},
	_horizontalMoveRepeat{},
	_gravity{},
	_score{},
	_state{ State::Stopped },
	_actionQueue{}
{
	_field.setPosition({
		static_cast<float>((Scenario::width / 2) - (Field::width / 2)),
		static_cast<float>(Score::height)
	});

	_nextTetrominos.setPosition({
		static_cast<float>(Scenario::width - TetrominoManager::width - Scenario::next_border),
		static_cast<float>(_field.getPosition().y)
	});

	_hold.setPosition({
		static_cast<float>(Scenario::hold_border),
		static_cast<float>(_field.getPosition().y)
	});

	_score.setPosition({
		static_cast<float>((Scenario::width / 2) - (Score::width / 2)),
		static_cast<float>(0)
		});

	_gravity.setGravityLevel(1);
}

void Scenario::render(sf::RenderTarget& canvas)
{
	clearCanvas();
	auto& fcanvas = Frame::canvas();

	_field.render(fcanvas,
		_currentTetrominoState == TetrominoState::None || _currentTetrominoState == TetrominoState::Inserting ? nullptr : &_currentTetromino,
		_currentTetrominoState != TetrominoState::Dropping ? nullptr : &_ghostTetromino
	);
	_nextTetrominos.render(fcanvas);
	_hold.render(fcanvas);
	_score.render(fcanvas);

	renderCanvas(canvas);
}

void Scenario::update(const sf::Time& delta)
{
	_updateActions(delta);
	_updateCurrentTetromino(delta);
	_score.update(delta);
}

void Scenario::dispatchEvent(const sf::Event& event)
{
	if (event.type == sf::Event::KeyPressed)
	{
		sf::Keyboard::Key key = event.key.code;
		switch (key)
		{
			case sf::Keyboard::Left:
				if (_horizontalMoveRepeat.action() != Action::MoveLeft)
				{
					pushAction(Action::MoveLeft);
					_horizontalMoveRepeat.registerAction(Action::MoveLeft);
				}
				break;

			case sf::Keyboard::Right:
				if (_horizontalMoveRepeat.action() != Action::MoveRight)
				{
					pushAction(Action::MoveRight);
					_horizontalMoveRepeat.registerAction(Action::MoveRight);
				}
				break;

			case sf::Keyboard::Q:
				pushAction(Action::RotateLeft);
				break;

			case sf::Keyboard::E:
				pushAction(Action::RotateRight);
				break;

			case sf::Keyboard::Up:
				pushAction(Action::HardDrop);
				break;

			case sf::Keyboard::Down:
				pushAction(Action::SoftDrop);
				break;

			case sf::Keyboard::Tab:
				pushAction(Action::Hold);
				break;
		}
	}
	else if (event.type == sf::Event::KeyReleased)
	{
		sf::Keyboard::Key key = event.key.code;
		switch (key)
		{
			case sf::Keyboard::Left:
				if (_horizontalMoveRepeat.action() == Action::MoveLeft)
					_horizontalMoveRepeat.releaseAction();
				break;

			case sf::Keyboard::Right:
				if (_horizontalMoveRepeat.action() == Action::MoveRight)
					_horizontalMoveRepeat.releaseAction();
				break;

			case sf::Keyboard::Up:
			case sf::Keyboard::Down:
				pushAction(Action::NormalDrop);
				break;
		}
	}
}

void Scenario::_updateActions(const sf::Time& delta)
{
	_horizontalMoveRepeat.update(delta);
	if (_horizontalMoveRepeat.isRepeating())
	{
		if (!_horizontalMoveRepeat.isWaiting())
		{
			_horizontalMoveRepeat.registerRepeat();
			pushAction(_horizontalMoveRepeat.action());
		}
	}

	while (!_actionQueue.empty())
	{
		switch (_actionQueue.front())
		{
		case Action::MoveLeft:
			_moveLeftTetromino();
			break;

		case Action::MoveRight:
			_moveRightTetromino();
			break;

		case Action::RotateLeft:
			_rotateLeftCurrentTetromino();
			break;

		case Action::RotateRight:
			_rotateRightCurrentTetromino();
			break;

		case Action::NormalDrop:
			_gravity.setMode(GravityClock::Mode::Normal);
			break;

		case Action::SoftDrop:
			_gravity.setMode(GravityClock::Mode::Soft);
			break;

		case Action::HardDrop:
			_gravity.setMode(GravityClock::Mode::Hard);
			break;

		case Action::Hold:
			_holdTetromino();
			break;
		}
		_actionQueue.pop();
	}
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
				_insertTetromino();
			}
			break;

		case TetrominoState::Inserting:
			_gravity.updateInserting(delta);
			if (!_gravity.isInserting())
			{
				_currentTetrominoState = TetrominoState::None;

				if(_bottomRowToErase >= 0 && _bottomRowToErase < Field::rows)
					_field.dropRows(_bottomRowToErase);
				_bottomRowToErase = -1;

				_hold.unlock();
			}
			break;

		case TetrominoState::None:
			_spawnTetromino(false);
			break;
	}
}

void Scenario::_spawnTetromino(bool useHold)
{
	if (useHold)
		_currentTetromino.build(_hold.tetromino());
	else _currentTetromino = _nextTetrominos.next();

	/* Try to situate into origin */
	_currentTetromino.setPosition(Field::rows - 5, (Field::columns / 2) - (Tetromino::columns / 2));
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
	_tetrominoInfo.set(_currentTetromino);
	_currentTetrominoState = TetrominoState::Dropping;
	_generateGhostTetromino();
}

void Scenario::_dropCurrentTetromino()
{
	Tetromino tryer = _currentTetromino;

	tryer.moveDown();
	if (_field.collide(tryer) || _field.isBottomOut(tryer))
	{
		if (_gravity.mode() == GravityClock::Mode::Hard)
		{
			_insertTetromino();
		}
		else
		{
			_currentTetrominoState = TetrominoState::Frozen;
			_gravity.freeze();
		}
		return;
	}

	_currentTetromino.moveDown();

	_tetrominoInfo.lastMove = MoveType::Drop;

	if (_gravity.mode() == GravityClock::Mode::Soft)
		_score.addSoftDropScore();
	else if (_gravity.mode() == GravityClock::Mode::Hard)
		_score.addHardDropScore();
}

void Scenario::_horizontalMoveTetromino(bool left)
{
	if (_currentTetrominoState == TetrominoState::None || _currentTetrominoState == TetrominoState::Inserting)
		return;

	Tetromino tryer = _currentTetromino;
	
	if (left)
	{
		tryer.moveLeft();
		if (!_field.collide(tryer) && !_field.isLeftOut(tryer))
			_currentTetromino.moveLeft();
	}
	else
	{
		tryer.moveRight();
		if (!_field.collide(tryer) && !_field.isRightOut(tryer))
			_currentTetromino.moveRight();
	}

	_tetrominoInfo.lastMove = MoveType::Horizontal;

	_evaluateTetrominoStateAfterAction();
}

void Scenario::_rotateCurrentTetromino(bool left)
{
	if (_currentTetrominoState == TetrominoState::None || _currentTetrominoState == TetrominoState::Inserting)
		return;

	Tetromino tryer = _currentTetromino;
	RotationState oldState = tryer.rotationState();

	if (left)
		tryer.leftRotate();
	else tryer.rightRotate();

	for (unsigned int i = 0; i < static_cast<unsigned int>(Tetromino::max_rotation_try); i++)
	{
		Tetromino tkick = tryer;
		tkick.kick(oldState, i);
		if (!_field.collide(tkick) && _field.isInside(tkick))
		{
			_currentTetromino = std::move(tkick);
			break;
		}
	}

	_tetrominoInfo.lastMove = MoveType::Rotate;
	_tetrominoInfo.rotation = _currentTetromino.rotationState();

	_evaluateTetrominoStateAfterAction();
}

void Scenario::_holdTetromino()
{
	if (_currentTetrominoState != TetrominoState::Dropping || _hold.isLock())
		return;
	
	if (_hold.empty())
	{
		_hold.hold(_currentTetromino);
		_spawnTetromino(false);
	}
	else
	{
		Tetromino::Type type = _currentTetromino.type();
		_spawnTetromino(true);
		_hold.hold(type);
	}
}

void Scenario::_evaluateTetrominoStateAfterAction()
{
	if (_currentTetrominoState == TetrominoState::Frozen)
	{
		Tetromino tryer = _currentTetromino;
		tryer.moveDown();

		if (!_field.collide(tryer) && !_field.isBottomOut(tryer))
		{
			_currentTetrominoState = TetrominoState::Dropping;
			_gravity.reset();
			_generateGhostTetromino();
		}
		else _gravity.freeze();
	}
	else if(_currentTetrominoState == TetrominoState::Dropping)
		_generateGhostTetromino();
}

void Scenario::_generateGhostTetromino()
{
	_ghostTetromino = _currentTetromino;
	_ghostTetromino.ghostify();

	Tetromino tryer = _ghostTetromino;

	int moveCount = 0;
	while (!_field.collide(tryer) && !_field.isBottomOut(tryer))
		tryer.moveDown(), ++moveCount;

	if (moveCount > 1)
		_ghostTetromino.move(-(moveCount - 1), 0);
}

void Scenario::_insertTetromino()
{
	_currentTetrominoState = TetrominoState::Inserting;

	_field.insert(_currentTetromino);
	if (_eraseCompleteLines())
		_gravity.erasingInsertion();
	else _gravity.insertion();
}

unsigned int Scenario::_eraseCompleteLines()
{
	auto cells = _currentTetromino.cellsAsVector();
	std::set<int> lines;
	for (const auto& cell : cells)
		lines.insert(cell.y);

	int erased = 0, bottomLine = Field::rows;
	for (int line : lines)
		if (_field.eraseIfComplete(line))
		{
			bottomLine = line < bottomLine ? line : bottomLine;
			erased++;
		}

	_bottomRowToErase = erased > 0 ? bottomLine : -1;

	_score.addLines(static_cast<UInt64>(erased));
	if (erased > 0)
	{
		switch (erased)
		{
			case 1: _score.addSingleScore(); break;
			case 2: _score.addDoubleScore(); break;
			case 3: _score.addTripleScore(); break;
			case 4:
			default: _score.addTetrisScore(); break;
		}
	}
	
	return static_cast<unsigned int>(erased);
}
