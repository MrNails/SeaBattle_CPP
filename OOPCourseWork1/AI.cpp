#include "AI.h"

#if _DEBUG
int AI::_globalId = 0;
#endif // _DEBUG



void AI::ResetRotationBarArr()
{
	for (size_t i = 0; i < DIRECTION_AMOUNT; i++)
		_rotationBanArr[i] = false;
}

void AI::ClearCurrentAttackableShip()
{
	for (auto itr = _currentAttackableShip.begin(); itr != _currentAttackableShip.end(); itr++)
		delete (*itr);

	_currentAttackableShip.clear();
}

void AI::UpdateAvailablesPositions()
{
	_availablePositions.remove_if([](const Cell* value) { return value->GetStatus() != CellStatus::Empty && value->GetStatus() != CellStatus::DeckAlive; });
}

AI::AI()
{
	_field = nullptr;
	_previousShotResult = ShotResult::Empty;

	ResetRotationBarArr();

#if _DEBUG
	_id = AI::_globalId++;
	allEntries = 0;
	gameEntries = 0;
#endif // _DEBUG

}

void AI::SetCurrentField(Field* field)
{
	_previousShotResult = ShotResult::Empty;

	_field = field;

	if (_currentAttackableShip.size() != 0)
		ClearCurrentAttackableShip();
	if (_availablePositions.size() != 0)
		_availablePositions.clear();

	for (size_t i = 0; i < field->GetWidth() * field->GetHeight(); i++)
		_availablePositions.push_back(field->GetField()[i]);
}

const char* GetShotResultAsString(ShotResult res)
{
	switch (res)
	{
	case ShotResult::Empty:
		return "Empty";
	case ShotResult::OutOfField:
		return "OutOfField";
	case ShotResult::Missed:
		return "Missed";
	case ShotResult::Hitted:
		return "Hitted";
	case ShotResult::Destroyed:
		return "Destroyed";
	case ShotResult::AlreadyDestroyed:
		return "AlreadyDestroyed";
	case ShotResult::AlreadyHitted:
		return "AlreadyHitted";
	default:
		return "Empty";
	}
}

ShotResult AI::DoMove()
{
	if (_availablePositions.empty())
		return ShotResult::Empty;

#if _DEBUG
	gameEntries++;
#endif // _DEBUG

	if (_previousShotResult == ShotResult::Empty || _previousShotResult == ShotResult::Destroyed ||
		((_previousShotResult == ShotResult::Missed || _previousShotResult == ShotResult::AlreadyHitted) && _currentAttackableShip.size() == 1))
	{
		ClearCurrentAttackableShip();
		UpdateAvailablesPositions();
		ResetRotationBarArr();

		int i = 0, randIndex = rand() % _availablePositions.size();
		auto itr = _availablePositions.begin();

		while (itr != _availablePositions.end() && i != randIndex)
		{
			itr++;
			i++;
		}

		auto randPos = (*itr);

#if _DEBUG
		std::cout << "Shot res = " << GetShotResultAsString(_previousShotResult) << ". Random shot: " << randPos->GetPosition()->x << ";" << randPos->GetPosition()->y << std::endl;
		allEntries++;
#endif // _DEBUG

		_previousShotResult = _field->ShootToCell(randPos->GetPosition());
		_currentAttackableShip.push_back(new Position(*randPos->GetPosition()));
	}
	else
	{
		int directionIndex = -1;
		Position* temp = nullptr;

		std::list<int> directionList;

		for (size_t i = 0; i < DIRECTION_AMOUNT; i++)
			if (!_rotationBanArr[i])
				directionList.push_back(i);

		auto lastPos = _currentAttackableShip.back();

		//≈сли в прошлый раз промахнулись, то убираем предыдущий выстрел из точек атакуемого корабл€
		if (_previousShotResult == ShotResult::Missed)
		{
			delete _currentAttackableShip.back();

			_currentAttackableShip.pop_back();
			lastPos = _currentAttackableShip.back();

			if (_currentAttackableShip.size() > 1 && directionList.size() == 1)
			{
				for (size_t i = 0; i < DIRECTION_AMOUNT; i++)
				{
					if (!_rotationBanArr[i])
					{
						_rotationBanArr[i] = true;

						//Ќаправлени€ расположены по два в друг за другом поэтому если текущее направление €вл€етс€ 0
						//(0 % 2 = 0), то можно прибавить 1. ≈сли же направление €вл€тс€ 1 (1 % 2 = 1), то можно отн€ть 1.
						//ƒл€ всех последующих направление работает така€ же схема (2 % 2 = 0) -> 1 и (3 % 2 = 1) -> -1.
						int calculatedIndex = i + (i % 2 == 0 ? 1 : -1);

						_rotationBanArr[calculatedIndex] = false;

						directionList.clear();
						directionList.push_back(calculatedIndex);

						lastPos = *_currentAttackableShip.begin();
						break;
					}
				}
			}
		}

		while (true)
		{
			auto itr = directionList.begin();

			if (directionList.size() > 1)
			{
				directionIndex = rand() % directionList.size();

				int i = 0;
				while (itr != directionList.end() && i != directionIndex)
				{
					itr++;
					i++;
				}
			}

			switch (*itr)
			{
			case 0:
				if (lastPos->x - 1 < 0)
					directionList.remove(0);
				else
					temp = new Position(lastPos->x - 1, lastPos->y);

				break;
			case 1:
				if (lastPos->x + 1 >= _field->GetWidth())
					directionList.remove(1);
				else
					temp = new Position(lastPos->x + 1, lastPos->y);
				break;
			case 2:
				if (lastPos->y - 1 < 0)
					directionList.remove(2);
				else
					temp = new Position(lastPos->x, lastPos->y - 1);
				break;
			case 3:
				if (lastPos->y + 1 >= _field->GetHeight())
					directionList.remove(3);
				else
					temp = new Position(lastPos->x, lastPos->y + 1);
				break;
			}

			if (temp == nullptr)
				continue;

			_previousShotResult = _field->ShootToCell(temp);

#if _DEBUG
			std::cout << "Shot res = " << GetShotResultAsString(_previousShotResult) << ". Shot to specified ship: " << temp->x << ";" << temp->y << std::endl;
			allEntries++;
#endif // _DEBUG

			if (_previousShotResult == ShotResult::AlreadyHitted)
			{
				delete temp;
				temp = nullptr;

				int rotation = *itr;

				if (directionList.size() == 1)
				{
					int evaluatedIndex = rotation + (rotation % 2 == 0 ? 1 : -1);
					_rotationBanArr[evaluatedIndex] = false;
					directionList.push_back(evaluatedIndex);
				}

				_rotationBanArr[rotation] = true;

				directionList.remove(rotation);

				continue;
			}

			_currentAttackableShip.push_back(temp);

			if (_previousShotResult == ShotResult::Missed && directionList.size() > 1)
				_rotationBanArr[*itr] = true;
			else if (_previousShotResult == ShotResult::Hitted)
			{
				for (size_t i = 0; i < DIRECTION_AMOUNT; i++)
					_rotationBanArr[i] = true;

				_rotationBanArr[*itr] = false;
			}

			break;
		}
	}

	return _previousShotResult;
}
