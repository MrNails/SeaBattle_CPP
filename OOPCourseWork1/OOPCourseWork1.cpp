#include <iostream>
#include <math.h>
#include <Windows.h>

#include "Ship.h"
#include "Field.h"
#include "AI.h"

#undef max
#undef min

const int FIELD_SIZE = 10,
SHIP_AMOUNT = 10,
FIELD_X_OFFSET = 10,			//Смещение 2 (и последующих) поля(ей) по Х для корректной отрисовки
FIELD_BORDER_OFFSET = 2,		//Смещение для отрисовки границ поля
FIELD_MARKUP_OFFSET = 1;		//Смещение для разметки поля (0, 1, 2, 3...; a, b, c...)

const float _1_DECK_SHIP_CHANCE = 0.4,
_2_DECK_SHIP_CHANCE = 0.3,
_3_DECK_SHIP_CHANCE = 0.2,
_4_DECK_SHIP_CHANCE = 0.1;

const char FIELD_SIDE_BORDER_SMBL = '|',
FIELD_TOP_BOTTOM_BORDER_SMBL = '-',
DECK_ALIVE_SMBL = '@',
DECK_DESTROYED_SMBL = 'X',
EMPTY_FIELD_SMBL = ' ',
MISSED_SHOT_SMBL = '.';

HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

#pragma region Common
// Функция для проверки и очистки ввода 
void HandleInput(const std::istream& stream, bool& hasError);

// Функция для проверки и обработки ввода
template<typename T>
void InputWrapper(std::istream& stream, const char* userText,
	const char* userErrorText, T& variable, bool (*checkFunc)(const T&) = nullptr);

// Функция для проверки и обработки ввода строк
void InputWrapper(std::istream& stream, const char* userText,
	const char* userErrorText, char* variable, size_t inputSize, bool (*checkFunc)(const char*) = nullptr);
#pragma endregion

void ClearConsole(HANDLE consoleHandle, COORD offset);
void SetCursorVisibility(bool visibility, HANDLE consoleHandle);

void DrawField(const Position* offset, const Field& field);
void FillField(Field& field);

/// <summary>
/// Возвращает символ для отрисовки клетки поля.
/// <para>
/// Для поля противника скрывает клетки с кораблями, а для поля игрока скрывает клетки с попаданиями противника.
/// </para>
/// </summary>
/// <param name="status">Статус клетки</param>
/// <param name="type">Тип поля</param>
/// <returns>Символ для отрисовки клетки</returns>
char GetCellSkin(CellStatus status, FieldType type);

Position* ParsePosFromText(char* coord);

void PvAIMode();
void AIvAIMode();

int main()
{
	srand(time(0));

	int gameMode = -1;

	auto checkGameMode = [](const int& arg) { return arg > 0 && arg < 3; };

	InputWrapper<int>(std::cin, "Availables game mode:\n\t1 - Player vs AI\n\t2 - AI vs AI\nSelect game mode: ", "Error game mode selection.\n", gameMode, checkGameMode);

	SetCursorVisibility(false, consoleHandle);

	if (gameMode == 1)
		PvAIMode();
	else if (gameMode == 2)
		AIvAIMode();

	system("pause");

	return 0;
}

#pragma region Common
void HandleInput(const std::istream& stream, bool& hasError)
{
	hasError = std::cin.fail();
	if (hasError)
		std::cin.clear();

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

template<typename T>
void InputWrapper(std::istream& stream, const char* userText,
	const char* userErrorText, T& variable, bool (*checkFunc)(const T&))
{
	bool inputError;

	while (true)
	{
		std::cout << userText;
		(stream) >> variable;

		HandleInput(stream, inputError);

		inputError = inputError || (checkFunc && !checkFunc(variable));

		if (inputError)
			std::cout << userErrorText;
		else
			break;
	}
}

void InputWrapper(std::istream& stream, const char* userText,
	const char* userErrorText, char* variable, size_t inputSize, bool (*checkFunc)(const char*))
{
	char* tempArr = new char[inputSize];

	while (true)
	{
		std::cout << userText;
		stream.read(tempArr, inputSize);

		stream.ignore(stream.rdbuf()->in_avail());

		if ((checkFunc && !checkFunc(tempArr)))
			std::cout << userErrorText;
		else
			break;
	}

	for (size_t i = 0; i < inputSize; i++)
		variable[i] = tempArr[i];

	delete[] tempArr;
}
#pragma endregion

void ClearConsole(HANDLE consoleHandle, COORD offset)
{
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(consoleHandle, &screen);
	FillConsoleOutputCharacterA(
		consoleHandle, ' ', screen.dwSize.X * screen.dwSize.Y, offset, &written
	);
	SetConsoleCursorPosition(consoleHandle, offset);
}

Position* ParsePosFromText(char* coord)
{
	char posX = tolower(coord[1]), posY = coord[0];

	return new Position(posX - '0', posY - 'a');
}

void SetCursorVisibility(bool visibility, HANDLE consoleHandle)
{
	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(consoleHandle, &cursorInfo);
	cursorInfo.bVisible = visibility;
	SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

void DrawUpBottomBorder(int size)
{
	for (size_t i = 0; i < size; i++)
		std::cout << FIELD_TOP_BOTTOM_BORDER_SMBL;
}

void DrawDigits(int size)
{
	for (size_t i = 0; i < size; i++)
		std::cout << i;
}

char GetCellSkin(CellStatus status, FieldType type)
{
	switch (status)
	{
	case CellStatus::Empty:
		return EMPTY_FIELD_SMBL;
	case CellStatus::DeckAlive:
		if (type == FieldType::Enemy)
			return EMPTY_FIELD_SMBL;
		else
			return DECK_ALIVE_SMBL;
	case CellStatus::DeckDestroyed:
		return DECK_DESTROYED_SMBL;
	case CellStatus::MissedShot:
		//if (type == FieldType::Player)
		//	return EMPTY_FIELD_SMBL;
		//else
			return MISSED_SHOT_SMBL;
	default:
		return '\0';
	}
}

void DrawField(const Position* offset, const Field& field)
{
	//+ 3 нужно, чтобы отрисовать границы и координаты поля
	int fieldHeight = field.GetHeight() + FIELD_BORDER_OFFSET + FIELD_MARKUP_OFFSET;
	int fieldWidth = field.GetWidth() + FIELD_BORDER_OFFSET + FIELD_MARKUP_OFFSET;
	auto fieldBody = field.GetField();

	int currentOffsetY = 0;

	char sideCoords[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j' };

	for (size_t i = 0; i < fieldHeight; i++)
	{
		COORD coord = { offset->x, offset->y + currentOffsetY };

		SetConsoleCursorPosition(consoleHandle, coord);

		if (i == 0)
		{
			std::cout << "  ";
			DrawDigits(fieldWidth - 3);
		}
		else if (i == 1 || i == fieldHeight - 1)
		{
			std::cout << " ";
			DrawUpBottomBorder(fieldWidth - 1);
		}
		else
		{
			for (size_t j = 0; j < fieldWidth; j++)
			{
				if (j == 0)
					std::cout << sideCoords[i - 2];
				else if (j == 1 || j == fieldWidth - 1)
					std::cout << FIELD_SIDE_BORDER_SMBL;
				else
					std::cout << GetCellSkin(fieldBody[field.GetWidth() * (i - 2) + (j - 2)]->GetStatus(), field.GetFieldType());
			}
		}

		std::cout << std::endl;

		currentOffsetY++;
	}

	COORD coord = { offset->x, offset->y + currentOffsetY };

	SetConsoleCursorPosition(consoleHandle, coord);
	if (field.GetFieldType() == FieldType::Player)
	{
		std::cout << "You\n";
	}
	else
		std::cout << "Player " << field.GetId() << std::endl;
}

void FillField(Field& field)
{
	float shipType = -1;

	float shipChanceArr[ShipType::_4DeckShip] =
	{
		_1_DECK_SHIP_CHANCE,
		_1_DECK_SHIP_CHANCE + _2_DECK_SHIP_CHANCE,
		_1_DECK_SHIP_CHANCE + _2_DECK_SHIP_CHANCE + _3_DECK_SHIP_CHANCE,
		_1_DECK_SHIP_CHANCE + _2_DECK_SHIP_CHANCE + _3_DECK_SHIP_CHANCE + _4_DECK_SHIP_CHANCE
	};

	int maxShipAmountArr[ShipType::_4DeckShip] = {}, currentShipAmountArr[ShipType::_4DeckShip] = {};

	maxShipAmountArr[ShipType::_1DeckShip - 1] = SHIP_AMOUNT * _1_DECK_SHIP_CHANCE;
	maxShipAmountArr[ShipType::_2DeckShip - 1] = SHIP_AMOUNT * _2_DECK_SHIP_CHANCE;
	maxShipAmountArr[ShipType::_3DeckShip - 1] = SHIP_AMOUNT * _3_DECK_SHIP_CHANCE;
	maxShipAmountArr[ShipType::_4DeckShip - 1] = SHIP_AMOUNT * _4_DECK_SHIP_CHANCE;

	int posX = -1, posY = -1, currentAddedShipAmount = 0;
	bool typeIsAssigned = false;

	ShipType sType;
	ShipRotation rotation;
	Ship* ship = nullptr;

	while (currentAddedShipAmount != SHIP_AMOUNT)
	{
		shipType = (rand() % 10 + 1) / 10.0;

		for (size_t i = 0; i < ShipType::_4DeckShip; i++)
		{
			if (shipType <= shipChanceArr[i] && currentShipAmountArr[i] != maxShipAmountArr[i])
			{
				sType = (ShipType)(i + 1);
				currentShipAmountArr[i]++;

				typeIsAssigned = true;
				break;
			}
		}

		if (!typeIsAssigned)
			continue;

		typeIsAssigned = false;

#if _DEBUG
		std::cout << "Current ship number: " << currentAddedShipAmount + 1 << ". Ship type: " << shipType << std::endl;
#endif // _DEBUG

		do
		{
			switch (rand() % 4)
			{
			case 1:
				rotation = ShipRotation::Down;
				break;
			case 2:
				rotation = ShipRotation::Right;
				break;
			case 3:
				rotation = ShipRotation::Left;
				break;
			case 0:
			default:
				rotation = ShipRotation::Up;
				break;
			}

			posX = rand() % FIELD_SIZE;
			posY = rand() % FIELD_SIZE;

			if (ship == nullptr)
				ship = new Ship(sType);

			ship->SetRotation(rotation);
			ship->SetNewHeadPosition(new Position(posX, posY));

		} while (!field.TryAddShip(ship));

		currentAddedShipAmount++;

		ship = nullptr;
	}
}

void AIvAIMode()
{
	auto offsetPlayer = new Position(0, 0),
		offsetEnemy = new Position(FIELD_SIZE + FIELD_X_OFFSET, 0);

	Field ai1Field(FIELD_SIZE, FIELD_SIZE, FieldType::Spectator);
	Field ai2Field(FIELD_SIZE, FIELD_SIZE, FieldType::Spectator);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	AI ai1, ai2;

	int movePossibility = 0;

	std::cout << "Creating fields...\n";

	FillField(ai1Field);
	FillField(ai2Field);

	system("cls");

	bool exit = false;

	ai1.SetCurrentField(&ai2Field);
	ai2.SetCurrentField(&ai1Field);

	while (!exit)
	{
		GetConsoleScreenBufferInfo(consoleHandle, &screen);

		SetConsoleCursorPosition(consoleHandle, { 0 });
		DrawField(offsetPlayer, ai1Field);
		DrawField(offsetEnemy, ai2Field);

		if (screen.dwCursorPosition.Y != 0)
			SetConsoleCursorPosition(consoleHandle, screen.dwCursorPosition);

		if (ai1Field.AllShipsDestroyed())
		{
			std::cout << "Player 1 won this game!\n";
			break;
		}
		else if (ai2Field.AllShipsDestroyed())
		{
			std::cout << "Player 2 won this game!\n";
			break;
		}

		ClearConsole(consoleHandle, { 0, FIELD_SIZE + 5 });

		if (movePossibility == 0 || movePossibility == 1) 
		{
			std::cout << "Player 1 move.\n";
			Sleep(500);
			auto ai1MoveResult = ai1.DoMove();

			if (ai1MoveResult == ShotResult::Hitted || ai1MoveResult == ShotResult::Destroyed ||
				ai1MoveResult == ShotResult::AlreadyDestroyed || ai1MoveResult == ShotResult::AlreadyHitted)
				movePossibility = 1;
			else if (ai1MoveResult == ShotResult::Missed)
				movePossibility = 2;
		}
		else
		{
			std::cout << "Player 2 move.\n";
			Sleep(500);
			auto ai2MoveResult = ai2.DoMove();

			if (ai2MoveResult == ShotResult::Hitted || ai2MoveResult == ShotResult::Destroyed ||
				ai2MoveResult == ShotResult::AlreadyDestroyed || ai2MoveResult == ShotResult::AlreadyHitted)
				movePossibility = 2;
			else
				movePossibility = 0;
		}
	}
}

void PvAIMode()
{
	auto offsetPlayer = new Position(0, 0),
		offsetEnemy = new Position(FIELD_SIZE + FIELD_X_OFFSET, 0);

	Field player(FIELD_SIZE, FIELD_SIZE, FieldType::Player);
	Field enemy(FIELD_SIZE, FIELD_SIZE, FieldType::Enemy);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	AI ai1;
	
	int movePossibility = 0;
	bool shipDestroyed = false;

	std::cout << "Creating fields...\n";

	FillField(player);
	FillField(enemy);

	system("cls");

	char* currPos = new char[] { -1, -1, '\0' };
	bool exit = false;

	auto checkPos = [](const char* pos)
	{
		return (pos[0] <= 'j' && pos[0] >= 'a') && (pos[1] <= '9' && pos[1] >= '0');
	};

	ai1.SetCurrentField(&player);

	while (!exit)
	{

		CONSOLE_SCREEN_BUFFER_INFO screen;;

		GetConsoleScreenBufferInfo(consoleHandle, &screen);

		SetConsoleCursorPosition(consoleHandle, { 0 });
		DrawField(offsetPlayer, player);
		DrawField(offsetEnemy, enemy);

		if (screen.dwCursorPosition.Y != 0)
			SetConsoleCursorPosition(consoleHandle, screen.dwCursorPosition);

		if (player.AllShipsDestroyed())
		{
			enemy.SetFieldType(FieldType::Player);

			SetConsoleCursorPosition(consoleHandle, { 0 });
			DrawField(offsetEnemy, enemy);

			std::cout << "You lose!\n";
			break;
		}
		else if (enemy.AllShipsDestroyed())
		{
			std::cout << "You win!\n";
			break;
		}

		GetConsoleScreenBufferInfo(consoleHandle, &screen);

		SetConsoleCursorPosition(consoleHandle, { 0 });
		DrawField(offsetPlayer, player);
		DrawField(offsetEnemy, enemy);

		if (screen.dwCursorPosition.Y != 0)
		{
			SetConsoleCursorPosition(consoleHandle, screen.dwCursorPosition);
		}

		ClearConsole(consoleHandle, { 0, FIELD_SIZE + 5 });

		if (movePossibility == 0 || movePossibility == 1)
		{
			if (shipDestroyed)
				std::cout << "You destroyed enemy ship!\n";

			std::cout << "You move\n";
			InputWrapper(std::cin, "Enter shoot coord: ", "Error input position\n", currPos, 2, checkPos);

			auto shootRes = enemy.ShootToCell(ParsePosFromText(currPos));

			while (shootRes == ShotResult::AlreadyDestroyed || shootRes == ShotResult::AlreadyHitted)
			{
				std::cout << "Your already hitted to this coord. Choose other cell.\n";
				InputWrapper(std::cin, "Enter shoot coord: ", "Error input position\n", currPos, 2, checkPos);

				shootRes = enemy.ShootToCell(ParsePosFromText(currPos));
			}

			if (shootRes == ShotResult::Destroyed)
				shipDestroyed = true;
			else
				shipDestroyed = false;

			if (shootRes == ShotResult::Missed)
				movePossibility = 2;
			else
				movePossibility = 1;
		}
		else
		{
			std::cout << "Player 2 move.\n";
			Sleep(700);
			auto ai1MoveResult = ai1.DoMove();

			if (ai1MoveResult == ShotResult::Hitted || ai1MoveResult == ShotResult::Destroyed ||
				ai1MoveResult == ShotResult::AlreadyDestroyed || ai1MoveResult == ShotResult::AlreadyHitted)
				movePossibility = 2;
			else
				movePossibility = 0;
		}
	}
}