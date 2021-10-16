#include "Ship.h"

void Ship::InitializaShip(ShipType type)
{
	int shipSize = (int)type;

	_destroyedCellsAmount = 0;
	_type = type;

	_body = new Cell*[shipSize];

	for (size_t i = 0; i < shipSize; i++)
		_body[i] = new Cell(new Position(0, 0), CellStatus::DeckAlive);
}

Ship::Ship() : Ship::Ship(ShipType::_1DeckShip)
{
}

Ship::Ship(const Ship& ship)
{
	_rotation = ship.GetRotation();
	_type = ship.GetType();
	_destroyedCellsAmount = ship._destroyedCellsAmount;

	_body = new Cell * [(int)_type];

	for (size_t i = 0; i < (int)ship.GetType(); i++)
		_body[i] = new Cell(*ship._body[i]);
}

Ship::Ship(ShipType type)
{
	InitializaShip(type);
}

Ship::~Ship()
{
	if (_body != nullptr)
		for (size_t i = 0; i < (int)_type; i++)
			if (_body[i] != nullptr)
				delete _body[i];

	_body = nullptr;
}

void Ship::SetRotation(ShipRotation rotation)
{
	_rotation = rotation;
}

void Ship::SetNewHeadPosition(const Position* position)
{
	auto oldHeadPos = _body[0]->GetPosition();
	Position* previousPos = (Position*)position;

	_body[0]->SetPosition((Position*)position);

	switch (_rotation)
	{
	case ShipRotation::Up:
		for (size_t i = 1; i < (int)_type; i++)
		{
			auto pos = new Position(previousPos->x, previousPos->y + 1);
			previousPos = pos;

			auto oldPos = _body[i]->GetPosition();
			_body[i]->SetPosition(pos);

			delete oldPos;
		}
		break;
	case ShipRotation::Down:
		for (size_t i = 1; i < (int)_type; i++)
		{
			auto pos = new Position(previousPos->x, previousPos->y - 1);
			previousPos = pos;

			auto oldPos = _body[i]->GetPosition();
			_body[i]->SetPosition(pos);

			delete oldPos;
		}
		break;
	case ShipRotation::Right:
		for (size_t i = 1; i < (int)_type; i++)
		{
			auto pos = new Position(previousPos->x - 1, previousPos->y);
			previousPos = pos;

			auto oldPos = _body[i]->GetPosition();
			_body[i]->SetPosition(pos);

			delete oldPos;
		}
		break;
	case ShipRotation::Left:
		for (size_t i = 1; i < (int)_type; i++)
		{
			auto pos = new Position(previousPos->x + 1, previousPos->y);
			previousPos = pos;

			auto oldPos = _body[i]->GetPosition();
			_body[i]->SetPosition(pos);

			delete oldPos;
		}
		break;
	default:
		break;
	}

#ifdef _DEBUG
	std::cout << "New body of " << _type << "ship is: ";
	for (size_t i = 0; i < (int)_type; i++)
		std::cout << _body[i]->GetPosition()->x << ";" << _body[i]->GetPosition()->y << " ";
	std::cout << std::endl;
#endif // DEBUG

	previousPos = nullptr;

	delete oldHeadPos;
}

Cell** Ship::GetBody() const
{
	return _body;
}

ShipRotation Ship::GetRotation() const
{
	return _rotation;
}

ShipType Ship::GetType() const
{
	return _type;
}

DamageResult Ship::DoDamage(const Position* position)
{
	for (size_t i = 0; i < (int)_type; i++)
	{
		auto deck = _body[i];

		if (*deck->GetPosition() == *position)
		{
			if (deck->GetStatus() == CellStatus::DeckDestroyed)
				return DamageResult::AlreadyDestroyed;

			deck->SetStatus(CellStatus::DeckDestroyed);
			_destroyedCellsAmount++;
			return DamageResult::Destroyed;
		}
	}

	return DamageResult::Missed;
}

bool Ship::IsDestroyed()
{
	return _destroyedCellsAmount == (int)_type;
}

bool Ship::CheckIntersection(const Ship* otherShip)
{
	for (size_t i = 0; i < (int)_type; i++)
	{
		for (size_t j = 0; j < (int)otherShip->GetType(); j++)
		{
			auto deckPos = _body[i]->GetPosition();
			auto otherDeckPos = otherShip->GetBody()[j]->GetPosition();

			if (*deckPos == *otherDeckPos ||
				deckPos->x + 1 == otherDeckPos->x && deckPos->y == otherDeckPos->y ||
				deckPos->x - 1 == otherDeckPos->x && deckPos->y == otherDeckPos->y ||
				deckPos->x == otherDeckPos->x && deckPos->y + 1 == otherDeckPos->y ||
				deckPos->x == otherDeckPos->x && deckPos->y - 1 == otherDeckPos->y ||
				deckPos->x + 1 == otherDeckPos->x && deckPos->y - 1 == otherDeckPos->y ||
				deckPos->x - 1 == otherDeckPos->x && deckPos->y + 1 == otherDeckPos->y ||
				deckPos->x + 1 == otherDeckPos->x && deckPos->y + 1 == otherDeckPos->y ||
				deckPos->x - 1 == otherDeckPos->x && deckPos->y - 1 == otherDeckPos->y)
				return true;
		}
	}

	return false;
}
