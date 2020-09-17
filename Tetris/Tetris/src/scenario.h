#pragma once

#include "game_basics.h"
#include "sprites.h"
#include "theme.h"



class Cell : private sf::RectangleShape
{
public:
	static constexpr Size width = 48;
	static constexpr Size height = 44;

private:
	CellColor _color;
	Offset _row;
	Offset _column;

public:
	Cell(const Cell&) = delete;
	Cell(Cell&&) noexcept = delete;

	Cell& operator= (const Cell&) = delete;
	Cell& operator= (Cell&&) noexcept = delete;


	Cell(CellColor color = CellColor::Empty);

	void changeColor(CellColor color);

	void setPosition(Offset row, Offset column);

	inline void render(sf::RenderTarget& canvas) { canvas.draw(*this); }

	inline CellColor color() const { return _color; }

	inline bool empty() const { return _color == CellColor::Empty; }

	inline operator bool() const { return _color != CellColor::Empty; }
	inline bool operator! () const { return _color == CellColor::Empty; }

	inline Cell(Offset row, Offset column, CellColor color = CellColor::Empty) : Cell(color) { setPosition(row, column); }
};

class Field
{
public:
	static constexpr Size rows = 22;
	static constexpr Size columns = 10;

private:
	static constexpr Size cellCount = rows * columns;

private:
	sf::RenderTexture _canvas;
	sf::RectangleShape _canvasShape;

	Cell _cells[cellCount] = {};
	Vec2f _position = {};
	Vec2f _size = { static_cast<float>(columns * Cell::width), static_cast<float>(rows * Cell::height) };



public:
	Field();
	Field(const Field&) = default;
	Field(Field&&) noexcept = default;
	~Field() = default;

	Field& operator= (const Field&) = default;
	Field& operator= (Field&&) noexcept = default;

	inline Cell& cell(Offset row, Offset column)
	{
		return _cells[utils::clamp(row, 0U, rows - 1) * columns + utils::clamp(column, 0U, columns - 1)];
	}

	inline const Cell& cell(Offset row, Offset column) const
	{
		return _cells[utils::clamp(row, 0U, rows - 1) * columns + utils::clamp(column, 0U, columns - 1)];
	}

	inline Cell& operator[] (const std::pair<Offset, Offset> location) { return cell(location.first, location.second); }
	inline const Cell& operator[] (const std::pair<Offset, Offset> location) const { return cell(location.first, location.second); }

	inline Vec2f& position() { return _position; }
	inline const Vec2f& position() const { return _position; }

	inline Vec2f& size() { return _size; }
	inline const Vec2f& size() const { return _size; }

public:
	void render(sf::RenderTarget& canvas);
	void update(const sf::Time& delta);
};
