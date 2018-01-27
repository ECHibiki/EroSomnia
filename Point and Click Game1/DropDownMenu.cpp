//#include "DropDownMenu.h"
//#include "Graphics.h"
//
//#define BASE_TEXTURE baseTexture->box
//#define ARROW_TEXTURE arrowTexture->box
//#define LOWEST_TAB tabs.at(0)->box
//
//DropDownMenu::DropDownMenu(int axis, std::vector<Texture*> textures)
//{
//
//	//set the base and arrow
//	baseTexture = textures.at(0);
//	arrowTexture = textures.at(1);
//	arrowTexture->box = { BASE_TEXTURE.x, BASE_TEXTURE.y + BASE_TEXTURE.w, ARROW_TEXTURE.w, ARROW_TEXTURE.h};
//
//	moveRate = baseTexture->prcnt(0.03, 'y');
//
//	for (int i = 2; i < textures.size(); i++) {
//		DropDownItem* ddi = new DropDownItem(textures.at(i));
//		ddi->fullyOpened = false;
//		tabs.push_back(ddi);
//	}
//
//
//	//UD
//	if (axis == 1) {
//		directionOpen = &DropDownMenu::down;
//		directionClose = &DropDownMenu::closeUp;
//		flipAngle = SDL_FLIP_VERTICAL;
//	}
//	//LR
//	else if (axis == 2) {
//		directionOpen = &DropDownMenu::left;
//		directionClose = &DropDownMenu::closeRight;
//		flipAngle = SDL_FLIP_HORIZONTAL;
//	}
//	else if (axis == 3) {
//
//	}
//	else if (axis == 4) {
//
//	}
//	else {
//
//	}
//}
//
//DropDownMenu::~DropDownMenu()
//{
//}
//
//void DropDownMenu::setTabBools(bool val) {
//	for each (DropDownItem* item in tabs) {
//		if (item->fullyOpened != val) {
//			item->fullyOpened = val;
//		}
//		if (item->poppingIn != val) {
//			item->poppingIn = val;
//		}
//	}
//}
//
//void DropDownMenu::renderMenu() {
//	int count = 0;
//	for each(DropDownItem* item in tabs) {
//		if (item->poppingIn == true || item->fullyOpened == true) {
//			item->renderTexture();
//		}
//	}
//	baseTexture->renderTexture();
//	if (menuClosed && menuMoving || menuOpen && !menuMoving) {
//		arrowTexture->renderTextureFlip(flipAngle);
//	}
//	else arrowTexture->renderTexture();
//}
//
//
//void DropDownMenu::open() {
//	tabs.at(0)->poppingIn = true;
//	visibleTabs = 0;
//	(this->*directionOpen)();
//}
//void DropDownMenu::close() {
//	visibleTabs = tabs.size() - 1;
//	(this->*directionClose)();
//}
//void DropDownMenu::popIn() {
//
//}
//
//void DropDownMenu::closeUp() {
//	int count = tabs.size() - 1;
//	for each(DropDownItem* item in tabs) {
//		if (item->fullyOpened == false) {
//			visibleTabs = --count;
//		}
//	}
//
//	for (int i = visibleTabs; i != -1; i--) {
//		tabs.at(i)->box.y -= moveRate;
//	}
//
//
//	if (tabs.at(visibleTabs)->box.y < BASE_TEXTURE.y + moveRate) {
//		tabs.at(visibleTabs)->poppingIn = false;
//		tabs.at(visibleTabs)->fullyOpened = false;
//		if (visibleTabs == 0) {
//			menuMoving = false;
//			menuClosed = true;
//			menuOpen = false;
//			ARROW_TEXTURE = { BASE_TEXTURE.x, BASE_TEXTURE.y + BASE_TEXTURE.w, ARROW_TEXTURE.w, ARROW_TEXTURE.h };
//			return;
//		}
//	}
//
//	ARROW_TEXTURE = { LOWEST_TAB.x, LOWEST_TAB.y + LOWEST_TAB.w, ARROW_TEXTURE.w, ARROW_TEXTURE.h };
//}
//void DropDownMenu::down() {
//	int count = -1;
//	for each(DropDownItem* item in tabs) {
//		if (item->poppingIn == true) {
//			visibleTabs = ++count;
//		}
//	}
//
//	int latestTab = 0;
//	for (int i = 0; i < visibleTabs + 1; i++) {
//		tabs.at(i)->box.y += moveRate;
//		latestTab = i;
//	}
//
//	if (tabs.at(latestTab)->box.y > BASE_TEXTURE.y + BASE_TEXTURE.h - moveRate) {		
//		if (latestTab + 1 == tabs.size()) {
//			tabs.at(latestTab)->fullyOpened = true;
//			menuMoving = false;
//			menuClosed = false;
//			menuOpen = true;
//		}
//		else {
//			tabs.at(latestTab + 1)->poppingIn = true;
//			tabs.at(latestTab)->fullyOpened = true;
//		}
//	}
//	ARROW_TEXTURE = { LOWEST_TAB.x, LOWEST_TAB.y + LOWEST_TAB.w, ARROW_TEXTURE.w, ARROW_TEXTURE.h };
//}
//void DropDownMenu::left() {
//	std::cout << "LLL";
//}
//void DropDownMenu::right() {
//	std::cout << "DONT USE";
//}
//void DropDownMenu::closeRight() {
//	std::cout << "RRR";
//}