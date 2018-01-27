#include "Animation.h"
#include <iostream>
Animation::Animation() 
{

	animationSize = frames.size() - 1;
	currentPosition = 0;

	counter = 0;
	gameFPS = 60;

	done = false;
}

Animation::Animation(std::vector<SDL_Texture*> set)
{
	frames = set;
	animationSize = frames.size();
	currentPosition = 0;

	counter = 0;
	gameFPS = 60;

	done = false;
}


Animation::~Animation()
{
}


SDL_Texture* Animation::itterateAnimation(int framerate) {
	counter++;
	//std::cout << counter << " ";
	SDL_Texture* frame =  frames.at(currentPosition);
	if (counter > gameFPS / framerate) {
		previousPosition = currentPosition;
		currentPosition++;
		if (currentPosition > animationSize - 1) {
			done = true;
			currentPosition = 0;
		}
		counter = 0;
	}
	return frame;
}

void Animation::destroyAnimation() {
	int frameSize = frames.size();
	for (int i = 0; i < frameSize; i++) {
		SDL_DestroyTexture(frames.at(i));
	}
}

void Animation::resetAnimation() {
	currentPosition = 0;
}

bool Animation::checkDone(){
	if (done) {
		return true;
	}
	else {
		return false;
	}
}