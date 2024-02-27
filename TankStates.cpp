#include <iostream>
#include <vector>
#include <algorithm>

#include "lab_m1/Tema2/TankState.h"

TankState GetRandomTankState(TankState curr) {
	switch (curr) {
	case TankState::moveForward:
		case TankState::moveBackward:
			return (rand() % 2 == 0) ? TankState::rotateLeft : TankState::rotateRight;
			break;
	case TankState::rotateLeft:
		case TankState::rotateRight:
			return (rand() % 2 == 0) ? TankState::moveForward : TankState::moveBackward;
			break;
	}
}