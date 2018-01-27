#pragma once
#include <vector>
#include <SDL2\SDL.h>

class Texture;
class DropDownItem;

class DropDownMenu
{
public:
	//1 for OpU ClD, 2 for OpL ClR
	DropDownMenu(int axis, std::vector<Texture*> textures);
	~DropDownMenu();

	std::vector<DropDownItem*> tabs;
	Texture* baseTexture;
	Texture* arrowTexture;

	int moveRate;
	int visibleTabs;
	bool menuMoving = false;
	bool menuOpen = false;
	bool menuClosed = true;

	void setTabBools(bool val);

	void renderMenu();

	//functors for the movement
	void open();
	void close();

	void popIn();

private:
	//define void
	typedef void(DropDownMenu::*DIRECTION)();
	DIRECTION directionOpen;
	DIRECTION directionClose;

	SDL_RendererFlip flipAngle;

	void up();
	void closeUp();

	void down();

	void left();

	void right();
	void closeRight();


};

