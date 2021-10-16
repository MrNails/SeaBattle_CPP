#pragma once
#include <list>
#include "Field.h"

class AI
{
private:
	static const int DIRECTION_AMOUNT = 4;

	std::list<Cell*> _availablePositions;
	std::list<const Position*> _currentAttackableShip;
	bool _rotationBanArr[4];
	Field* _field;

	ShotResult _previousShotResult;

#if _DEBUG
	static int _globalId;
	int _id;
#endif // _DEBUG


	//Position* _previousPos;

	void ResetRotationBarArr();
	void ClearCurrentAttackableShip();
	void UpdateAvailablesPositions();
public:
	AI();

	void SetCurrentField(Field* field);
	bool DoMove();
};

