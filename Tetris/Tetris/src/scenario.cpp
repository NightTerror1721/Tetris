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

unsigned int Field::TSlotCorners(const Tetromino& tetromino)
{
	#define check_corner(_Row, _Column, _Corner) \
	if((_Row) >= 0 && (_Row) < Field::rows && (_Column) >= 0 && (_Column) < Field::columns && \
			_cells[(_Row) * Field::columns + (_Column)]) ++(_Corner)

	if (tetromino.type() != Tetromino::Type::T)
		return 0;

	int row = tetromino.row();
	int column = tetromino.column();
	unsigned int corners = 0;

	check_corner(row, column, corners);
	check_corner(row, column + 2, corners);
	check_corner(row + 2, column, corners);
	check_corner(row + 2, column + 2, corners);

	return corners;

	#undef check_corner
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
	_tPoints.setString("score: " + std::to_string(_points));
	_tPoints.setFillColor(sf::Color::White);
	utils::centrate_text(_tPoints, {}, { static_cast<float>(Score::display_width), static_cast<float>(Score::display_height) });
}

void Score::_updateLinesText()
{
	_tLines.setString("lines: " + std::to_string(_lines));
	_tLines.setFillColor(sf::Color::White);
	utils::centrate_text(_tLines,
		{ 0, static_cast<float>(Score::display_height) },
		{ static_cast<float>(Score::display_width), static_cast<float>(Score::display_height) }
	);
}

void Score::_updateLevelText()
{
	_tLevel.setString("level: " + std::to_string(_level));
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

void Score::setLevel(unsigned int level)
{
	_level = level < 1 ? 1 : level;
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

	UInt64 amount = static_cast<UInt64>(base) * static_cast<UInt64>(_level);
	_remainingPoints += amount;
}







void TetrominoScenarioInfo::set(const Tetromino& tetromino, MoveType moveType)
{
	lastMove = moveType;
	type = tetromino.type();
	rotation = tetromino.rotationState();
	kicks = 0;
}

void TetrominoScenarioInfo::registerDrop()
{
	lastMove = MoveType::Drop;
	kicks = 0;
}
void TetrominoScenarioInfo::registerHorizontal()
{
	lastMove = MoveType::Horizontal;
	kicks = 0;
}
void TetrominoScenarioInfo::registerRotate(RotationState rotation, unsigned int kicks)
{
	lastMove = MoveType::Rotate;
	this->rotation = rotation;
	this->kicks = kicks;
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
	_linesPerLevel{ 10 },
	_currentLevel{ 1 },
	_tetrominoInfo{},
	_horizontalMoveRepeat{},
	_gravity{},
	_score{},
	_state{ State::Running },
	_pauseButton{ false },
	_pause{ PauseState::None },
	_pauseCountdown{},
	_pauseText{},
	_sounds{},
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

	_pauseText.setCharacterSize(60);
	_pauseText.setFillColor(sf::Color::White);
	_pauseText.setFont(global::fonts.get("arial"));
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

	if (_pause != PauseState::None)
		fcanvas.draw(_pauseText);

	renderCanvas(canvas);
}

void Scenario::update(const sf::Time& delta)
{
	if (_state == State::Running)
	{
		if (_pause == PauseState::Resuming)
		{
			_pauseCountdown -= delta;
			if (_pauseCountdown <= sf::Time::Zero)
				_pause = PauseState::None;
			else
			{
				int secs = static_cast<int>(_pauseCountdown.asSeconds() + 1);
				secs = utils::clamp(secs, 0, 3);
				_pauseText.setString(std::to_string(secs));
				utils::centrate_text(_pauseText, {}, getSize());

				_clearActionsQueue();

				goto sound_part;
			}
		}
		else if (_pause == PauseState::Paused)
		{
			_clearActionsQueue();
			goto sound_part;
		}

		_updateActions(delta);
		_updateCurrentTetromino(delta);
		_score.update(delta);

		sound_part:
		_sounds.update();
	}
}

void Scenario::dispatchEvent(const sf::Event& event)
{
	if (event.type == sf::Event::KeyPressed)
	{
		KeyboardKey key = event.key.code;
		switch (key)
		{
			case default_control::move_left:
				if (_horizontalMoveRepeat.action() != Action::MoveLeft)
				{
					pushAction(Action::MoveLeft);
					_horizontalMoveRepeat.registerAction(Action::MoveLeft);
				}
				break;

			case default_control::move_right:
				if (_horizontalMoveRepeat.action() != Action::MoveRight)
				{
					pushAction(Action::MoveRight);
					_horizontalMoveRepeat.registerAction(Action::MoveRight);
				}
				break;

			case default_control::rotate_left:
				pushAction(Action::RotateLeft);
				break;

			case default_control::rotate_right:
				pushAction(Action::RotateRight);
				break;

			case default_control::harddrop:
				pushAction(Action::HardDrop);
				break;

			case default_control::softdrop:
				pushAction(Action::SoftDrop);
				break;

			case default_control::hold:
				pushAction(Action::Hold);
				break;

			case sf::Keyboard::Escape:
				_setPause(_pause != PauseState::Paused);
				break;
		}
	}
	else if (event.type == sf::Event::KeyReleased)
	{
		sf::Keyboard::Key key = event.key.code;
		switch (key)
		{
			case default_control::move_left:
				if (_horizontalMoveRepeat.action() == Action::MoveLeft)
					_horizontalMoveRepeat.releaseAction();
				break;

			case default_control::move_right:
				if (_horizontalMoveRepeat.action() == Action::MoveRight)
					_horizontalMoveRepeat.releaseAction();
				break;

			case default_control::softdrop:
			case default_control::harddrop:
				pushAction(Action::NormalDrop);
				break;

			case sf::Keyboard::Escape:
				_pauseButton = false;
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

				if (_bottomRowToErase >= 0 && _bottomRowToErase < Field::rows)
				{
					_field.dropRows(_bottomRowToErase);
					_playSound(sound_id::drop_after_clear);
				}
				_bottomRowToErase = -1;

				_checkLevel();

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
			_playSound(sound_id::tetrimino_harddrop);
		}
		else
		{
			_currentTetrominoState = TetrominoState::Frozen;
			_gravity.freeze();
		}
		return;
	}

	_currentTetromino.moveDown();

	_tetrominoInfo.registerDrop();

	if (_gravity.mode() == GravityClock::Mode::Soft)
	{
		_score.addSoftDropScore();
		_playSound(sound_id::tetrimino_softdrop);
	}
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
		{
			_currentTetromino.moveLeft();
			_playSound(sound_id::tetrimino_move);
		}
	}
	else
	{
		tryer.moveRight();
		if (!_field.collide(tryer) && !_field.isRightOut(tryer))
		{
			_currentTetromino.moveRight();
			_playSound(sound_id::tetrimino_move);
		}
	}

	_tetrominoInfo.registerHorizontal();

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

	unsigned int kickId = 0;
	for (; kickId < static_cast<unsigned int>(Tetromino::max_rotation_try); kickId++)
	{
		Tetromino tkick = tryer;
		tkick.kick(oldState, kickId);
		if (!_field.collide(tkick) && _field.isInside(tkick))
		{
			_currentTetromino = std::move(tkick);
			_playSound(sound_id::tetrimino_rotate);
			break;
		}
	}

	_tetrominoInfo.registerRotate(_currentTetromino.rotationState(), kickId);

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

	_playSound(sound_id::tetrimino_hold);
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
	else
	{
		_gravity.insertion();
		_playSound(sound_id::tetrimino_hit);
	}
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

	if (_tetrominoInfo.type == Tetromino::Type::T &&
		_tetrominoInfo.lastMove == TetrominoScenarioInfo::MoveType::Rotate &&
		_field.TSlotCorners(_currentTetromino) > 2)
	{
		erased = erased < 0 ? 0 : erased;

		if (_tetrominoInfo.kicks > 0 && _tetrominoInfo.kicks < 3)
		{
			switch (erased)
			{
				case 0: _score.addTSpinMiniNoLinesScore(); break;
				case 1: _score.addTSpinMiniSingleScore(), _playSound(sound_id::single_line); break;
				case 2: _score.addTSpinMiniDoubleScore(), _playSound(sound_id::double_line); break;
				case 3:
				default: _score.addTSpinTripleScore(), _playSound(sound_id::triple_line); break;
			}
			_playSound(sound_id::special_clear);
		}
		else
		{
			switch (erased)
			{
				case 0: _score.addTSpinNoLinesScore(); break;
				case 1: _score.addTSpinSingleScore(), _playSound(sound_id::single_line); break;
				case 2: _score.addTSpinDoubleScore(), _playSound(sound_id::double_line); break;
				case 3:
				default: _score.addTSpinTripleScore(), _playSound(sound_id::triple_line); break;
			}
		}
	}
	else if (erased > 0)
	{
		switch (erased)
		{
			case 1: _score.addSingleScore(), _playSound(sound_id::single_line); break;
			case 2: _score.addDoubleScore(), _playSound(sound_id::double_line); break;
			case 3: _score.addTripleScore(), _playSound(sound_id::triple_line); break;
			case 4:
			default: _score.addTetrisScore(), _playSound(sound_id::tetris_line); break;
		}
	}
	
	return static_cast<unsigned int>(erased);
}

void Scenario::_checkLevel()
{
	unsigned int level = (static_cast<unsigned int>(_score.lines() / static_cast<UInt64>(_linesPerLevel)) + 1);
	if (level != _currentLevel)
	{
		_currentLevel = level;
		_gravity.setGravityLevel(level);
		_score.setLevel(level);
	}
}

void Scenario::_setPause(bool paused)
{
	if (_pauseButton)
		return;

	_pauseButton = true;

	if (paused)
	{
		if (_pause == PauseState::Paused)
			return;

		_pause = PauseState::Paused;
		_pauseText.setString("PAUSED");
		utils::centrate_text(_pauseText, {}, getSize());
	}
	else
	{
		if (_pause != PauseState::Paused)
			return;

		_pause = PauseState::Resuming;
		_pauseCountdown = sf::seconds(3);
		_pauseText.setString("3");
		utils::centrate_text(_pauseText, {}, getSize());
	}
}

void Scenario::_clearActionsQueue()
{
	while (!_actionQueue.empty())
		_actionQueue.pop();
}
