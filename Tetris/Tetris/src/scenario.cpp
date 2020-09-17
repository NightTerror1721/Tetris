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

	changeColor(color);
}

void Cell::changeColor(CellColor color)
{
	_color = color;

	sf::RectangleShape::setTexture(&global::theme.cellColorTexture(color), true);
}

void Cell::setPosition(Offset row, Offset column)
{
	_row = utils::clamp(row, 0, Field::rows - 1);
	_column = utils::clamp(column, 0, Field::columns - 1);

	sf::RectangleShape::setPosition(
		static_cast<float>(_column * Cell::width),
		static_cast<float>(((Field::rows - 1) * Cell::height) - (_row * Cell::height))
	);
}


Field::Field() :
	_canvas{},
	_canvasShape{}
{
	_canvas.create(Field::columns * Cell::width, Field::rows * Cell::height);

	_canvasShape.setSize({
		static_cast<float>(Field::columns * Cell::width),
		static_cast<float>(Field::rows* Cell::height)
	});
	_canvasShape.setTexture(&_canvas.getTexture(), true);

	for (Offset idx = 0; idx < cellCount; ++idx)
	{
		if (idx / columns == 21 && (idx % columns) == 0)
		_cells[idx].setPosition(idx / columns, idx % columns);
	}
}

void Field::render(sf::RenderTarget& canvas)
{
	_canvas.clear();

	for (Offset idx = 0; idx < cellCount; idx++)
		_cells[idx].render(_canvas);

	_canvas.display();
	canvas.draw(_canvasShape);
}

void Field::update(const sf::Time& delta)
{

}
