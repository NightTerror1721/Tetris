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

	void ghostify();

	void setPosition(int row, int column);

	inline void render(sf::RenderTarget& canvas) const
	{
		if(_color != CellColor::Empty)
			canvas.draw(*this);
	}

	inline CellColor color() const { return _color; }

	inline bool empty() const { return _color == CellColor::Empty; }

	inline operator bool() const { return _color != CellColor::Empty; }
	inline bool operator! () const { return _color == CellColor::Empty; }

	inline Cell(int row, int column, CellColor color = CellColor::Yellow) : Cell(color) { setPosition(row, column); }

	inline int row() const { return _row; }
	inline int column() const { return _column; }
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
	static constexpr int type_count = static_cast<int>(Type::Z) + 1;

	static constexpr int width = rows * Cell::width;
	static constexpr int height = columns * Cell::height;

	static constexpr int max_rotation_try = 5;

private:
	static constexpr int cellCount = rows * columns;

private:
	Cell _cells[cellCount];
	int _row = 0;
	int _column = 0;
	Type _type = Type::I;
	RotationState _rotation = RotationState::origin();

	mutable int _idx[4] = {};
	mutable bool _validIdx = false;

	mutable Vec2i _vecs[4] = {};
	mutable bool _validVecs = false;

public:
	Tetromino() = default;
	Tetromino(const Tetromino&) = default;
	Tetromino(Tetromino&&) noexcept = default;
	~Tetromino() = default;

	Tetromino& operator= (const Tetromino&) = default;
	Tetromino& operator= (Tetromino&&) noexcept = default;

	void render(sf::RenderTarget& canvas) const;

	void build(Type type);

	void ghostify();

	void setPosition(int row, int column);

	void move(int rowDelta, int columnDelta);
	void moveToOrigin();

	void leftRotate();
	void rightRotate();

	void kick(RotationState prevState, unsigned int tryId);

	std::array<int, 4> cellsIndex() const;
	std::array<Vec2i, 4> cellsAsVector() const;

	CellColor color() const;

public:

	inline void moveDown() { move(-1, 0); }
	inline void moveLeft() { move(0, -1); }
	inline void moveRight() { move(0, 1); }

	inline int row() const { return _row; }
	inline int column() const { return _column; }
	inline Vec2i getPosition() const { return { _column, _row }; }

	inline Type type() const { return _type; }

	inline RotationState rotationState() const { return _rotation; }

private:
	static Vec2i _kickFactors(unsigned int tryId, Type type, RotationState rstate);
};



class Field : public Frame
{
public:
	static constexpr int rows = 22;
	static constexpr int columns = 10;
	static constexpr int visible_rows = rows - 2;

	static constexpr int width = columns * Cell::width;
	static constexpr int height = visible_rows * Cell::height;

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

	void render(sf::RenderTarget& canvas, const Tetromino* tetromino = nullptr, const Tetromino* ghost = nullptr);
	void update(const sf::Time& delta);

	bool collide(const Tetromino& tetromino);
	bool isTopOut(const Tetromino& tetromino);
	bool isBottomOut(const Tetromino& tetromino);
	bool isLeftOut(const Tetromino& tetromino);
	bool isRightOut(const Tetromino& tetromino);
	bool isInside(const Tetromino& tetromino);

	void insert(const Tetromino& tetromino);

	bool eraseIfComplete(int row);

	void dropRows(int bottomRow);

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



class GravityClock
{
private:
	static constexpr Int64 min_waiting_time = static_cast<Int64>(0.05 / 60.0 * 1000000.0); // 20G //
	static constexpr Int64 soft_drop_time = static_cast<Int64>(1.0 / 60.0 * 1000000.0); // 1G //
	static constexpr Int64 freeze_time = static_cast<Int64>(0.5 * 1000000.0); // 0.5 seconds //
	static constexpr Int64 insertion_time = static_cast<Int64>(0.5 * 1000000.0); // 0.5 seconds //
	static constexpr Int64 insertion_with_erase_time = static_cast<Int64>(0.75 * 1000000.0); // 0.75 seconds //


public:
	enum class Mode { Normal, Soft, Hard };

private:
	sf::Time _waiting;
	sf::Time _remaining;
	sf::Time _freezing;
	sf::Time _inserting;
	Mode _mode = Mode::Normal;

public:
	GravityClock() = default;
	GravityClock(const GravityClock&) = default;
	GravityClock(GravityClock&&) noexcept = default;
	~GravityClock() = default;

	GravityClock& operator= (const GravityClock&) = default;
	GravityClock& operator= (GravityClock&&) noexcept = default;

	void setGravityLevel(unsigned int level);

	void updateWaiting(const sf::Time& delta);
	void updateFreezing(const sf::Time& delta);
	void updateInserting(const sf::Time& delta);

	void registerDrop();

	void resetWaiting();

	void setMode(Mode mode);

	inline void resetMode() { _mode = Mode::Normal; }
	inline Mode mode() const { return _mode; }

	inline void freeze() { _freezing = sf::microseconds(freeze_time); }
	inline void insertion() { _inserting = sf::microseconds(insertion_time); }
	inline void erasingInsertion() { _inserting = sf::microseconds(insertion_with_erase_time); }
	inline void resetFreezing() { _freezing = sf::Time::Zero; }
	inline void resetInserting() { _inserting = sf::Time::Zero; }
	inline void reset() { resetMode(), resetWaiting(), resetFreezing(); }
	
	inline bool isFrozen() const { return _freezing > sf::Time::Zero; }
	inline bool isWaiting() const
	{
		return _mode != Mode::Hard && _remaining > sf::Time::Zero;
	}
	inline bool isInserting() const { return _inserting > sf::Time::Zero; }
};



class TetrominoBag
{
private:
	Tetromino::Type _bag[Tetromino::type_count] = {};
	unsigned int _remaining = 0;

public:
	TetrominoBag() = default;
	TetrominoBag(const TetrominoBag&) = default;
	TetrominoBag(TetrominoBag&&) noexcept = default;
	~TetrominoBag() = default;

	TetrominoBag& operator= (const TetrominoBag&) = default;
	TetrominoBag& operator= (TetrominoBag&&) noexcept = default;

	Tetromino::Type take();

private:
	void _generate();
};



class TetrominoManager : public Frame
{
public:
	static constexpr int next_count = 5;

	static constexpr int width = static_cast<int>(Tetromino::width * 0.6);
	static constexpr int height = static_cast<int>(Tetromino::height * TetrominoManager::next_count * 0.6);

private:
	TetrominoBag _bag;
	std::list<Tetromino> _next;

public:
	TetrominoManager();
	TetrominoManager(const TetrominoManager&) = default;
	TetrominoManager(TetrominoManager&&) noexcept = default;
	~TetrominoManager() = default;

	TetrominoManager& operator= (const TetrominoManager&) = default;
	TetrominoManager& operator= (TetrominoManager&&) noexcept = default;

	Tetromino next();

	void render(sf::RenderTarget& canvas);

private:
	void generate();
};



class HoldManager : public Frame
{
public:
	static constexpr int width = static_cast<int>(Tetromino::width * 0.6);
	static constexpr int height = static_cast<int>(Tetromino::height * 0.6);

private:
	Tetromino _tetromino;
	bool _empty;
	bool _lock;

public:
	HoldManager();
	HoldManager(const HoldManager&) = default;
	HoldManager(HoldManager&&) noexcept = default;
	~HoldManager() = default;

	HoldManager& operator= (const HoldManager&) = default;
	HoldManager& operator= (HoldManager&&) noexcept = default;

	void render(sf::RenderTarget& canvas);

	void hold(Tetromino::Type type);

	inline bool empty() const { return _empty; }

	inline void hold(const Tetromino& tetromino) { hold(tetromino.type()); }

	inline const Tetromino& tetromino() const { return _tetromino; }

	inline bool isLock() { return _lock; }

	inline void unlock() { _lock = false; }
};



enum class ScenarioAction
{
	None,
	MoveLeft,
	MoveRight,
	RotateLeft,
	RotateRight,
	NormalDrop,
	SoftDrop,
	HardDrop,
	Hold
};



class ActionRepeatManager
{
public:
	static constexpr Int64 auto_repeat_delay = static_cast<Int64>(170 * 1000); /* 170 milliseconds */
	static constexpr Int64 auto_repeat_speed = static_cast<Int64>(50 * 1000); /* 50 milliseconds */

private:
	sf::Time _delay;
	sf::Time _speed;
	ScenarioAction _action = ScenarioAction::None;

public:
	ActionRepeatManager() = default;
	ActionRepeatManager(const ActionRepeatManager&) = default;
	ActionRepeatManager(ActionRepeatManager&&) noexcept = default;
	~ActionRepeatManager() = default;

	ActionRepeatManager& operator= (const ActionRepeatManager&) = default;
	ActionRepeatManager& operator= (ActionRepeatManager&&) noexcept = default;

	void update(const sf::Time& delta);

	void registerAction(ScenarioAction action);

	inline void releaseAction() { registerAction(ScenarioAction::None); }

	inline bool isRepeating() const { return _action != ScenarioAction::None && _delay <= sf::Time::Zero; }
	inline bool isWaiting() const { return _action == ScenarioAction::None || _speed > sf::Time::Zero; }
	inline void registerRepeat() { _speed += sf::microseconds(auto_repeat_speed); }
	inline ScenarioAction action() const { return _action; }
};



class Scenario : public Frame
{
public:
	static constexpr int hold_border = 10;
	static constexpr int next_border = 10;

	static constexpr int width = Field::width + TetrominoManager::width + HoldManager::width + (hold_border * 2) + (next_border * 2);
	static constexpr int height = Field::height + 180;

public:
	enum class State { Stopped, Running, GameOver };
	

private:
	enum class TetrominoState { None, Dropping, Frozen, Inserting };
	using Action = ScenarioAction;

private:
	Field _field;

	HoldManager _hold;
	TetrominoManager _nextTetrominos;
	Tetromino _currentTetromino;
	Tetromino _ghostTetromino;
	TetrominoState _currentTetrominoState;

	int _bottomRowToErase;

	ActionRepeatManager _horizontalMoveRepeat;

	GravityClock _gravity;

	State _state;

	std::queue<ScenarioAction> _actionQueue;

public:
	Scenario();
	Scenario(const Scenario&) = delete;
	Scenario(Scenario&&) noexcept = default;
	~Scenario() = default;

	Scenario& operator= (const Scenario&) = delete;
	Scenario& operator= (Scenario&&) noexcept = default;

	inline State state() const { return _state; }

	inline Field& field() { return _field; }
	inline TetrominoManager& nextTetrominoManager() { return _nextTetrominos; }
	inline HoldManager& holdManager() { return _hold; }

	inline void setLevel(unsigned int level) { _gravity.setGravityLevel(level); }

	inline void pushAction(ScenarioAction action) { _actionQueue.push(action); }

public:
	void render(sf::RenderTarget& canvas);

	void update(const sf::Time& delta);

	void dispatchEvent(const sf::Event& event);

private:
	void _updateActions(const sf::Time& delta);
	void _updateCurrentTetromino(const sf::Time& delta);

	void _spawnTetromino(bool useHold);
	void _dropCurrentTetromino();
	void _horizontalMoveTetromino(bool left);
	void _rotateCurrentTetromino(bool left);
	void _holdTetromino();

	void _evaluateTetrominoStateAfterAction();

	void _generateGhostTetromino();

	void _insertTetromino();

	unsigned int _eraseCompleteLines();

private:
	inline void _moveLeftTetromino() { _horizontalMoveTetromino(true); }
	inline void _moveRightTetromino() { _horizontalMoveTetromino(false); }

	inline void _rotateLeftCurrentTetromino() { _rotateCurrentTetromino(true); }
	inline void _rotateRightCurrentTetromino() { _rotateCurrentTetromino(false); }
};
