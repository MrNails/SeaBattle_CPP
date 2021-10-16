#include "Cell.h"

Cell::Cell() : Cell::Cell(new Position(0, 0), CellStatus::Empty)
{}

Cell::Cell(Position* position) : Cell::Cell(position, CellStatus::Empty)
{}

Cell::Cell(const Cell& cell)
{
	_position = new Position(cell.GetPosition()->x, cell.GetPosition()->y);
	_status = cell.GetStatus();
}

Cell::Cell(Position* position, CellStatus status)
{
	SetPosition(position);
	SetStatus(status);

	_width = DEFAULT_CELL_SIZE;
	_height = DEFAULT_CELL_SIZE;
}

Cell::~Cell()
{
	if (_position != nullptr)
		delete _position;

	_position = nullptr;

#if _DEBUG
	std::cout << "Cell destructor" << std::endl;
#endif // _DEBUG
}

void Cell::SetStatus(CellStatus status)
{
	_status = status;
}

void Cell::SetPosition(Position* position)
{
#if _DEBUG
	if (position == nullptr)
		if (_position != nullptr)
			std::cout << "Attempt to set null position to cell with coord {" << _position->x << ";" << _position->y << "}\n";
		else
			std::cout << "Attempt to set null position to new cell\n";
	else
		_position = position;
#else
	if (position == nullptr)
		_position = new Position(0, 0);
	else
		_position = position;
#endif // _DEBUG
}

CellStatus Cell::GetStatus() const
{
	return _status;
}

const Position* Cell::GetPosition() const
{
	return _position;
}

int Cell::GetWidth() const
{
	return _width;
}

int Cell::GetHeight() const
{
	return _height;
}

Position::Position() : Position::Position(0, 0)
{}

Position::Position(int x, int y)
{
	this->x = x;
	this->y = y;
}

bool operator==(const Position& left, const Position& right)
{
	return left.x == right.x && left.y == right.y;
}

bool operator!=(const Position& left, const Position& right)
{
	return !(left == right);
}

const char* GetCellStatusAsString(CellStatus status)
{
	switch (status)
	{
	case CellStatus::Empty:
		return "Empty";
	case CellStatus::DeckAlive:
		return "DeckAlive";
	case CellStatus::DeckDestroyed:
		return "DeckDestroyed";
	case CellStatus::MissedShot:
		return "MissedShot";
	default:
		return "";
	}
}
