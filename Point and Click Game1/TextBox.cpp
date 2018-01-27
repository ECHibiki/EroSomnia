#include "TextBox.h"

#include "Graphics.h"

TextBox::TextBox(int fontsize, int fonttype, std::string line, SDL_Rect textBox, SDL_Renderer* r, SDL_Window* w)
{
	fontSize = fontsize; fontType = fonttype; textBoxLine = line; textBoxBox = textBox; r1 = r, w1 = w;
	marginBoxBox = textBox;
	createBoxWords(line);
}

TextBox::TextBox(int fontsize, int fonttype, std::string line, SDL_Rect textBox, SDL_Rect marginBox, SDL_Renderer* r, SDL_Window* w)
{
	fontSize = fontsize; fontType = fonttype; textBoxLine = line; textBoxBox = textBox; r1 = r, w1 = w;
	marginBoxBox = marginBox;
	createBoxWords(line);
}


TextBox::TextBox(int fontsize, int fonttype, std::string line, SDL_Color col, SDL_Rect textBox, SDL_Rect marginBox, SDL_Renderer* r, SDL_Window* w)
{
	fontSize = fontsize; fontType = fonttype; textBoxLine = line; textBoxBox = textBox; r1 = r, w1 = w;
	color = col;
	marginBoxBox = marginBox;
	createBoxWords(line);
}


TextBox::~TextBox()
{
}

void TextBox::renderBoxWords(SDL_Color fill, SDL_Color border) {
	SDL_SetRenderDrawColor(r1, fill.r, fill.g, fill.b, fill.a);
	SDL_RenderFillRect(r1, &textBoxBox);
	SDL_SetRenderDrawColor(r1, border.r, border.g, border.b, border.a);
	SDL_RenderDrawRect(r1, &textBoxBox);

	for each(Words* word in textBoxWords) {
		word->renderTexture();
	}
}

void TextBox::renderBoxWords(SDL_Color fillStart, SDL_Color fillEnd, double pixelSize, int angle, double growth, double transitionRate, SDL_Color border) {

	SDL_SetRenderDrawColor(r1, fillStart.r, fillStart.g, fillStart.b, fillStart.a);
	SDL_RenderFillRect(r1, &textBoxBox);

	double aRep = 0;
	double a = 0;
	//if (angle != 90) {
		a = tan(angle * 3.14159265 / 180.0);
		aRep = -1.0 / a;
	//}
	//else {
	//	aRep = 0;
//		a = -1;
//	}

	double transitionPercent = 0;

	int y2 = textBoxBox.y + textBoxBox.h - 1;
	int xBound = textBoxBox.x + textBoxBox.w;
	int x1 = textBoxBox.x;
	for (double x = 0; 1 > (transitionPercent = pow(x * transitionRate, growth) / 100); x+= pixelSize) {

		double y0 = (a * x - aRep * x);
		double x0 = (-y0 / aRep);

		int y1 = textBoxBox.y + textBoxBox.h - y0;
		if (y1 < textBoxBox.y) {
			y1 = textBoxBox.y;
			double xBuffer = (textBoxBox.h - y0) / aRep;
			x1 = textBoxBox.x + xBuffer;
			if (x1 > (textBoxBox.w)) break;
		}
		else {
			x1 = textBoxBox.x;
		}
		int x2 = textBoxBox.x + x0;
		if (x2 > xBound) {
				x2 = xBound - 1;
				double xBuffer = ((textBoxBox.w - 2) * aRep + y0);
				y2 = textBoxBox.y + textBoxBox.h - xBuffer;
			}
		else {
			y2 = textBoxBox.y + textBoxBox.h - 1;
		}

		/**! alpha = a(x - b)^2 + c */
		int alpha = fillEnd.a - (transitionPercent ) * 255;
		if (alpha < 0) 
			alpha = 0;
		SDL_SetRenderDrawColor(r1, fillEnd.r, fillEnd.g, fillEnd.b, alpha);
		//SDL_Rect fakeLine = {
		//	textBoxBox.x , textBoxBox.y - y0 ,
		//	textBoxBox.x + x0, textBoxBox.y + textBoxBox.h
		//};
		//SDL_RenderFillRect(r1, &fakeLine);
		SDL_RenderDrawLine(r1, x1, y1 , x2, y2);
	}

	SDL_SetRenderDrawColor(r1, border.r, border.g, border.b, border.a);
	SDL_RenderDrawRect(r1, &textBoxBox);

	for each(Words* word in textBoxWords) {
		word->renderTexture();
	}
}

void TextBox::changeBoxWords(std::string line) {
	for each(Words* word in textBoxWords) {
		word->clearTexture();
	}
	textBoxWords.clear();
	createBoxWords(line);
}

void TextBox::createBoxWords(std::string line) {
	Words* wordStore = new Words(1,1,"err",r1,w1);

	int yPos = marginBoxBox.y;
	int xPos = marginBoxBox.x;
	int numberWhiteSpace = 1;
	int stringPos = 0;
	int oldPos = 0;

	while (line.size() > 0) {
		int wsCount = 0;
		std::string testString;
		bool end = true;
		for each(char character in line) {
			testString += character;
			stringPos++;
			if (isspace(character)) {
				wsCount++;
				if (wsCount == numberWhiteSpace) {
					end = false;
					break;
				}
				oldPos = stringPos;
			}

		}
		Words* textLine = new Words(fontSize, fontType, testString, color, xPos, yPos, r1, w1);
		if ((textLine->box.x + textLine->box.w) > (marginBoxBox.x + marginBoxBox.w)) {
			textLine->clearWords();
			textBoxWords.push_back(wordStore);
			wordStore = new Words(1, 1, "err", color, r1, w1);
			yPos += Graphics::prcnt(0.05, 'y', w1);
			numberWhiteSpace = 1;
			line.erase(0, oldPos);
			stringPos = 0;
			oldPos = 0;
			end = false;
		}
		else {
			wordStore->clearWords();
			wordStore = textLine;
			numberWhiteSpace++;
			stringPos = 0;
			oldPos = 0;
		}
		if (end) {
			textBoxWords.push_back(textLine);
			break;
		}
	}
}