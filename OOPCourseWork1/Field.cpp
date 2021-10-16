#include "Field.h"

void Field::InitializaField()
{
	_cells = new Cell * [_width * _height];

	for (size_t i = 0; i < _height; i++)
		for (size_t j = 0; j < _width; j++)
			_cells[_height * i + j] = new Cell(new Position(i, j), CellStatus::Empty);


	_ships = new Ship * [_maxShipAmount];
}

void Field::FillNearestCellAs(Ship* ship, CellStatus status)
{
	int shipSize = (int)ship->GetType();

	for (size_t i = 0; i < shipSize; i++)
	{
		auto currCellPos = ship->GetBody()[i]->GetPosition();

		//Левая
		if (currCellPos->x - 1 >= 0)
		{
			auto cell = _cells[_width * currCellPos->y + currCellPos->x - 1];

			if (cell->GetStatus() != CellStatus::DeckDestroyed)
				cell->SetStatus(status);
		}
		
		//Правая
		if (currCellPos->x + 1 < _width)
		{
			auto cell = _cells[_width * currCellPos->y + currCellPos->x + 1];

			if (cell->GetStatus() != CellStatus::DeckDestroyed)
				cell->SetStatus(status);
		}

		//Верхняя
		if (currCellPos->y - 1 >= 0)
		{
			auto cell = _cells[_width * (currCellPos->y - 1) + currCellPos->x];

			if (cell->GetStatus() != CellStatus::DeckDestroyed)
				cell->SetStatus(status);
		}

		//Нижняя
		if (currCellPos->y + 1 < _height)
		{
			auto cell = _cells[_width * (currCellPos->y + 1) + currCellPos->x];

			if (cell->GetStatus() != CellStatus::DeckDestroyed)
				cell->SetStatus(status);
		}

		//Верхняя левая
		if (currCellPos->x - 1 >= 0 && currCellPos->y - 1 >= 0)
		{
			auto cell = _cells[_width * (currCellPos->y - 1) + currCellPos->x - 1];

			if (cell->GetStatus() != CellStatus::DeckDestroyed)
				cell->SetStatus(status);
		}

		//Нижняя правая
		if (currCellPos->x + 1 < _width && currCellPos->y + 1 < _height)
		{
			auto cell = _cells[_width * (currCellPos->y + 1) + currCellPos->x + 1];

			if (cell->GetStatus() != CellStatus::DeckDestroyed)
				cell->SetStatus(status);
		}

		//Нижняя левая
		if (currCellPos->x - 1 >= 0 && currCellPos->y + 1 < _height)
		{
			auto cell = _cells[_width * (currCellPos->y + 1) + currCellPos->x - 1];

			if (cell->GetStatus() != CellStatus::DeckDestroyed)
				cell->SetStatus(status);
		}

		//Верхняя правая
		if (currCellPos->x + 1 < _width && currCellPos->y - 1 >= 0)
		{
			auto cell = _cells[_width * (currCellPos->y - 1) + currCellPos->x + 1];

			if (cell->GetStatus() != CellStatus::DeckDestroyed)
				cell->SetStatus(status);
		}
	}
}

Field::Field() : Field::Field(DEFAULT_FIELD_SIZE, DEFAULT_FIELD_SIZE, FieldType::Player)
{
}

Field::Field(const Field& field)
{
	_width = field.GetWidth();
	_height = field.GetHeight();
	_type = field.GetFieldType();

	_currentShipAmount = 0;
	_maxShipAmount = field._maxShipAmount;

	_cells = new Cell * [_width * _height];
	_ships = new Ship * [_maxShipAmount];

	for (size_t i = 0; i < _width * _height; i++)
		_cells[i] = new Cell(*field.GetField()[i]);

	for (size_t i = 0; i < _maxShipAmount; i++)
		_ships[i] = new Ship(*field._ships[i]);
}

Field::Field(const int width, const int height, FieldType type)
{
	_width = width;
	_height = height;
	_type = type;

	_currentShipAmount = 0;
	_maxShipAmount = DEFAULT_SHIPS_AMOUNT;

	InitializaField();
}

Field::~Field()
{
	for (size_t i = 0; i < _currentShipAmount; i++)
	{
		auto currShip = _ships[i];

		for (size_t j = 0; j < (int)currShip->GetType(); j++)
		{
			auto deckPos = currShip->GetBody()[j]->GetPosition();

			_cells[_width * deckPos->y + deckPos->x] = nullptr;
		}
	}

	if (_cells != nullptr)
		for (size_t i = 0; i < _width * _height; i++)
			if (_cells[i] != nullptr)
				delete _cells[i];

	if (_ships != nullptr)
		for (size_t i = 0; i < _maxShipAmount; i++)
			delete _ships[i];
}

void Field::SetFieldType(FieldType type)
{
	_type = type;
}

int Field::GetWidth() const
{
	return _width;
}

int Field::GetHeight() const
{
	return _height;
}

FieldType Field::GetFieldType() const
{
	return _type;
}

Cell** Field::GetField() const
{
	return _cells;
}

bool Field::TryAddShip(Ship* ship)
{
	if (_currentShipAmount == _maxShipAmount)
		return false;

	for (size_t i = 0; i < (int)ship->GetType(); i++)
	{
		auto currentPos = ship->GetBody()[i]->GetPosition();
		if (currentPos->x < 0 || currentPos->x >= _width ||
			currentPos->y < 0 || currentPos->y >= _height)
			return false;
	}

	for (size_t i = 0; i < _currentShipAmount; i++)
		if (_ships[i]->CheckIntersection(ship))
			return false;

	_ships[_currentShipAmount] = ship;

	//for (size_t i = 0; i < GetHeight(); i++)
	//{
	//	for (size_t j = 0; j < GetWidth(); j++)
	//	{
	//		switch (_cells[GetWidth() * (i) + (j)].GetStatus())
	//		{
	//		case CellStatus::Empty:
	//			std::cout << "0";
	//			break;
	//		case CellStatus::DeckAlive:
	//			std::cout << "1";
	//			break;
	//		case CellStatus::DeckDestroyed:
	//			std::cout << "2";
	//			break;
	//		case CellStatus::MissedShot:
	//			std::cout << "3";
	//			break;
	//		default:
	//			break;
	//		}
	//	}
	//	std::cout << std::endl;
	//}

	for (size_t i = 0; i < (int)ship->GetType(); i++)
	{
		auto cell = ship->GetBody()[i];
		auto cellPos = cell->GetPosition();

		auto previousCell = _cells[_width * cellPos->y + cellPos->x];
		_cells[_width * cellPos->y + cellPos->x] = cell;

#if _DEBUG
		std::cout << "Previous cell status: " << GetCellStatusAsString(previousCell->GetStatus()) << std::endl;
#endif // _DEBUG

		delete previousCell;
	}

	//for (size_t i = 0; i < GetHeight(); i++)
	//{
	//	for (size_t j = 0; j < GetWidth(); j++)
	//	{
	//		switch (_cells[GetWidth() * (i)+(j)].GetStatus())
	//		{
	//		case CellStatus::Empty:
	//			std::cout << "0";
	//			break;
	//		case CellStatus::DeckAlive:
	//			std::cout << "1";
	//			break;
	//		case CellStatus::DeckDestroyed:
	//			std::cout << "2";
	//			break;
	//		case CellStatus::MissedShot:
	//			std::cout << "3";
	//			break;
	//		default:
	//			break;
	//		}
	//	}
	//	std::cout << std::endl;
	//}

	_currentShipAmount++;
	return true;
}

bool Field::AllShipsDestroyed()
{
	bool result = true;

	for (size_t i = 0; i < _currentShipAmount; i++)
		result = result && _ships[i]->IsDestroyed();

	return result;
}

ShotResult Field::ShootToCell(const Position* pos)
{
	if (pos->x < 0 || pos->x >= _width ||
		pos->y < 0 || pos->y >= _height)
		return ShotResult::OutOfField;

	for (size_t i = 0; i < _currentShipAmount; i++)
	{
		auto dmgRes = _ships[i]->DoDamage(pos);

		if (dmgRes == DamageResult::Destroyed)
			if (_ships[i]->IsDestroyed())
			{
				FillNearestCellAs(_ships[i], CellStatus::MissedShot);

				return ShotResult::Destroyed;
			}
			else
				return ShotResult::Hitted;
		else if (dmgRes == DamageResult::AlreadyDestroyed)
			return ShotResult::AlreadyDestroyed;
	}

	auto cell = _cells[_width * pos->y + pos->x];

	if (cell->GetStatus() == CellStatus::MissedShot)
		return ShotResult::AlreadyHitted;

	cell->SetStatus(CellStatus::MissedShot);

	return ShotResult::Missed;
}
