#include"KeyboardMouseIO.h"
#include <iostream>

#define NUMBER_OF_INPUTS 100
#define LCLICK inputArray.at(LEFTMOUSECLICKED) && !inputArray.at(LEFTMOUSEHELD)
#define LHELD  inputArray.at(LEFTMOUSEHELD)
#define RCLICK inputArray.at(RIGHTMOUSECLICKED) && !inputArray.at(RIGHTMOUSEHELD)
#define RHELD  inputArray.at(RIGHTMOUSEHELD)

KeyboardMouseIO::KeyboardMouseIO()
{
	inputArray.reserve(NUMBER_OF_INPUTS);
	for (int i = 0; i < NUMBER_OF_INPUTS; i++) {
		inputArray.push_back(false);
	}

	distanceCounter = 0;
	oldX = -1;
	oldY = -1;
}


KeyboardMouseIO::~KeyboardMouseIO()
{
}

//Returns a pointer to the origional array
std::vector<bool>* KeyboardMouseIO::processKBMInput(int * x, int * y) {
	//std::cout << inputArray[MOUSECLICKED];
	if (inputArray[MOUSECLICKED] == true) inputArray[MOUSEHELD] = true;	else inputArray[MOUSEHELD] = false;

	if (inputArray[LEFTMOUSECLICKED] == true) inputArray[LEFTMOUSEHELD] = true;	else inputArray[LEFTMOUSEHELD] = false;
	if (inputArray[RIGHTMOUSECLICKED] == true) inputArray[RIGHTMOUSEHELD] = true;	else inputArray[RIGHTMOUSEHELD] = false;
	if (inputArray[MOUSEWHEELUP] == true) inputArray[MOUSEWHEELUP] = false;
	if (inputArray[MOUSEWHEELDOWN] == true) inputArray[MOUSEWHEELDOWN] = false;

	if (inputArray[KEYPRESSED]) inputArray[KEYHELD] = true; else inputArray[KEYHELD] = false;

	if (inputArray[NUM1]) inputArray[NUM1_H] = true; else inputArray[NUM1_H] = false;
	if (inputArray[NUM2]) inputArray[NUM2_H] = true; else inputArray[NUM2_H] = false;
	if (inputArray[NUM3]) inputArray[NUM3_H] = true; else inputArray[NUM3_H] = false;
	if (inputArray[NUM4]) inputArray[NUM4_H] = true; else inputArray[NUM4_H] = false;
	if (inputArray[NUM5]) inputArray[NUM5_H] = true; else inputArray[NUM5_H] = false;
	if (inputArray[NUM6]) inputArray[NUM6_H] = true; else inputArray[NUM6_H] = false;
	if (inputArray[NUM7]) inputArray[NUM7_H] = true; else inputArray[NUM7_H] = false;
	if (inputArray[NUM8]) inputArray[NUM8_H] = true; else inputArray[NUM8_H] = false;
	if (inputArray[NUM9]) inputArray[NUM9_H] = true; else inputArray[NUM9_H] = false;

	if (inputArray[KEYQ]) inputArray[KEYQ_H] = true; else inputArray[KEYQ_H] = false;
	if (inputArray[KEYW]) inputArray[KEYW_H] = true; else inputArray[KEYW_H] =  false;
	if (inputArray[KEYE]) inputArray[KEYE_H] = true; else inputArray[KEYE_H] = false;
	if (inputArray[KEYR]) inputArray[KEYR_H] = true; else inputArray[KEYR_H] = false;
	if (inputArray[KEYT]) inputArray[KEYT_H] = true; else inputArray[KEYT_H] = false;
	if (inputArray[KEYY]) inputArray[KEYY_H] = true; else inputArray[KEYY_H] = false;

	if (inputArray[CTRL] == true) inputArray[CTRLHELD] = true;	else inputArray[CTRLHELD] = false;
	while (SDL_PollEvent(&evnt)) {
		switch (evnt.type) {
		case SDL_KEYDOWN:
			inputArray[KEYPRESSED] = true;

			switch (evnt.key.keysym.sym) {
				case SDLK_LCTRL:
					inputArray[CTRL] = true;
					break;
				case SDLK_1:
					inputArray[NUM1] = true;
					break;
				case SDLK_2:
					inputArray[NUM2] = true;
					break;
				case SDLK_3:
					inputArray[NUM3] = true;
					break;
				case SDLK_4: 
					inputArray[NUM4] = true;
					break;
				case SDLK_5:
					inputArray[NUM5] = true;
					break;
				case SDLK_6:
					inputArray[NUM6] = true;
					break;
				case SDLK_7:
					inputArray[NUM7] = true;
					break;
				case SDLK_8:
					inputArray[NUM8] = true;
					break;
				case SDLK_9: 
					inputArray[NUM9] = true;
					break;

				case SDLK_q:
					inputArray[KEYQ] = true;
					break;
				case SDLK_w:
					inputArray[KEYW] = true;
					break;
				case SDLK_e:
					inputArray[KEYE] = true;
					break;
				case SDLK_r:
					inputArray[KEYR] = true;
					break;
				case SDLK_t:
					inputArray[KEYT] = true;
					break;
				case SDLK_y:
					inputArray[KEYY] = true;
					break;
			}
			break;
		case SDL_KEYUP:
			inputArray[KEYPRESSED] = false;
			switch (evnt.key.keysym.sym) {

			case SDLK_LCTRL:
				inputArray[CTRL] = false;
				break;
			case SDLK_1:
				inputArray[NUM1] = false;
				break;
			case SDLK_2:
				inputArray[NUM2] = false;
				break;
			case SDLK_3:
				inputArray[NUM3] = false;
				break;
			case SDLK_4:
				inputArray[NUM4] = false;
				break;
			case SDLK_5:
				inputArray[NUM5] = false;
				break;
			case SDLK_6:
				inputArray[NUM6] = false;
				break;
			case SDLK_7:
				inputArray[NUM7] = false;
				break;
			case SDLK_8:
				inputArray[NUM8] = false;
				break;
			case SDLK_9:
				inputArray[NUM9] = false;
				break;

			case SDLK_q:
				inputArray[KEYQ] = false;
				break;
			case SDLK_w:
				inputArray[KEYW] = false;
				break;
			case SDLK_e:
				inputArray[KEYE] = false;
				break;
			case SDLK_r:
				inputArray[KEYR] = false;
				break;
			case SDLK_t:
				inputArray[KEYT] = false;
				break;
			case SDLK_y:
				inputArray[KEYY] = false;
				break;
			}
			break;

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
			else if (evnt.wheel.y < 0) {
				inputArray[MOUSEWHEELDOWN] = true;
			}
		case SDL_MOUSEBUTTONDOWN:
			inputArray[MOUSECLICKED] = true;
			if (evnt.button.button == SDL_BUTTON_LEFT)
				inputArray[LEFTMOUSECLICKED] = true;
			if (evnt.button.button == SDL_BUTTON_RIGHT)
				inputArray[RIGHTMOUSECLICKED] = true;
			break;
		case SDL_MOUSEBUTTONUP:
			if (evnt.button.button == SDL_BUTTON_RIGHT)
				inputArray[RIGHTMOUSECLICKED] = false;
			if (evnt.button.button == SDL_BUTTON_LEFT)
				inputArray[LEFTMOUSECLICKED] = false;
			if (!(inputArray[RIGHTMOUSECLICKED] || inputArray[LEFTMOUSECLICKED]))
				inputArray[MOUSECLICKED] = false;
			break;
		}
	}
	//std::cout << inputArray[LEFTMOUSECLICKED];
	//std::cout << inputArray[LEFTMOUSEHELD] << "\n";
	return &inputArray;
}

void KeyboardMouseIO::trackDestopMouse(int* x, int* y) {
	SDL_GetGlobalMouseState(x, y);
}

void KeyboardMouseIO::trackGameMouse(int* x, int* y) {
	SDL_GetMouseState(x, y);
}

int KeyboardMouseIO::calculateMouseDistance(int x, int y) {
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

int KeyboardMouseIO::calculateMouseDistance(int x, int y, double scale) {
	int distance = 0;
	if (oldX != -1) {
		distance = sqrt(
			(oldX - x) * (oldX - x) + (oldY - y) * (oldY - y)
		) * scale;
		distanceCounter += distance;
	}

	oldX = x;
	oldY = y;

	return distance;
}

void KeyboardMouseIO::resetDistanceCounter() {
	distanceCounter = 0; oldX = -1; oldY = -1;
}


//0 is exit pressed, 1 is held, 2 is clicked 
//Returns a copy of the origional array
std::vector<bool> KeyboardMouseIO::getInputArray() {
	return inputArray;
}
