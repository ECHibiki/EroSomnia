#pragma once

#include "Graphics.h"

#include <fstream>

#include <boost\archive\binary_iarchive.hpp>
#include <boost\archive\binary_oarchive.hpp>
#include<boost/serialization/vector.hpp>


class MenuGraphics {

	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int vers) {
		ar & card0; // used for op
		ar & card1; // used for unlock 1
		ar & card2;
		ar & card3;
		ar & card4;

		ar & MenuBG;
		ar & Title;
		ar & Z3;
		ar & Z2;
		ar & Z1;

		ar & subBG;

		ar & subMenuIconR;
		//SubMenu::subMenuIconL = new Texture(-1, 0.6, "art/iconL.png", Items::r1, Items::w1);
		ar & ExtraBG;
		ar & ExtraBGHolder;
		ar & pendulumBase;
		ar & pendulumRod;
		ar & pendulumBase2;
		ar & pendulumRod2;

		//ar & containerTextureEC;
		//ar & containerTextureSH;

		ar & bg1;
		ar & bg2;
		ar & bg3;
		ar & bg4;
		ar & bg5;
		ar & bg6;
	}
public:
	MenuGraphics();

	void saveMenuGraphics() {
		std::fstream file;
		file.open("data/menu.rave", std::ios::binary | std::fstream::out);
		boost::archive::binary_oarchive archive(file);
		archive << this;
		file.close();
	}
	static MenuGraphics loadMenuGraphics(){
		std::fstream file;
		MenuGraphics *mg;
		MenuGraphics mgCopy;
		file.open("data/menu.rave", std::ios::binary | std::fstream::in);
		if (file.is_open()) {
			boost::archive::binary_iarchive archive(file);
			archive >> mg;
			mgCopy = *mg;
		}
		else{
			std::cout << "MENU GRAPHICS NOT FOUND";
			mg = new MenuGraphics();
			mgCopy = *mg;
		}
		file.close();
		return mgCopy;
	}

	Texture* card0; // used for op
	Texture* card1; // used for unlock 1
	Texture* card2;
	Texture* card3;
	Texture* card4;

	Texture* MenuBG;
	Texture* Title;
	Texture* Z3;
	Texture* Z2;
	Texture* Z1;

	Texture* subBG;

	Texture* subMenuIconR;
	//SubMenu::subMenuIconL = new Texture(-1, 0.6, "art/iconL.png", Items::r1, Items::w1);
	Texture* ExtraBG;
	Texture* ExtraBGHolder;
	Texture* pendulumBase;
	Texture* pendulumRod;
	Texture* pendulumBase2;
	Texture* pendulumRod2;

	//Texture* containerTextureEC;
	//Texture* containerTextureSH;

	Texture* bg1;
	Texture* bg2;
	Texture* bg3;
	Texture* bg4;
	Texture* bg5;
	Texture* bg6;

	void configureLibrary(SDL_Renderer * r, SDL_Window * w) {
		card0->loadFromByteArray(r, w); // used for op
		card0->box = SDL_Rect{ 0,0,Graphics::prcnt(1, 'x', w), Graphics::prcnt(1, 'y', w) };
		card1->loadFromByteArray(r, w); // used for unlock 1
		card1->box = SDL_Rect{ 0,0,Graphics::prcnt(1, 'x', w), Graphics::prcnt(1, 'y', w) };
		card2->loadFromByteArray(r, w);
		card2->box = SDL_Rect{ 0,0,Graphics::prcnt(1, 'x', w), Graphics::prcnt(1, 'y', w) };
		card3->loadFromByteArray(r, w);
		card3->box = SDL_Rect{ 0,0,Graphics::prcnt(1, 'x', w), Graphics::prcnt(1, 'y', w) };
		card4->loadFromByteArray(r, w);
		card4->box = SDL_Rect{ 0,0,Graphics::prcnt(1, 'x', w), Graphics::prcnt(1, 'y', w) };

		MenuBG->loadFromByteArray(r, w);
		Title->loadFromByteArray(r, w);
		Z3->loadFromByteArray(r, w);
		Z2->loadFromByteArray(r, w);
		Z1->loadFromByteArray(r, w);

		subBG->loadFromByteArray(r, w);

		subMenuIconR->loadFromByteArray(r, w);
		//SubMenu::subMenuIconL = new Texture(-1, 0.6, "art/iconL.png", Items::r1, Items::w1)->loadFromByteArray(r, w);
		ExtraBG->loadFromByteArray(r, w);
		ExtraBGHolder->loadFromByteArray(r, w);
		pendulumBase->loadFromByteArray(r, w);
		pendulumRod->loadFromByteArray(r, w);
		pendulumBase2->loadFromByteArray(r, w);
		pendulumRod2->loadFromByteArray(r, w);

		//containerTextureEC->loadFromByteArray(r, w);
		//containerTextureSH->loadFromByteArray(r, w);

		bg1->loadFromByteArray(r, w);
		bg2->loadFromByteArray(r, w);
		bg3->loadFromByteArray(r, w);
		bg4->loadFromByteArray(r, w);
		bg5->loadFromByteArray(r, w);
		bg6->loadFromByteArray(r, w);
	}
	void clearLibrary() {
		card0->clearTexture(); // used for op
		card1->clearTexture(); // used for unlock 1
		card2->clearTexture();
		card3->clearTexture();
		card4->clearTexture();

		MenuBG->clearTexture();
		Title->clearTexture();
		Z3->clearTexture();
		Z2->clearTexture();
		Z1->clearTexture();

		subBG->clearTexture();

		subMenuIconR->clearTexture();
		//SubMenu::subMenuIconL = new Texture(-1, 0.6, "art/iconL.png", Items::r1, Items::w1)->clearTexture();
		ExtraBG->clearTexture();
		ExtraBGHolder->clearTexture();
		pendulumBase->clearTexture();
		pendulumRod->clearTexture();
		pendulumBase2->clearTexture();
		pendulumRod2->clearTexture();

		//containerTextureEC->clearTexture();
		//containerTextureSH->clearTexture();

		bg1->clearTexture();
		bg2->clearTexture();
		bg3->clearTexture();
		bg4->clearTexture();
		bg5->clearTexture();
		bg6->clearTexture();
	}

};

MenuGraphics::MenuGraphics(){}