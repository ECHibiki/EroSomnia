#pragma once
#include <vector>

class MouseIO;

class Screen
{
public:
	Screen();
	~Screen();

	void gameLoop();

	void processInput();
	void update();
	void play();
	void render();
	void close();

	bool fadeIn(double durration);
	bool fadeOut(double durration);
	void fadeOutMusic(int fadeOutDuration);

	int x;
	int y;
	MouseIO* mio;
	std::vector<bool>* inputs;

	int alpha;
	double fadeRate;
	double FPSDelay;

};



Screen::Screen()
{
	/* in the header
	int x;
	int y;
	MouseIO* mio;
	std::vector<bool>* inputs;

	int alpha;
	double fadeRate;
	double FPSDelay;
	*/
}


Screen::~Screen()
{
}

void Screen::gameLoop() {

}

void Screen::processInput() {

}

void Screen::update() {

}
void Screen::play() {

}
void Screen::render() {

}
void Screen::close() {

}
