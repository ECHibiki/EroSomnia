#pragma once
#include<SDL2\SDL.h>
#include<vector>

#include "KBIndexes.h"
#include "MouseIndexes.h"

class KeyboardMouseIO
{
public:
	KeyboardMouseIO();
	~KeyboardMouseIO();

	std::vector<bool>* processKBMInput(int * x, int * y);
	void trackDestopMouse(int * x, int * y);
	void trackGameMouse(int * x, int * y);
	std::vector<bool> getInputArray();

	int calculateMouseDistance(int x, int y);
	int calculateMouseDistance(int x, int y, double scale);
	int returnTotalDistance() { return distanceCounter; }
	void resetDistanceCounter();

private:
	int oldX, oldY;
	int distanceCounter;

	SDL_Event evnt;
	std::vector<bool> inputArray;
};

