#pragma once

#include "game_basics.h"
#include "sprites.h"
#include "theme.h"


class Cell : private sf::RectangleShape
{
public:
	static constexpr int width = 48;
	static constexpr int height = 44;

private:
	CellColor _color;
	int _row;
	int _column;

public:
	Cell(const Cell&) = default;
	Cell(Cell&&) noexcept = default;

	Cell& operator= (const Cell&) = default;
	Cell& operator= (Cell&&) noexcept = default;


	Cell(CellColor color = CellColor::Empty);

	void changeColor(CellColor color);

	void setPosition(int row, int column);

	inline void render(sf::RenderTarget& canvas) const
	{
		canvas.draw(*this);
	}

	inline CellColor color() const { return _color; }

	inline bool empty() const { return _color == CellColor::Empty; }

	inline operator bool() const { return _color != CellColor::Empty; }
	inline bool operator! () const { return _color == CellColor::Empty; }

	inline Cell(int row, int column, CellColor color = CellColor::Yellow) : Cell(color) { setPosition(row, column); }
};



struct RotationState
{
	static constexpr int Origin = 0;
	static constexpr int Right = 1;
	static constexpr int Inverse = 2;
	static constexpr int Left = 3;

	int state;

	constexpr RotationState() : state{ Origin } {}
	constexpr RotationState(int state) : state{ utils::clamp(state, Origin, Left) } {}

	inline bool isOrigin() { return state == Origin; }
	inline bool isRight() { return state == Right; }
	inline bool isInverse() { return state == Inverse; }
	inline bool isLeft() { return state == Left; }

	static constexpr RotationState origin() { return Origin; }
	static constexpr RotationState right() { return Right; }
	static constexpr RotationState inverse() { return Inverse; }
	static constexpr RotationState left() { return Left; }

	static constexpr bool isOrigin(RotationState state) { return state.state == Origin; }
	static constexpr bool isRight(RotationState state) { return state.state == Right; }
	static constexpr bool isInverse(RotationState state) { return state.state == Inverse; }
	static constexpr bool isLeft(RotationState state) { return state.state == Left; }
};

constexpr RotationState& operator++ (RotationState& state)
{
	return (state.state = state.state == RotationState::Left ? RotationState::Origin : state.state + 1), state;
}
constexpr RotationState operator++ (RotationState& state, int)
{
	RotationState copy = state;
	return ++state, copy;
}

constexpr RotationState& operator-- (RotationState& state)
{
	return (state.state = state.state == RotationState::Origin ? RotationState::Left : state.state - 1), state;
}
constexpr RotationState operator-- (RotationState& state, int)
{
	RotationState copy = state;
	return --state, copy;
}

constexpr bool operator== (RotationState left, RotationState right) { return left.state == right.state; }
constexpr bool operator!= (RotationState left, RotationState right) { return left.state != right.state; }



class Tetromino
{
public:
	enum class Type { I, O, T, J, L, S, Z };

public:
	static constexpr int rows = 4;
	static constexpr int columns = 4;

private:
	static constexpr int cellCount = rows * columns;

private:
	Cell _cells[cellCount];
	int _row = 0;
	int _column = 0;
	Type _type;
	RotationState _rotation = RotationState::origin();

	mutable int _idx[4];
	mutable bool _validIdx = false;

public:
	Tetromino() = default;
	Tetromino(const Tetromino&) = default;
	Tetromino(Tetromino&&) noexcept = default;
	~Tetromino() = default;

	Tetromino& operator= (const Tetromino&) = default;
	Tetromino& operator= (Tetromino&&) noexcept = default;

	void render(sf::RenderTarget& canvas) const;

	void build(Type type);

	void setPosition(int row, int column);

	void move(int rowDelta, int columnDelta);

	void leftRotate();
	void rightRotate();

	std::array<int, 4> cellsIndex() const;

public:
	inline void moveDown() { move(-1, 0); }
};



class Field : public Frame
{
public:
	static constexpr int rows = 22;
	static constexpr int columns = 10;
	static constexpr int visible_rows = 20;

private:
	static constexpr int cellCount = rows * columns;
	static constexpr int visibleCellCount = visible_rows * columns;

private:
	Cell _cells[cellCount];

public:
	Field();
	Field(const Field&) = default;
	Field(Field&&) noexcept = default;
	~Field() = default;

	Field& operator= (const Field&) = default;
	Field& operator= (Field&&) noexcept = default;

	void render(sf::RenderTarget& canvas, const Tetromino* tetromino = nullptr);
	void update(const sf::Time& delta);

	bool collide(const Tetromino& tetromino);
	bool isTopOut(const Tetromino& tetromino);
	bool isBottomOut(const Tetromino& tetromino);

	inline Cell& cell(int row, int column)
	{
		return _cells[utils::clamp(row, 0, rows - 1) * columns + utils::clamp(column, 0, columns - 1)];
	}

	inline const Cell& cell(int row, int column) const
	{
		return _cells[utils::clamp(row, 0, rows - 1) * columns + utils::clamp(column, 0, columns - 1)];
	}

	inline Cell& operator[] (const std::pair<int, int> location) { return cell(location.first, location.second); }
	inline const Cell& operator[] (const std::pair<int, int> location) const { return cell(location.first, location.second); }
};



class Scenario : public Frame
{
private:
	static constexpr int maxQueuedTetronimos = 4;

private:
	Field _field;
	std::list<Tetromino> _nextTetrominos;
	Tetromino _currentTetromino;

	sf::Time _remainingToDown;
	sf::Time _timeToDown;

public:
	Scenario();
	Scenario(const Scenario&) = delete;
	Scenario(Scenario&&) noexcept = default;
	~Scenario() = default;

	Scenario& operator= (const Scenario&) = delete;
	Scenario& operator= (Scenario&&) noexcept = default;
};
