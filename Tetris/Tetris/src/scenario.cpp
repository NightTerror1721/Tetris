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
	sf::RectangleShape::setTexture(&global::theme.cellColorTexture(color), true);
}

void Cell::changeColor(CellColor color)
{
	if (_color != color)
	{
		_color = color;
		sf::RectangleShape::setTexture(&global::theme.cellColorTexture(color), true);
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
		{ columns * Cell::width, visible_rows * Cell::height },
		{ static_cast<float>(columns * Cell::width), static_cast<float>(visible_rows * Cell::height) }
	},
	_cells{}
{

	for (Offset idx = 0; idx < cellCount; ++idx)
		_cells[idx].setPosition(idx / columns, idx % columns);
}

void Field::render(sf::RenderTarget& canvas, const Tetromino* tetromino)
{
	clearCanvas();

	sf::RenderTarget& frameCanvas = Frame::canvas();

	for (int idx = 0; idx < visibleCellCount; idx++)
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
