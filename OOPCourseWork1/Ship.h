#pragma once
#include <iostream>
#include "Cell.h"

enum ShipType
{
	_1DeckShip = 1,
	_2DeckShip = 2,
	_3DeckShip = 3,
	_4DeckShip = 4
};

enum class ShipRotation
{
	Up,
	Down,
	Right,
	Left
};

enum class DamageResult
{
	Destroyed,
	Missed,
	AlreadyDestroyed
};

class Ship
{
private:
	Cell** _body;
	ShipRotation _rotation;
	ShipType _type;
	int _destroyedCellsAmount;

	void InitializaShip(ShipType);

public:
	Ship();
	Ship(const Ship& ship);
	Ship(ShipType type);
	~Ship();

	void SetRotation(ShipRotation rotation);
	void SetNewHeadPosition(const Position* position);

	Cell** GetBody() const;
	ShipRotation GetRotation() const;
	ShipType GetType() const;

	DamageResult DoDamage(const Position* position);

	bool IsDestroyed();

	bool CheckIntersection(const Ship* otherShip);
};

