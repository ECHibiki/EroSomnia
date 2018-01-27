#pragma once
#include <vector>
#include <SDL2/SDL.h>


class Animation
{
public:
	Animation();
	Animation(std::vector<SDL_Texture*> set);
	~Animation();

	SDL_Texture* itterateAnimation(int framerate);
	void destroyAnimation();
	void resetAnimation();
	bool checkDone();

	int counter;
	int gameFPS;

	bool done;

	int currentPosition;
	int previousPosition;
	int animationSize;
	std::vector<SDL_Texture*> frames;
};

