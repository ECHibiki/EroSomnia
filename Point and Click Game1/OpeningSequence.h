#pragma once

#include <vector>

#include "ScreenIncludes.h"


namespace OpeningSequence
{
	void OpeningSequence(int* screen, SDL_Renderer* r1, SDL_Window* win);

	void gameLoop();

	void processInput();
	void update();
	void play();
	void render();
	void close();

	bool endTransitionFlag;
	bool langClicked;
	bool logo1Flag;
	bool logo2Flag;
	bool screenRunning;
	bool skippedFlag;


	const int FPS = 30;
	
	double fadeRate;
	DATATYPE_MILLISECONDS systemTime;

	int alpha;
	int x;
	int y;
	int* screen;

	long long initial = 0;
	long long expired = 0;
	long long start = 0;
	long long timer = 0;


	MouseIO* mio;

	SDL_Renderer* r1;
	SDL_Window* w1;

	Texture* engSymbol;
	Texture* jpSymbol;
	Texture* logoEC;

	Texture* logo2;
	Texture* logo3;

	std::vector<bool>* inputs;

	void OpeningSequence::OpeningSequence(int* screen, SDL_Renderer* rend, SDL_Window* win)
	{
		OpeningSequence::screen = screen;
		r1 = rend;
		w1 = win;

		alpha = 0;

		mio = new MouseIO();

		engSymbol = new Texture(SDL_Rect{0,0,Graphics::prcnt(0.1, 'x', w1),Graphics::prcnt(0.1, 'y', w1) }, "Icons/EN.png", r1, w1 );
		jpSymbol = new Texture(SDL_Rect{ 500,500,Graphics::prcnt(0.1, 'x', w1),Graphics::prcnt(0.1, 'y', w1) }, "Icons/JP.png", r1, w1);
		gameLoop();
	}


	void OpeningSequence::gameLoop() {
		screenRunning = true;
		systemTime = SYSTEM_MILLISECONDS;
		initial = 0;
		expired = 0;

		while (screenRunning) {
			systemTime = SYSTEM_MILLISECONDS;
			initial = systemTime.count();

			processInput();
			update();
			play();
			render();

			GenericGameTools::stallGame(&initial, &expired, (int)(1.0 / FPS * 1000));
		}

		close();
	}

	void OpeningSequence::processInput() {
		int x, y;
		inputs = mio->processMouseInput(&x, &y);

		screenRunning = !inputs->at(QUIT);
	
	}

	void OpeningSequence::update() {
		if (langClicked) {
			systemTime = SYSTEM_MILLISECONDS;
			timer = systemTime.count();
			int elapsed = timer - start;
		}
		else if (timer < 123 && timer > -1 ) {
			logo1Flag = true;
			alpha = 0;
			//check skip
		}
		else if (timer < 456 && timer > -1) {
			logo2Flag = true;
			alpha = 0;
			//check skip
		}
		else if (timer < 789 && timer > -1) {
			endTransitionFlag = true;
			alpha = 0;
			//if transition reaches a point end the opening screen
			//check skip	
		}
		else if (timer < 1123 && timer > -1) {
			screenRunning = false;
		}
		else if(langClicked){
			//load respective text
			alpha = 0;
			start = timer;
		}

		if (skippedFlag) {
			//set end transition flags
			//set timer to a certain pointer
		}


	}
	void OpeningSequence::play() {
		
	}
	void OpeningSequence::render() {
		SDL_RenderClear(r1);
	
		//if transition flag
			//if alpha is not at max
				//fade in
			//destroy logo2
		//if logo2 flag
			//if alpha is not at max
				//fade in
			//destroy logo1
		//if logo1 flag
			//if alpha is not at max
				//fade in
			//destroy symbols
		//else
			//if alpha is not at max
				//fade in
			engSymbol->renderTexture();
			jpSymbol->renderTexture();

		SDL_RenderPresent(r1);
	}
	void OpeningSequence::close() {
		*screen = 10;
		delete engSymbol;
		delete jpSymbol;
	}

};
