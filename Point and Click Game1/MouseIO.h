#pragma once
#include<SDL2\SDL.h>
#include<vector>

#include "MouseIndexes.h"

class MouseIO
{
public:
	MouseIO();
	~MouseIO();

	enum mouseIndexes;

	std::vector<bool>* processMouseInput(int * x, int * y);
	void trackDestopMouse(int * x, int * y);
	void trackGameMouse(int * x, int * y);
	std::vector<bool> getInputArray();

	int calculateMouseDistance(int x, int y);
	int returnTotalDistance() { return distanceCounter; }
	void resetDistanceCounter();

private:
	int oldX, oldY;
	int distanceCounter;

	SDL_Event evnt;
	std::vector<bool> inputArray;
};

