#include "MouseIO.h"
#include <iostream>

#define NUMBER_OF_INPUTS 33
#define LCLICK inputArray.at(LEFTMOUSECLICKED) && !inputArray.at(LEFTMOUSEHELD)
#define LHELD  inputArray.at(LEFTMOUSEHELD)
#define RCLICK inputArray.at(RIGHTMOUSECLICKED) && !inputArray.at(RIGHTMOUSEHELD)
#define RHELD  inputArray.at(RIGHTMOUSEHELD)

MouseIO::MouseIO()
{
	inputArray.reserve(NUMBER_OF_INPUTS);
	for (int i = 0; i < NUMBER_OF_INPUTS; i++) {
		inputArray.push_back(false);
	}

	distanceCounter = 0;
	oldX = -1;
	oldY = -1;
}


MouseIO::~MouseIO()
{
}

//Returns a pointer to the origional array
std::vector<bool>* MouseIO::processMouseInput(int * x, int * y) {
	//std::cout << inputArray[MOUSECLICKED];
	if (inputArray[MOUSECLICKED] == true) inputArray[MOUSEHELD] = true;
	else inputArray[MOUSEHELD] = false;
	if (inputArray[LEFTMOUSECLICKED] == true) inputArray[LEFTMOUSEHELD] = true;
	else inputArray[LEFTMOUSEHELD] = false;
	if (inputArray[RIGHTMOUSECLICKED] == true) inputArray[RIGHTMOUSEHELD] = true;
	else inputArray[RIGHTMOUSEHELD] = false;
	//if (inputArray[MIDDLEMOUSECLICKED] == true) inputArray[MIDDLEMOUSEHELD] = true;
	//else
	inputArray[MIDDLEMOUSEHELD] = false; 	inputArray[MIDDLEMOUSECLICKED] = false;
	if (inputArray[MOUSEWHEELUP] == true) inputArray[MOUSEWHEELUP] = false;
	if (inputArray[MOUSEWHEELDOWN] == true) inputArray[MOUSEWHEELDOWN] = false;
	while (SDL_PollEvent(&evnt)) {
		switch (evnt.type) {
		case SDL_QUIT:
			inputArray[QUIT] = true;
			break;
		case SDL_MOUSEMOTION:
			SDL_GetMouseState(x, y);
			break;
		case SDL_MOUSEWHEEL:
			if (evnt.wheel.y > 0) {
				inputArray[MOUSEWHEELUP] = true;
			}
			else if(evnt.wheel.y < 0) {
				inputArray[MOUSEWHEELDOWN] = true;
			}
		case SDL_MOUSEBUTTONDOWN:
			inputArray[MOUSECLICKED] = true;
			if (evnt.button.button == SDL_BUTTON_LEFT)
				inputArray[LEFTMOUSECLICKED] = true;
			if (evnt.button.button == SDL_BUTTON_RIGHT) 
				inputArray[RIGHTMOUSECLICKED] = true;
			if(evnt.button.button = SDL_BUTTON_MIDDLE)
				inputArray[MIDDLEMOUSECLICKED] = true;
			break;
		case SDL_MOUSEBUTTONUP:
			if (evnt.button.button == SDL_BUTTON_RIGHT)
				inputArray[RIGHTMOUSECLICKED] = false;
			if (evnt.button.button == SDL_BUTTON_LEFT)
				inputArray[LEFTMOUSECLICKED] = false;
			if(!(inputArray[RIGHTMOUSECLICKED] || inputArray[LEFTMOUSECLICKED]))
				inputArray[MOUSECLICKED] = false;
			if (!(evnt.button.button = SDL_BUTTON_MIDDLE))
				inputArray[MIDDLEMOUSECLICKED] = false;
			break;
		}
	}
	return &inputArray;
}

void MouseIO::trackDestopMouse(int* x, int* y) {
	SDL_GetGlobalMouseState(x, y);
}

void MouseIO::trackGameMouse(int* x, int* y) {
	SDL_GetMouseState(x, y);
}

int MouseIO::calculateMouseDistance(int x, int y) {
	int distance = 0;
	if (oldX != -1) {
		distance = sqrt(
			(oldX - x) * (oldX - x) + (oldY - y) * (oldY - y)
		);
		distanceCounter += distance;
	}

	oldX = x;
	oldY = y;

	return distance;
}

void MouseIO::resetDistanceCounter() {
	distanceCounter = 0; oldX = -1; oldY = -1;
}


//0 is exit pressed, 1 is held, 2 is clicked 
//Returns a copy of the origional array
std::vector<bool> MouseIO::getInputArray() {
	return inputArray;
}
