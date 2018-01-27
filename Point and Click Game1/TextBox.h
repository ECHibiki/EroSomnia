#pragma once

#include <string>
#include <vector>
#include <SDL2\SDL.h>

class Words;

class TextBox
{
public:
	TextBox(int fontSize, int fontType, std::string line, SDL_Rect textBox, SDL_Renderer* r1, SDL_Window* w1);
	TextBox(int fontSize, int fontType, std::string line, SDL_Rect textBox, SDL_Rect marginBox, SDL_Renderer* r1, SDL_Window* w1);
	TextBox(int fontSize, int fontType, std::string line, SDL_Color col, SDL_Rect textBox, SDL_Rect marginBox, SDL_Renderer* r1, SDL_Window* w1);
	~TextBox();

	int fontSize;
	int fontType;

	std::vector<Words*> textBoxWords;
	std::string textBoxLine;
	SDL_Rect textBoxBox;
	SDL_Rect marginBoxBox;
	SDL_Renderer* r1; SDL_Window* w1;
	SDL_Color color = {0,0,0,255};

	void renderTypeInText();
	void renderBoxWords(SDL_Color fill, SDL_Color border);
	void renderBoxWords(SDL_Color fillStart, SDL_Color fillEnd, double pixelSize, int angle, double growth, double transitionRate, SDL_Color border);

	void createBoxWords(std::string line);
	void changeBoxWords(std::string line);
};

