#pragma once
#include <iostream>

enum class CellStatus : short
{
	Empty,
	DeckAlive,
	DeckDestroyed,
	MissedShot
};

struct Position
{
	int x, y;

	Position();
	Position(int x, int y);
};

class Cell
{
private:
	CellStatus _status;
	Position* _position;

	int _width, _height;

public:
	static const int DEFAULT_CELL_SIZE = 10;

	Cell();
	Cell(Position* position);
	Cell(const Cell& cell);
	Cell(Position* position, CellStatus status);
	~Cell();

	void SetStatus(CellStatus status);
	void SetPosition(Position* position);
	
	CellStatus GetStatus() const;
	const Position* GetPosition() const;
	int GetWidth() const;
	int GetHeight() const;
};

bool operator==(const Position& left, const Position& right);
bool operator!=(const Position& left, const Position& right);

const char* GetCellStatusAsString(CellStatus status);