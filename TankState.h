#pragma once

enum class TankState {
	moveForward,	// 0
	moveBackward,	// 1
	rotateLeft,		// 2
	rotateRight,	// 3
	rotateTurret,	// 4
};

TankState GetRandomTankState(TankState curr);