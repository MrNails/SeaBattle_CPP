#pragma once
#include "Ship.h"

enum class FieldType
{
	Player,
	Enemy,
	Spectator
};

enum class ShotResult
{
	/// <summary>
	/// ���������
	/// </summary>
	Empty,

	/// <summary>
	/// ���� ������� ��� ������ �� ��������� ����
	/// </summary>
	OutOfField,

	/// <summary>
	/// ���� ������� ��� ������ � ������ ������
	/// </summary>
	Missed,

	/// <summary>
	/// ���� ������� ����� � ������ �������, � �������� ������ ����� ������
	/// </summary>
	Hitted,

	/// <summary>
	/// ���� ������� ��������� �������
	/// </summary>
	Destroyed,

	/// <summary>
	/// ���� ��������� ���� � ����� � ����������� �������
	/// </summary>
	AlreadyDestroyed,

	/// <summary>
	/// ���� ��������� ���� � �����, � ������� ����� ��������
	/// </summary>
	AlreadyHitted
};

class Field
{
private:
	Cell** _cells;
	Ship** _ships;
	int _width, _height, _currentShipAmount, _maxShipAmount;
	FieldType _type;

	void InitializaField();

	void FillNearestCellAs(Ship* ship, CellStatus status);
public:
	static const int DEFAULT_FIELD_SIZE = 10;
	static const int DEFAULT_SHIPS_AMOUNT = 10;

	Field();
	Field(const Field& field);
	Field(int width, int height, FieldType type);
	~Field();

	void SetFieldType(FieldType type);

	int GetWidth() const;
	int GetHeight() const;
	FieldType GetFieldType() const;

	Cell** GetField() const;

	bool TryAddShip(Ship* ship);

	bool AllShipsDestroyed();

	ShotResult ShootToCell(const Position* pos);
};

