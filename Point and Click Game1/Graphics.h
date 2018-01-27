#include <SDL2\SDL.h>
#include <SDL_Image\SDL_image.h>
#include <SDL_TTF\SDL_ttf.h>

#include<iostream>
#include <string>
#include <vector>

#include <boost\archive\binary_iarchive.hpp>
#include <boost\archive\binary_oarchive.hpp>
#include <boost\serialization/vector.hpp>
#include <boost\serialization/string.hpp>
#include <boost/serialization/config.hpp>
#include <boost/serialization/shared_ptr.hpp>


#pragma once

		class Graphics;				class Animation;
class Texture; class Words;
class DropDownItem;

////////////////ADDITION OF SERIALIZATION TO ALL SDL PRIMITIVES//////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Graphics
{

	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive&ar, const unsigned int ver) {
	}

public:
	Graphics() {
		box.x = boxX;
		box.y = boxY;
		box.w = boxW;
		box.h = boxH;
	};
	Graphics(SDL_Renderer* renderer, SDL_Window* window);
	Graphics(Graphics* g, int x, int y) {
		texture = g->texture;
		box = g->box;
		box.x = x;
		box.y = y;
		renderer = g->renderer;
		window = g->window;
	};
	~Graphics();

	bool timeSet;
	bool setup = false;
	int alphaOff = 0;
	int alphaMax = 255;
	float alphaCur;
	static float scale;
	long long internalTime;
	double rTime;
	int frameTimer; bool left, right;
	int rotateTimer;
	double xvel, yvel;

	SDL_Renderer* renderer;
	SDL_Window* window;
	SDL_Rect box; double boxX, boxY, boxW, boxH;
	SDL_Texture* texture;

	SDL_Rect offBox;

	void setTransparency(int a);

	void clearTexture();
	void renderTexture();
	void renderTexture(int X, int Y);
	void renderTexture(SDL_Rect box);
	void renderTexture(float percentX, float percentY);
	void renderTexture(double percentX, double percentY);
	bool renderTextureHScrolling(double xVel, double yVel, int offXPos, int offYPos);
	bool renderTextureScaleScrolling(double xVel, double yVel, int offXPos, int offYPos, double growRate);
	bool renderTextureScaleScrollingLoop(double xVel, double yVel, int offXPos, int offYPos, double growRate, SDL_Rect * r);
	bool renderTextureScaleScrollingLoop(double xVel, double yVel, double xAcc, double yAcc, int offXPos, int offYPos, double growRate, SDL_Rect * r);
	void renderTextureDScrolling(double xVel, double yVel, int offXPos, int offYPos, int jumpRestriction, int shift, char axisJump);
	void renderTextureTransparent(int alpha);
	bool renderTextureFadeIn(long long duration);
	bool renderTextureFadeOut(long long duration);
	bool renderTextureFadeOut(long long duration, SDL_Rect);
	void renderTextureClip(float percentX, float percentY, float percentH, float percentW);
	void renderTextureClip(int x, int y, int clipPercentX, int clipPercentY, int clipPercentW, int clipPercentH);
	void renderTextureClip(double xPerc, double yPerc, float clipPercentX, float clipPercentY, float clipPercentW, float clipPercentH);
	void renderTextureClip(float percentX, float percentY, float percentH, float percentW, int alpha);
	void renderTextureRotate(double angle);
	void renderTextureFlip(SDL_RendererFlip flip);
	void renderTextureSize(float x, float y, float w, float h);
	void renderTextureSize(int x, int y, int w, int h);
	void renderTextureSizeFlip(float x, float y, float w, float h, SDL_RendererFlip flip);
	void renderTextureSizeFlip(int x, int y, int w, int h, SDL_RendererFlip flip);
	void renderTextureSizeRotate(int x, int y, int w, int h, int angle);
	//prefferable to just set location if permanent for texture

	//single frame animations
	void setMovementPattern(int x1, int y1, int x2, int y2, double moveRate);
	bool singleFrameAnimate(int FPS);
	bool singleFrameRotate(int FPS, int angle);
	void renderSingleFrame() { SDL_RenderCopyEx(renderer, texture, NULL, &box, angle, NULL, SDL_FLIP_NONE); }
	void assignFrameToLocation(int x, int y);
	std::vector<Graphics*> assignedFrameCopies;
	void clearFrames() { assignedFrameCopies.clear(); }
	int assignmentNumber;//holds vector position
	int counter;int angle;
	int x2, y2; double moveRate;int x1; int y1;
	int searchFramesByCoordinates(int x, int y, int r);
	int searchFramesByCoordinates(int x, int y);
	void updateAssignedLocations(double prcntX, double prcntY);

	void resetTimer() { internalTime = 0; timeSet = false; setup = false; }

	void setLocation(float percentX, float percentY);
	void setLocation(double x, double y);
	void setLocation(int x, int y);
	
	SDL_Color black = SDL_Color{ 0x00,0x00,0x00,00 };
	SDL_Color white = SDL_Color{ 0xff,0xff,0xff,00 };

	//static instantiation of prcnt method using determined window paramater
	static int prcnt(float percent, char axis, SDL_Window*window) {
		int windowWidth;
		int windowHeight;
		SDL_GetWindowSize(window, &windowWidth, &windowHeight);

		if (axis == 'x') return percent * windowWidth;
		else if (axis == 'y') return percent * windowHeight;
		else return 0;
	}
	//nonstatic instantiation of prcnt method using predetermined window paramater
	int prcnt(float percent, char axis) {
		int windowWidth;
		int windowHeight;
		SDL_GetWindowSize(window, &windowWidth, &windowHeight);

		if (axis == 'x') return percent * windowWidth;
		else if (axis == 'y') return percent * windowHeight;
		else return 0;
	}

	//Scaling 0 is nearest. 1 is linear. 2 is anisotropic. This is by order of quality
	static bool initialize(int scaling) {
		if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) /*(SDL_Init(SDL_INIT_EVERYTHING) < 0) */{
			std::cout << "SDL_INIT FAILED\n";
			return false;
		}
		if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
			std::cout << "IMG_INIT FAILED\n";
			return false;
		}
		if (TTF_Init() == -1) {
			std::cout << "TTF_INIT FAILED\n";
			return false;
		}

		//Set the interpolation/scale quality 
		//THIS WAS JUST ADDED, CHECK IF THIS IS SCREWING UP
		switch (scaling) {
		case 1: 
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
			break;
		case 2:
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
			break;
		default:
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
		}
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Texture : public Graphics
{
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int i) {
		ar & (x = box.x);
		ar & (y = box.y);
		ar & (w = box.w);
		ar & (h = box.h);
		ar & byteArrayContainer;
		ar & size;
	}

public:
	/**Setting the rect will cause issues with built in scaling*/
	Texture() {};
	Texture(SDL_Rect rect, SDL_Texture* image, SDL_Renderer* renderer, SDL_Window* window);
	Texture(SDL_Rect rect, std::string path, SDL_Renderer* renderer, SDL_Window* window);

	Texture(float percentX, float percentY, std::string path, SDL_Renderer* renderer, SDL_Window* window);
	Texture(float percentX, float percentY, std::string path, SDL_Renderer* renderer, SDL_Window* window, bool flag);
	Texture(float percentX, float percentY, float percentW, float percentH, std::string path, SDL_Renderer* renderer, SDL_Window* window);
	Texture(float percentX, float percentY, float percentW, float percentH, std::string path, SDL_Renderer* renderer, SDL_Window* window, bool flag);
	Texture(std::string path, SDL_Renderer* renderer, SDL_Window* window);
	Texture(std::string path, SDL_Renderer* renderer, SDL_Window* window, bool flag) {
		this->renderer = renderer;
		this->window = window;
		raw = SDL_RWFromFile(path.c_str(), "rb");
		if (raw == nullptr) {
			std::cout << "NULL: " << path << "\n";
			return;
		}
		size = raw->size(raw);
		std::cout << size << std::endl;
		byteArray = new uint8_t[size];
		raw->read(raw, byteArray, 1, size);
		SDL_Surface* free = IMG_Load_RW(SDL_RWFromMem(byteArray, size), 1);
		if (free == NULL) {
			std::cout << "NULL: " << SDL_GetError() << "\n";
			printf("IMG_Load: %s\n", IMG_GetError());
		}
		texture = SDL_CreateTextureFromSurface(renderer, free);
		if (texture == NULL) {
			std::cout << "NULL: " << SDL_GetError() << "\n";
			printf("IMG_Load: %s\n", IMG_GetError());
		}

		x = box.x = 0;
		y = box.y = 0;
		h = box.h = free->h * Graphics::scale;
		w = box.w = free->w * Graphics::scale;
		SDL_FreeSurface(free);
		SDL_FreeRW(raw);
		for (int i = 0; i < size; i++)
			byteArrayContainer.push_back(byteArray[i]);

	}
	Texture(Texture* text);

	~Texture();

	void changeImageSize(float percentW, float percentH);

	void clearTexture() {
		Graphics::clearTexture();
		byteArrayContainer.swap(std::vector<unsigned char>());
	}

	void loadFromByteArray(SDL_Renderer* r, SDL_Window * w);

private:
	SDL_Texture* createImageFromPath(std::string path);
	SDL_Texture* createImageFromPath_SET_SIZE(std::string path);
	SDL_RWops* raw;
	std::vector<uint8_t> byteArrayContainer;
	uint8_t * byteArray ;
	int size;
	int x, y, w, h;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//class DropDownItem : public Texture
//{
//public:
//	DropDownItem(SDL_Rect rect, std::string path, SDL_Renderer* renderer, SDL_Window* window);
//	DropDownItem(Texture* texture);
//	~DropDownItem();
//
//	bool fullyOpened;
//	bool poppingIn;
//};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Words : public Graphics
{
public:
	Words() { };
	Words(int fontSize, int fontKey, std::string textLine, SDL_Renderer* renderer, SDL_Window* window);
	Words(int fontSize, int fontKey, std::string textLine, SDL_Color col, SDL_Renderer* renderer, SDL_Window* window);
	Words(int fontSize, int fontKey, std::string textLine, SDL_Color col, float percentX, float percentY, SDL_Renderer* renderer, SDL_Window* window);
	Words(int fontSize, int fontKey, std::string textLine, SDL_Color col, double percentX, double percentY, SDL_Renderer* renderer, SDL_Window* window);
	Words(int fontSize, int fontKey, std::string textLine, float percentX, float percentY,  SDL_Renderer* renderer, SDL_Window* window);
	Words(int fontSize, int fontKey, std::string textLine, double percentX, double percentY, SDL_Renderer* renderer, SDL_Window* window);
	Words(int fontSize, int fontKey, std::string textLine, int x, int y, SDL_Renderer* renderer, SDL_Window* window);
	Words(int fontSize, int fontKey, std::string textLine, SDL_Color col, int X, int Y, SDL_Renderer* renderer, SDL_Window* window);
	Words(TTF_Font* font, std::string textLine, SDL_Color col, int X, int Y, SDL_Renderer* renderer, SDL_Window* window);
	~Words();

	std::string getText();
	TTF_Font* getFont() { return font; }

	void renderWords(std::string line, int X, int Y);
	void renderWords(SDL_Color);
	void renderOutlineAndWords();

	void changeText(std::string textLine);
	void changeText(std::string textLine, SDL_Color color);
	void changeTextFont(int fontsie, int fontKey);
	void changeTextColor(SDL_Color color);
	void changeFont(char* ft);

	void createOutline(SDL_Color color);

	void clearWords();
	static int textQuality;

	static const Uint16* stUi16(std::string s) {
		Uint16* ret = new Uint16[100];
		int i = 0;
		for each(char c in s) {
			ret[i++] = (Uint16)c;
		}
		return ret;
	}

private:
	TTF_Font* decideFont(int i);
	char* ttf_Cirno = "Fonts/Cirno.ttf";
	char* ttf_Constantia = "Fonts/Constantia.ttf";
	char* ttf_Franklin = "Fonts/franklin-gothic-medium.ttf";
	char* ttf_NicoMoji = "Fonts/nicomoji-plus_1.11.ttf";

	SDL_Texture* loadFont(SDL_Renderer* renderer, SDL_Color color, std::string, int rendering);

	std::vector<Words*> outline;

	std::string text;
	TTF_Font* font;
	int fontSize;
	SDL_Color color;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Animation
{
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int i) {
		ar & frames;
	}
public:
	Animation();
	Animation(std::vector<Texture*> set);
	Animation(std::vector<Texture*> set, bool flag);
	Animation(Texture* spritesheet, int frameWidth, int frameHeight, bool flag);
	~Animation();


	bool itterateAnimation(int framerate);
	bool itterateOnce(int framerate);
	bool itterateAnimationStartEnd(int framerate, int loopStart, int loopEnd);
	bool itterateAnimation(int distanceTraveled, int movementRate);
	bool itterateAnimation(int distanceTraveled, int movementRate, int frameSkip);
	bool itterateAnimation(int distanceTraveled, int movementRate, int frameSkip, int loopStart, int loopEnd);
	bool itterateAnimationRefreshed(int distanceTraveled);
	void destroyAnimation();
	void resetAnimation();
	void setLocations(float x, float y);
	void setLocations(double x, double y);
	void setLocations(int x, int y);

	bool checkDone();

	void loadFromByteArray( SDL_Renderer* r, SDL_Window* w);

	int counter;
	const int gameFPS = 60;

	bool done;
	bool isSheet;

	int currentPosition;
	int previousPosition;
	int animationSize;
	double oldDistance = 0;
	double frameSkipCounter;
	std::vector<Texture*> frames;

	Texture* spriteSheet;
	int frameWidth, frameHeight;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////