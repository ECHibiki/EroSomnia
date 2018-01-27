#pragma once
#include <chrono>
#include <SDL2\SDL.h>
#include <string>

#define SYSTEM_MILLISECONDS std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
#define DATATYPE_MILLISECONDS std::chrono::milliseconds
class GenericGameTools
{
public:
	GenericGameTools();
	~GenericGameTools();

	static void stallGame(long long *initial, long long *expired, int pauseTime);
	//NOTE THAT FRAMERATE CAN BE LIMITED TO SCREEN REFRESH RATE IF  SDL_CreateRenderer(mpWindow.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); IS USED
	static bool checkRectHitBox(SDL_Rect rect, int x, int y);
	static bool checkCircleHitBox(int centerPointX, int centerPointY, int radius, int x, int y);
	static bool	checkOvalHitBox(int centerPointX, int centerPointY, int aFactor, int bFactor, int x, int y);
	static std::string createPointString(int points, int leadingZeros);
	static int decrementAtRate(int value, int duration);
	static int incrementAtRate(int value, int max, int duration);
	static void reset();
private:
	static long long internalTime;
	static int currentValue;
	static bool timeSet;
};

GenericGameTools::GenericGameTools()
{
}


GenericGameTools::~GenericGameTools()
{
}

long long GenericGameTools::internalTime = 0;
int GenericGameTools::currentValue = 0;
bool GenericGameTools::timeSet = false;

void GenericGameTools::reset() {
	internalTime = 0;
	currentValue = 0;
	timeSet = false;
}

int GenericGameTools::decrementAtRate(int value, int duration) {
	if (!timeSet) {
		internalTime = SDL_GetTicks();
		currentValue = value;
		timeSet = true;
	}
	float rate = value / (((float)duration / 1000) * 30);
	currentValue -= rate;
	if (internalTime - SDL_GetTicks() > duration || currentValue <= 0) {
		return 0;
	}
	return currentValue;
}

int GenericGameTools::incrementAtRate(int value, int max, int duration) {
	if (!timeSet) {
		internalTime = SDL_GetTicks();
		currentValue = value;
		timeSet = true;
	}
	float rate = max / (((float)duration / 1000) * 30);
	currentValue += rate;
	if (internalTime - SDL_GetTicks() > duration || currentValue >= max) {
		currentValue = max;
		return max;
	}

	return currentValue;
}


void GenericGameTools::stallGame(long long* initial, long long* expired, int pauseTime) {
	DATATYPE_MILLISECONDS ms = SYSTEM_MILLISECONDS;

	while (*expired - *initial < pauseTime) {
		ms = SYSTEM_MILLISECONDS;
		*expired = ms.count();
	}
}

bool GenericGameTools::checkRectHitBox(SDL_Rect rect, int x, int y) {
	//std::cout << x << " " << y << " "<< rect.x << " " << rect.y << " XY\n";
//	std::cout << (rect.x < x && (rect.x + rect.w) > x && rect.y < y && (rect.y + rect.h) > y) << "BOOl" ;
	return (rect.x < x && (rect.x + rect.w) > x && rect.y < y && (rect.y + rect.h) > y);
}

bool GenericGameTools::checkCircleHitBox(int centerPointX, int centerPointY, int radius, int x, int y) {
	int x2 = x - centerPointX;
	int y2 = y - centerPointY;
	return (radius * radius) >= ((x2) * (x2)+(y2) * (y2));
}

bool GenericGameTools::checkOvalHitBox(int centerPointX, int centerPointY, int aFactor, int bFactor, int x, int y) {
	int x2 = x - centerPointX;
	int y2 = y - centerPointY;
	return 1.0 >= ((float)(x2 * x2) / (aFactor * aFactor)) + ((float)(y2 * y2) / (bFactor * bFactor));
}

std::string GenericGameTools::createPointString(int points, int leadingZeros) {
	std::string pointsString = "";
	int divisor = 1000;

	for (int i = 0; i < leadingZeros; i++) {
		if (!(points / (divisor / ((int)pow(10, i))))){
			pointsString += "0";
		}
		else {
			pointsString += std::to_string(points);
			break;
		}
	}
	return pointsString;
}