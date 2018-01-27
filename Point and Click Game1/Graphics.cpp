#include "Graphics.h"
#include <boost\archive\binary_iarchive.hpp>
#include <boost\archive\binary_oarchive.hpp>
#include <boost\serialization/vector.hpp>


/**************************************************************************************************************************************************************/



/**************************************************************************************************************************************************************/


float Graphics::scale = 1.0;

Graphics::Graphics(SDL_Renderer* renderer, SDL_Window* window)
{
	this->renderer = renderer;
	this->window = window;
}

Graphics::~Graphics()
{
	SDL_DestroyTexture(texture);
}

void Graphics::setTransparency(int a){
	SDL_SetTextureAlphaMod(texture, a);
}

void Graphics::clearTexture()
{
	SDL_DestroyTexture(texture);
}
void Graphics::renderTexture() {
	SDL_RenderCopy(renderer, texture, NULL, &box);
}
void Graphics::renderTexture(SDL_Rect box) {
	SDL_RenderCopy(renderer, texture, NULL, &box);
}

//CURRENTLY ONLY FOR SCROLLING TOP X TO A LEFT Y
void Graphics::renderTextureDScrolling(double xVel, double yVel, int offXPos, int offYPos, int jumpRestriction, int shift, char axisJump) {
	// change textures to smaller size
	// figure out how to get this to actually work.
	//get it to work with clipping.

	int windowHeight, windowWidth;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	if (!setup) {
		setup = true;
		box = { (int)((windowWidth - offXPos + (xVel * shift)) / 1.0), (int)((offYPos - box.h + (yVel * shift))/1.0), box.w, box.h };
		offBox = {box.x + box.w + (int)((xVel * shift)), box.y - (int)(0.470097244732577*  box.h)  - (int)((yVel * shift)), box.w, box.h };
		boxX = box.x; boxY = box.y;
		boxW = offBox.x; boxH = offBox.y;
	}
	boxX += xVel;		boxW += xVel;
	boxY += yVel;		boxH += yVel;
	box.x = ceil(boxX);		box.y = ceil(boxY);
	offBox.x = ceil(boxW);   offBox.y = ceil(boxH);

	SDL_RenderCopy(renderer, texture, NULL, &box);
	SDL_RenderCopy(renderer, texture, NULL, &offBox);

	if (axisJump == 'xy') {
		if (box.x + box.w < 0 || box.y - box.h > windowHeight) {
			boxX = windowWidth - offXPos;
			boxY = offYPos - box.h;
		}
		if (offBox.x + box.w  < 0 || offBox.y - box.h> windowHeight) {
			boxW = offXPos;
			boxH = offYPos;
		}
	}
	else if (axisJump == 'x') {
		if (box.x + box.w - jumpRestriction < 0) {
			boxX = windowWidth - offXPos;
			boxY = offYPos - box.h;
		}
		if (offBox.x + offBox.w - jumpRestriction  < 0) {
			boxW = windowWidth - offXPos;
			boxH = offYPos - offBox.h;
		}
	}

}

bool Graphics::renderTextureHScrolling(double xVel, double yVel, int offXPos, int offYPos) {

	int windowHeight, windowWidth;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	if (!setup) {
		setup = true;
		box = { box.x + offXPos, box.y + offYPos, box.w, box.h };
		offBox = { box.x + box.w, box.y + offYPos, box.w, box.h };
		boxX = box.x; boxY = box.y;
		boxW = offBox.x; boxH = offBox.y;
	}
	boxX += xVel;		boxW += xVel;
	boxY += yVel;		boxH += yVel;
	box.x = ceil(boxX);		box.y = ceil(boxY);
	offBox.x = ceil(boxW);   offBox.y = ceil(boxH);

	SDL_RenderCopy(renderer, texture, NULL, &box);
	//SDL_RenderCopy(renderer, texture, NULL, &offBox);

	if (box.x + box.w  < 0) {
		return true;
	}
	else return false;
	if (offBox.x + offBox.w  < 0) {
		boxW = windowWidth;
	}

}

bool Graphics::renderTextureScaleScrolling(double xVel, double yVel, int offXPos, int offYPos, double growRate) {

	int windowHeight, windowWidth;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	if (!setup) {
		setup = true;
		box = { box.x + offXPos, box.y + offYPos, box.w, box.h };
		//offBox = { box.x + box.w, box.y + offYPos, box.w, box.h };
		boxX = box.x; boxY = box.y;
		//boxW = offBox.x; boxH = offBox.y;
	}
	boxX += xVel;		//boxW += xVel;
	boxY += yVel;		//boxH += yVel;
	box.x = ceil(boxX);		box.y = ceil(boxY);
	box.w += growRate;		box.h += growRate;
	//offBox.x = ceil(boxW);   offBox.y = ceil(boxH);

	SDL_RenderCopy(renderer, texture, NULL, &box);
	//SDL_RenderCopy(renderer, texture, NULL, &offBox);

	if (box.x + box.w  < 0) {
		return true;
	}
	else return false;
	if (offBox.x + offBox.w  < 0) {
		boxW = windowWidth;
	}

}

bool Graphics::renderTextureScaleScrollingLoop(double xVel, double yVel, int offXPos, int offYPos, double growRate, SDL_Rect* r) {

	int windowHeight, windowWidth;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	if (!setup) {
		setup = true;
		r->x = box.x + offXPos;
		r->y = box.y + offYPos ;
		r->w = box.w;
		r->h = box.h;
	};
	r->x += xVel;		r->y += yVel;
	r->w += growRate;		r->h += growRate;
	//offBox.x = ceil(boxW);   offBox.y = ceil(boxH);

	SDL_RenderCopy(renderer, texture, NULL, r);
	//SDL_RenderCopy(renderer, texture, NULL, &offBox);

	if (r->x + r->w + 400  < 0) {
		setup = false;
		return true;
	}
	else return false;
}

bool Graphics::renderTextureScaleScrollingLoop(double xVel, double yVel, double xAcc, double yAcc, int offXPos, int offYPos, double growRate, SDL_Rect* r) {
	int windowHeight, windowWidth;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	if (!setup) {
		xvel = xVel;
		yvel = yVel;
		setup = true;
		r->x = box.x + offXPos;
		r->y = box.y + offYPos;
		r->w = box.w;
		r->h = box.h;
	};
	r->x += (xvel += xAcc);		
	r->y += (yvel += yAcc);
	r->w += growRate;		
	r->h += growRate;
	//offBox.x = ceil(boxW);   offBox.y = ceil(boxH);

	SDL_RenderCopy(renderer, texture, NULL, r);
	//SDL_RenderCopy(renderer, texture, NULL, &offBox);

	if (r->x + r->w + 400  < 0) {
		setup = false;
		return true;
	}
	else return false;
}

void Graphics::renderTextureTransparent(int alpha){
	SDL_SetTextureAlphaMod(this->texture, alpha);
	SDL_RenderCopy(renderer, texture, NULL, &box);
	SDL_SetTextureAlphaMod(this->texture, 255);
}

bool Graphics::renderTextureFadeIn(long long duration) {
	if (!timeSet) {
		internalTime = SDL_GetTicks();
		alphaCur = alphaOff;
		timeSet = true;
	}
	float rate = 255 / ((float)((float)duration / 1000) * 30);
	alphaCur += rate;

	if (internalTime - SDL_GetTicks() > duration || alphaCur >= 255) {
		alphaCur = 255;
		SDL_RenderCopy(renderer, texture, NULL, &box);
		return true;
	}
	SDL_SetTextureAlphaMod(this->texture, alphaCur);
	SDL_RenderCopy(renderer, texture, NULL, &box);
	return false;
}
bool Graphics::renderTextureFadeOut(long long duration) {
	if (!timeSet) {
		internalTime = SDL_GetTicks();
		alphaCur = alphaMax;
		timeSet = true;
	}
	float rate = 255 / (((float)duration / 1000) * 30);
	alphaCur -= rate;
	if (internalTime - SDL_GetTicks() > duration || alphaCur <= 0) {
		alphaCur = 0;
		return true;
	}
	SDL_SetTextureAlphaMod(this->texture, alphaCur);
	SDL_RenderCopy(renderer, texture, NULL, &box);
	return false;
}

bool Graphics::renderTextureFadeOut(long long duration, SDL_Rect box) {
	if (!timeSet) {
		internalTime = SDL_GetTicks();
		alphaCur = alphaMax;
		timeSet = true;
	}
	float rate = 255 / (((float)duration / 1000) * 30);
	alphaCur -= rate;
	if (internalTime - SDL_GetTicks() > duration || alphaCur <= 0) {
		alphaCur = 0;
		return true;
	}
	SDL_SetTextureAlphaMod(this->texture, alphaCur);
	SDL_RenderCopy(renderer, texture, NULL, &box);
	return false;
}


void Graphics::assignFrameToLocation(int x, int y) {
	assignedFrameCopies.push_back(new Graphics(this, x, y));
}
void Graphics::setMovementPattern(int x1, int y1, int x2, int y2, double moveR) {
	this->x2 = x2; this->y2 = y2; this->moveRate = moveR;
	this->x1 = x1; this->y1 = y1;
}
bool Graphics::singleFrameAnimate(int FPS) {
		//counter++;
		int now = SDL_GetTicks();
		bool move = false;
		if (now - frameTimer > (1/(double)FPS * 1000) ) {
			//counter = 0;
			move = true;
			if (now - internalTime > 100) {
				frameTimer = SDL_GetTicks();
				if (box.x - x2 < moveRate)
					box.x += moveRate;
				else if (box.x - x2 > moveRate)
					box.x -= moveRate;

				if (box.y - y2 < moveRate)
					box.y += moveRate;
				else if (box.y - y2 > moveRate)
					box.y -= moveRate;

				if (abs(box.x - x2) <= moveRate && abs(box.y - y2) <= moveRate) {
					if (!timeSet) {
						internalTime = SDL_GetTicks();
						timeSet = true;
					}
					else timeSet = false;

					int x = x2; int y = y2;
					y2 = y1; x2 = x1;
					y1 = y; x1 = x;
				}
			}
			
		}
	return move;
}
bool Graphics::singleFrameRotate(int FPS, int angleRate) {
	//counter++;
	int now = SDL_GetTicks();
	bool move = false;
	if (now - rotateTimer > (1 / (double)FPS * 1000)) {
		//counter = 0;
		std::cout << angle << "\n";
		move = true;
		if (now - rTime > 100) {
			rotateTimer = SDL_GetTicks();
			if (angle == 15) {
				left = true;
				right = false;
			}
			else if (angle == -15) {
				left =  false;
				right = true;
			}
			if(left)
				angle -= angleRate;
			else
				angle += angleRate;
			//if (abs(box.x - x2) <= moveRate && abs(box.y - y2) <= moveRate) {
			//	if (!timeSet) {
			//		internalTime = SDL_GetTicks();
			//		timeSet = true;
			//	}
			//	else timeSet = false;

			//	int x = x2; int y = y2;
			//	y2 = y1; x2 = x1;
			//	y1 = y; x1 = x;
			//}
		}

	}
	return move;
}
#include <math.h>
int Graphics::searchFramesByCoordinates(int x, int y, int r) {
	int pos = -1;
	for each(Graphics* g in assignedFrameCopies) {
		pos++;
		//std::cout << sqrt((x - g->box.x)*(x - g->box.x) + (y - g->box.y)*(y - g->box.y))  <<" < " << r<< "\n";
		if (sqrt((x - g->box.x)*(x - g->box.x) + (y - g->box.y)*(y - g->box.y)) < r) {
			return pos;
		}
	}
	return -1;
}
int Graphics::searchFramesByCoordinates(int x, int y) {
	int pos = -1;
	for each(Graphics* g in assignedFrameCopies) {
		pos++;
		if (g->box.x < x && (g->box.x + g->box.w) > x && g->box.y < y && (g->box.y + g->box.h) > y) {
			return pos;
		}
	}
	return -1;
}

void Graphics::updateAssignedLocations(double prcntX, double prcntY) {
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);
	for each(Graphics* g in assignedFrameCopies) {
		g->x1 += windowWidth * prcntX; g->x2 += windowWidth* prcntX;
		g->box.x += windowWidth * prcntX; g->box.y += windowHeight* prcntY;
		g->y1 += windowHeight * prcntY; g->y2 += windowHeight* prcntY;
	}
}


void Graphics::setLocation(float percentX, float percentY) {
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	box.x = percentX * windowWidth;
	box.y = percentY * windowHeight;
}

void Graphics::setLocation(double percentX, double percentY) {
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	box.x = percentX * windowWidth;
	box.y = percentY * windowHeight;
}

void Graphics::setLocation(int x, int y) {
	box.x = x;
	box.y = y;
}


void Graphics::renderTextureFlip(SDL_RendererFlip flip) {
	SDL_RenderCopyEx(renderer, texture, NULL, &box, NULL, NULL, flip);
}
void Graphics::renderTextureRotate(double angle) {
	SDL_RenderCopyEx(renderer, texture, NULL, &box, angle, NULL, SDL_FLIP_NONE);
}

//input a percentage of X and Y relative to screen resolution
void Graphics::renderTexture(float percentX, float percentY) {
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);
	SDL_Rect tempBox = { percentX * windowWidth, percentY * windowHeight, box.w, box.h };

	SDL_RenderCopy(renderer, texture, NULL, &tempBox);
}

void Graphics::renderTexture(double percentX, double percentY) {
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);
	SDL_Rect tempBox = { percentX * windowWidth, percentY * windowHeight, box.w, box.h };

	SDL_RenderCopy(renderer, texture, NULL, &tempBox);
}

//input exact int of X and Y relative to screen resolution
void Graphics::renderTexture(int X, int Y) {
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);
	SDL_Rect tempBox = { X, Y, box.w, box.h };

	SDL_RenderCopy(renderer, texture, NULL, &tempBox);
}

//percent to move the box over by. Doesn't change box position
void Graphics::renderTextureClip(float clipPercentX, float clipPercentY, float clipPercentW, float clipPercentH) {
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	int w = clipPercentW * windowWidth;
	if (w > box.w)
		w = box.w;
	int h = clipPercentH * windowHeight;
	if (h > box.h)
		h = box.h;
	SDL_Rect tempBox = { box.x, box.y, w, h };
	SDL_Rect clipRect = { clipPercentX * windowWidth, clipPercentY * windowHeight, w * 1 / Graphics::scale, h * 1 / Graphics::scale };

	SDL_RenderCopy(renderer, texture, &clipRect, &tempBox);
}

void Graphics::renderTextureClip(double xPerc, double yPerc, float clipPercentX, float clipPercentY, float clipPercentW, float clipPercentH) {
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	int w = clipPercentW * windowWidth;
	if (w > box.w)
		w = box.w;
	int h = clipPercentH * windowHeight;
	if (h > box.h)
		h = box.h;
	SDL_Rect tempBox = { xPerc * windowWidth, yPerc * windowHeight, w, h };
	SDL_Rect clipRect = { clipPercentX * windowWidth, clipPercentY * windowHeight, w * 1 / Graphics::scale, h * 1 / Graphics::scale };

	SDL_RenderCopy(renderer, texture, &clipRect, &tempBox);
}

void Graphics::renderTextureClip(int x, int y, int clipX, int clipY, int clipW, int clipH) {
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	int w = clipW;
	if (w > box.w)
		w = box.w;
	int h = clipH;
	if (h > box.h)
		h = box.h;
	SDL_Rect tempBox = { x, y, w, h };
	SDL_Rect clipRect = { clipX, clipY, w, h};

	SDL_RenderCopy(renderer, texture, &clipRect, &tempBox);
}

void Graphics::renderTextureClip(float clipPercentX, float clipPercentY, float clipPercentW, float clipPercentH, int alpha) {
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	int w = clipPercentW * windowWidth;
	if (w > box.w)
		w = box.w;
	int h = clipPercentH * windowHeight;
	if (h > box.h)
		h = box.h;
	SDL_Rect tempBox = { box.x, box.y, w, h };
	SDL_Rect clipRect = { clipPercentX * windowWidth, clipPercentY * windowHeight, w * 1 / Graphics::scale, h * 1 / Graphics::scale };

	SDL_SetTextureAlphaMod(this->texture, alpha);
	SDL_RenderCopy(renderer, texture, &clipRect, &tempBox);
	SDL_SetTextureAlphaMod(this->texture, 255);
}

void Graphics::renderTextureSize(float x, float y, float w, float h) {
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	w = w * windowWidth;
	h = h * windowHeight;
	SDL_Rect tempRect = { x * windowWidth, y * windowHeight, w, h};
	SDL_RenderCopy(renderer, texture, NULL, &tempRect);
}

void Graphics::renderTextureSize(int x, int y, int w, int h) {
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	SDL_Rect tempRect = { x , y, w, h };
	SDL_RenderCopy(renderer, texture, NULL, &tempRect);
}

void Graphics::renderTextureSizeFlip(float x, float y, float w, float h, SDL_RendererFlip flip) {
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	w = w * windowWidth;
	h = h * windowHeight;
	SDL_Rect tempRect = { x * windowWidth, y * windowHeight, w, h };
	SDL_RenderCopyEx(renderer, texture, NULL, &tempRect, NULL, NULL, flip);
}
void Graphics::renderTextureSizeFlip(int x, int y, int w, int h, SDL_RendererFlip flip) {
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	SDL_Rect tempRect = { x, y, w, h };
	SDL_RenderCopyEx(renderer, texture, NULL, &tempRect, NULL, NULL, flip);
}

void Graphics::renderTextureSizeRotate(int x, int y, int w, int h, int angle) {
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	SDL_Rect tempRect = { x , y, w, h };
	SDL_RenderCopyEx(renderer, texture, NULL, &tempRect, angle, NULL, SDL_FLIP_NONE);
}

/**************************************************************************************************************************************************************/



/**************************************************************************************************************************************************************/











//Dangerous
Texture::Texture(SDL_Rect rect, SDL_Texture* image, SDL_Renderer* renderer, SDL_Window* window):Graphics(renderer, window) {
	box = rect;
	texture = image;
}
Texture::Texture(SDL_Rect rect, std::string path, SDL_Renderer* renderer, SDL_Window* window): Graphics(renderer, window) {
	box = rect;
	texture = createImageFromPath(path);
}

Texture::Texture(std::string path, SDL_Renderer* renderer, SDL_Window* window) : Graphics(renderer, window) {
	//box set by texture creation;
	texture = createImageFromPath_SET_SIZE(path);
}
Texture::Texture(float percentX, float percentY, std::string path, SDL_Renderer* renderer, SDL_Window* window) : Graphics(renderer, window) {
	//box set by texture creation;
	texture = createImageFromPath_SET_SIZE(path);

	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	box.x = (int)(percentX * windowWidth);
	box.y = (int)(percentY * windowHeight);
}
Texture::Texture(float percentX, float percentY, std::string path, SDL_Renderer* renderer, SDL_Window* window, bool flag) : Graphics(renderer, window) {
	//int windowWidth;
	//int windowHeight;
	//SDL_GetWindowSize(window, &windowWidth, &windowHeight);


	int windowWidth = 1076;
	int windowHeight = 1100;
	x = box.x = percentX * windowWidth;
	y = box.y = percentY * windowHeight;
	
	//byte stream creation
	if (flag) {
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
		texture = SDL_CreateTextureFromSurface(renderer, free);
		h = box.h = free->h;
		w = box.w = free->w;
		SDL_FreeSurface(free);

		for (int i = 0; i < size; i++)
			byteArrayContainer.push_back(byteArray[i]);
	}
	else {
		//box set by texture creation;
		texture = createImageFromPath_SET_SIZE(path);
	}
}
//modify width and height based off of normal parameters
Texture::Texture(float percentX, float percentY, float percentW, float percentH, std::string path, SDL_Renderer* renderer, SDL_Window* window) : Graphics(renderer, window) {
	//box set by texture creation;
	texture = createImageFromPath_SET_SIZE(path);

	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	box.x = percentX * windowWidth;
	box.y = percentY * windowHeight;
	box.w = percentW * box.w;
	box.h = percentH * box.h;
}
//modify width and height based off of normal parameters
Texture::Texture(float percentX, float percentY, float percentW, float percentH, std::string path, SDL_Renderer* renderer, SDL_Window* window, bool flag) : Graphics(renderer, window) {
	//int windowWidth;
	//int windowHeight;
	//SDL_GetWindowSize(window, &windowWidth, &windowHeight);
	int windowWidth = 1076;
	int windowHeight = 1100;
	x = box.x = percentX * windowWidth;
	y = box.y = percentY * windowHeight;
	if (flag) {
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

		w = box.w = percentW * free->w;
		h = box.h = percentH * free->h;

		texture = SDL_CreateTextureFromSurface(renderer, free);
		SDL_FreeSurface(free);

		for (int i = 0; i < size; i++)
			byteArrayContainer.push_back(byteArray[i]);
	}
	//box set by texture creation;
	else{ texture = createImageFromPath_SET_SIZE(path); }
}
Texture::Texture(Texture* text): Graphics(text->renderer, text->window) {
	box = text->box;
	texture = text->texture;
}

Texture::~Texture() {}

void Texture::changeImageSize(float percentW, float percentH) {
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	box.w = percentW * windowWidth;
	box.h = percentH * windowHeight;
}

SDL_Texture* Texture::createImageFromPath(std::string path) {
	SDL_Texture* newTexture = NULL;
	SDL_Surface* surface = IMG_Load(path.c_str());
	alphaCur = 255;
	if (surface == NULL) std::cout << "SDL_Image Error " << IMG_GetError() << "\n";
	newTexture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	return newTexture;
}

SDL_Texture* Texture::createImageFromPath_SET_SIZE(std::string path) {
	SDL_Texture* newTexture = NULL;
	SDL_Surface* surface = IMG_Load(path.c_str());

	if (surface == NULL)
		std::cout << "SDL_Image Error " << IMG_GetError() << "\n";
	else {
		x = 0; y = 0; w = (int)(surface->w * Graphics::scale); h = (int)(surface->h * Graphics::scale);
		box = SDL_Rect{ 0,0, w, h};
		newTexture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
	}
	return newTexture;
}


void Texture::loadFromByteArray(SDL_Renderer* r, SDL_Window * win) {
	renderer = r; window = win;
	byteArray = new uint8_t[byteArrayContainer.size()];
	int c = 0;
	for each(uint8_t i in byteArrayContainer) {
		byteArray[c++] = i;
	}
	SDL_Surface* free = IMG_Load_RW(SDL_RWFromMem(byteArray, size), 1);
	texture = SDL_CreateTextureFromSurface(renderer, free);
	box = SDL_Rect{ (int)(x * Graphics::scale),(int)(y * Graphics::scale), (int)(w * Graphics::scale), (int)(h * Graphics::scale) };
	SDL_FreeSurface(free);
	delete[] byteArray;
	byteArrayContainer.swap(std::vector<unsigned char>());

	alphaOff = 0;
	alphaMax = 255;
	alphaCur = 255;
	timeSet = false;
}






/**************************************************************************************************************************************************************/







/**************************************************************************************************************************************************************/


//
//DropDownItem::DropDownItem(SDL_Rect rect, std::string path, SDL_Renderer* renderer, SDL_Window* window) : Texture(rect, path, renderer, window)
//{
//	fullyOpened = false;
//	poppingIn = false;
//}
//DropDownItem::DropDownItem(Texture* texture) : Texture(texture)
//{
//	fullyOpened = false;
//	poppingIn = false;
//}
//
//
//DropDownItem::~DropDownItem()
//{
//}




/**************************************************************************************************************************************************************/







/**************************************************************************************************************************************************************/

int Words::textQuality = 2;

//0 for Constantia // 1 for Franklin // 2 for Cirno
Words::Words(int fontSize, int fontKey, std::string textLine, SDL_Renderer* renderer, SDL_Window* window) : Graphics(renderer, window) {
	this->fontSize = fontSize = fontSize * Graphics::scale;
	this->text = textLine;
	
	this->font = decideFont(fontKey);

	//Set the texture size in loadFont
	texture = loadFont(renderer, black, textLine, textQuality);
	box.x = 0;
	box.y = 0;
}
Words::Words(int fontSize, int fontKey, std::string textLine, SDL_Color col, SDL_Renderer* renderer, SDL_Window* window) : Graphics(renderer, window) {

	this->fontSize = fontSize = fontSize * Graphics::scale;
	this->text = textLine;
	this->color = col;
	
	this->font = decideFont(fontKey);

	//Set the texture size in loadFont
	texture = loadFont(renderer, col, textLine, textQuality);
	box.x = 0;
	box.y = 0;
}

Words::Words(int fontSize, int fontKey, std::string textLine, SDL_Color col, float percentX, float percentY, SDL_Renderer* renderer, SDL_Window* window) : Graphics(renderer, window) {

	this->fontSize = fontSize = fontSize * Graphics::scale;
	this->text = textLine;
	this->color = col;

	this->font = decideFont(fontKey);

	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	//Set the texture size in loadFont
	texture = loadFont(renderer, col, textLine, textQuality);
	box.x = percentX * windowWidth;
	box.y = percentY * windowHeight;
}
Words::Words(int fontSize, int fontKey, std::string textLine, SDL_Color col, double percentX, double percentY, SDL_Renderer* renderer, SDL_Window* window) : Graphics(renderer, window) {
	this->fontSize = fontSize = fontSize * Graphics::scale;
	this->text = textLine;
	this->color = col;
	
	this->font = decideFont(fontKey);

	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	//Set the texture size in loadFont
	texture = loadFont(renderer, col, textLine, textQuality);
	box.x = percentX * windowWidth;
	box.y = percentY * windowHeight;
}


//Place Font in certain location of X and Y
Words::Words(int fontSize, int fontKey, std::string textLine, float percentX, float percentY,   SDL_Renderer* renderer, SDL_Window* window) : Graphics(renderer, window) {
	this->fontSize = fontSize = fontSize * Graphics::scale;
	this->text = textLine;
	
	this->font = decideFont(fontKey);

	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	//Set the texture size in loadFont
	texture = loadFont(renderer, black, textLine, textQuality);
	box.x = percentX * windowWidth;
	box.y = percentY * windowHeight;
}

Words::Words(int fontSize, int fontKey, std::string textLine, double percentX, double percentY, SDL_Renderer* renderer, SDL_Window* window) : Graphics(renderer, window) {
	this->fontSize = fontSize = fontSize * Graphics::scale;
	this->text = textLine;
	
	this->font = decideFont(fontKey);

	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	//Set the texture size in loadFont
	texture = loadFont(renderer, black, textLine, textQuality);
	box.x = percentX * windowWidth;
	box.y = percentY * windowHeight;
}

Words::Words(int fontSize, int fontKey, std::string textLine, int X, int Y, SDL_Renderer* renderer, SDL_Window* window) : Graphics(renderer, window) {
	this->fontSize = fontSize = fontSize * Graphics::scale;
	this->text = textLine;
	
	this->font = decideFont(fontKey);

	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	//Set the texture size in loadFont
	texture = loadFont(renderer, black, textLine, textQuality);
	box.x = X;
	box.y = Y;
}

Words::Words(int fontSize, int fontKey, std::string textLine, SDL_Color col, int X, int Y,  SDL_Renderer* renderer, SDL_Window* window) : Graphics(renderer, window) {
	this->fontSize = fontSize = fontSize * Graphics::scale;
	this->text = textLine;

	this->font = decideFont(fontKey);

	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	//Set the texture size in loadFont
	texture = loadFont(renderer, col, textLine, textQuality);
	box.x = X;
	box.y = Y;
}

Words::Words(TTF_Font* font, std::string textLine, SDL_Color col, int X, int Y, SDL_Renderer* renderer, SDL_Window* window) : Graphics(renderer, window) {
	this->text = textLine;

	this->font = font;
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	//Set the texture size in loadFont
	texture = loadFont(renderer, col, textLine, textQuality);
	box.x = X;
	box.y = Y;
}


Words::~Words() {
	SDL_DestroyTexture(texture);
}

TTF_Font* Words::decideFont(int i) {
	switch (i) {
	case 0:	return TTF_OpenFont(ttf_Constantia, fontSize); break;
	case 1:	return TTF_OpenFont(ttf_Franklin, fontSize); break;
	case 2:	return TTF_OpenFont(ttf_Cirno, fontSize); break;
	case 3: return TTF_OpenFont(ttf_NicoMoji, fontSize); break;
	}
}


//input exact int of X and Y relative to screen resolution
void Words::renderWords(std::string line, int X, int Y) {
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	changeText(line);

	this->box.x = X;
	this->box.y = Y;

	SDL_RenderCopy(renderer, texture, NULL, &this->box);
}
void Words::renderWords(SDL_Color color) {
	changeTextColor(color);

	SDL_RenderCopy(renderer, texture, NULL, &this->box);
}
void Words::renderOutlineAndWords() {
	for each(Words* w in outline)
		w->renderTexture();
	renderTexture();
}


//Creates a new word texture with the set color
void Words::changeText(std::string textLine) {
	SDL_DestroyTexture(texture);
	this->text = textLine;

	//Set the texture size in loadFont
	texture = loadFont(renderer, color, textLine, textQuality);
}

//Creates a new word texture with a new color
void Words::changeText(std::string textLine, SDL_Color col) {
	SDL_DestroyTexture(texture);
	this->text = textLine;
	this->color = col;

	//Set the texture size in loadFont
	texture = loadFont(renderer, color, textLine, textQuality);
}

//Method makes call to superclass field box. Sets it with createFontBox(Sdl surface)
SDL_Texture* Words::loadFont(SDL_Renderer* renderer, SDL_Color color, std::string words, int rendering) {
	SDL_Surface* textSurface;
	SDL_Texture* returnTexture;
	//3 for outline, 2 for high, 1 for BG 0 for low, default low
	if (rendering == 0) {
		textSurface = TTF_RenderText_Solid(font, words.c_str(), color);
	}
	else if (rendering == 1) {
		textSurface = TTF_RenderText_Shaded(font, words.c_str(), color, SDL_Color{ 255,255,255 });
	}
	else if (rendering == 2) {
		textSurface = TTF_RenderText_Blended(font, words.c_str(), color);
	}
	else if (rendering == 3) {
		TTF_SetFontOutline(font, 1);
		textSurface = TTF_RenderText_Blended(font, words.c_str(), color);
		TTF_SetFontOutline(font, 0);
	}
	else {
		textSurface = TTF_RenderText_Solid(font, words.c_str(), color);
	}
	this->color = color;
	if (textSurface == NULL) {
		std::cout << TTF_GetError() << "\n";
	}
	box.w = textSurface->w;
	box.h = textSurface->h;
	returnTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_FreeSurface(textSurface);

	return returnTexture;
}

void Words::changeTextColor(SDL_Color col) {
	SDL_DestroyTexture(texture);
	color = col;
	texture = loadFont(renderer, color, text, textQuality);
}

void Words::changeTextFont(int fontsize, int fontKey) {
	SDL_DestroyTexture(texture);
	this->fontSize = fontsize * Graphics::scale;

	switch (fontKey) {
	case 0:	font = TTF_OpenFont(ttf_Constantia, fontSize); break;
	case 1:	font = TTF_OpenFont(ttf_Franklin, fontSize); break;
	case 2:	font = TTF_OpenFont(ttf_Cirno, fontSize); break;
	}

	texture = loadFont(renderer, color, text, textQuality);
}

void Words::changeFont(char* ft) {
	font = TTF_OpenFont(ft, fontSize);
}

std::string Words::getText() {
	return text;
}

void Words::clearWords() {
	for each (Words* w in outline){
		w->clearWords();
	}
	clearTexture();
	TTF_CloseFont(font);
}
#include <sstream>
void Words::createOutline(SDL_Color color) {
	TTF_Font* boldFont = font;
	TTF_SetFontStyle(boldFont, TTF_STYLE_BOLD);
	int advanceCounter = 0;
	/*	outline.push_back(new Words(boldFont, text, color, box.x - 2, box.y, renderer, window));
	outline.at(0)->box.w = box.w + 20;
	outline.at(0)->box.h = box.h;*/
	int i = 0;
	for each(char character in text) {
		std::stringstream ss;
		std::string s;
		ss << character;
		ss >> s;
		int x, X, y, Y, adv;
		TTF_GlyphMetrics(font, character, &x, &X, &y, &Y, &adv);
		std::cout << x << " " << X << " " << adv << " " << y << " " << Y << "\n";
		outline.push_back(new Words(boldFont, s, color, box.x + advanceCounter - x, box.y - 4, renderer, window));
		outline.at(i++)->box.h += 8;
		advanceCounter += adv - 7;
	}
}



/**************************************************************************************************************************************************************/






/**************************************************************************************************************************************************************/












Animation::Animation()
{

	animationSize = frames.size() - 1;
	currentPosition = 0;

	counter = 0;
	done = false;
}

Animation::Animation(std::vector<Texture*> set)
{
	frames = set;
	animationSize = frames.size();
	currentPosition = 0;

	counter = 0;
	done = false;
}

Animation::Animation(std::vector<Texture*> set, bool flag)
{
	frames = set;
	animationSize = frames.size();
	currentPosition = 0;

	counter = 0;
	done = false;
}

Animation::Animation(Texture* sSheet, int fWidth, int fHeight, bool flag)
{
	isSheet = true;
	spriteSheet = sSheet;
	frameHeight = fHeight;
	frameWidth = fWidth;
	animationSize = frames.size();
	currentPosition = 0;

	counter = 0;
	done = false;
}





Animation::~Animation()
{
}


bool Animation::itterateAnimation(int framerate) {
	counter++;
	bool move = false;
	Texture* frame = frames.at(currentPosition);
	if (counter > (gameFPS / framerate-1)) {
		previousPosition = currentPosition;
		currentPosition++;
		move = true;
		if (currentPosition > animationSize - 1) {
			done = true;
			currentPosition = 0;
		}
		counter = 0;
	}
	frame->renderTexture();
	return move;
}

bool Animation::itterateOnce(int framerate) {
	counter++;
	bool move = false;
	Texture* frame = frames.at(currentPosition);
	if (counter > (gameFPS / framerate - 1) && !done) {
		previousPosition = currentPosition;
		currentPosition++;
		move = true;
		if (currentPosition > animationSize - 1) {
			done = true;
			currentPosition = 0;
		}
		counter = 0;
	}
	frame->renderTexture();
	return done;
}

bool Animation::itterateAnimationStartEnd(int framerate, int loopStart, int loopEnd) {
	counter++;
	bool move = false;
	Texture* frame = frames.at(currentPosition);
	if (counter > (gameFPS / framerate - 1)) {
		previousPosition = currentPosition;
		currentPosition++;
		move = true;
		if (currentPosition > loopEnd - 1) {
			done = true;
			currentPosition = loopStart;
		}
		counter = 0;
	}
	frame->renderTexture();
	return move;
}

bool Animation::itterateAnimation(int distanceTraveled, int movementRate) {
	Texture* frame = frames.at(currentPosition);
	bool move = false;
	if (distanceTraveled > oldDistance + movementRate || oldDistance > 0) {
		previousPosition = currentPosition;
		currentPosition++;
		move = true;
		if (currentPosition > animationSize - 1) {
			done = true;
			currentPosition = 0;
		}
		counter = 0;
		if (oldDistance <= 0)
			oldDistance = distanceTraveled;
		oldDistance -= 5.0;
	}
	frame->renderTexture();
	return move;
}

bool Animation::itterateAnimation(int distanceTraveled, int movementRate, int frameSkip) {
	Texture* frame = frames.at((int)currentPosition);
	bool move = false;
	std::cout << distanceTraveled << "\n";
	std::cout << oldDistance << "\n";
	if (distanceTraveled > movementRate || oldDistance > 0) {
		previousPosition = currentPosition;
		frameSkipCounter += frameSkip - floor(frameSkip);
		move = true;
		if (frameSkipCounter > 1) frameSkipCounter = frameSkip - floor(frameSkip);
		currentPosition += (int)floor(frameSkip + frameSkipCounter);
		if (currentPosition > animationSize - 1) {
			done = true;
			currentPosition = 0;
		}
		counter = 0;
		if (oldDistance <= 0)
			oldDistance = distanceTraveled;
		oldDistance -= 20.0;
	}
	frame->renderTexture();
	return move;
}

bool Animation::itterateAnimation(int distanceTraveled, int movementRate, int frameSkip, int loopStart, int loopEnd) {
	Texture* frame = frames.at(currentPosition);
	bool move = false;
	//std::cout << distanceTraveled << "\n";
	//std::cout << oldDistance << "\n";
	if (distanceTraveled > movementRate || oldDistance > 0) {
		previousPosition = currentPosition;
		currentPosition += frameSkip;
		move = true;
		if (currentPosition > loopEnd - 1) {
			done = true;
			currentPosition = loopStart;
		}
		counter = 0;
		if(oldDistance <= 0)
			oldDistance = distanceTraveled;
		oldDistance -= 5.0;
	}
	frame->renderTexture();
	return move;
}

//! Requires main to reset distance traveled
bool Animation::itterateAnimationRefreshed(int distanceTraveled) {
	Texture* frame = frames.at(currentPosition);
	bool move = false;
	if (distanceTraveled == 0) {
		previousPosition = currentPosition;
		currentPosition++;
		move = true;
		if (currentPosition > animationSize - 1) {
			done = true;
			currentPosition = 0;
		}
		counter = 0;
	}
	frame->renderTexture();
	return move;
}

void Animation::setLocations(float prcntX, float prcntY) {
	for each(Texture* frame in frames) {
		frame->setLocation(prcntX, prcntY);
	}
}
void Animation::setLocations(double prcntX, double prcntY) {
	for each(Texture* frame in frames) {
		frame->setLocation(prcntX, prcntY);
	}
}
void Animation::setLocations(int x, int y) {
	for each(Texture* frame in frames) {
		frame->setLocation(x, y);
	}
}


void Animation::destroyAnimation() {
	int frameSize = frames.size();
	for (int i = 0; i < frameSize; i++) {
		frames.at(i)->clearTexture();
	}
}

void Animation::resetAnimation() {
	currentPosition = 0;
}

bool Animation::checkDone() {
	if (done) {
		return true;
	}
	else {
		return false;
	}
}

void Animation::loadFromByteArray(SDL_Renderer* r, SDL_Window* w) {
	for each(Texture* t in frames) {
		t->loadFromByteArray(r, w);
	}
	animationSize = frames.size();
	currentPosition = 0;

	counter = 0;
	done = false;
}