#pragma once
#include "Graphics.h"

#include <fstream>
#include <stdarg.h>

#include <boost\archive\binary_iarchive.hpp>
#include <boost\archive\binary_oarchive.hpp>
#include <boost\serialization/vector.hpp>

class AnimationLibrary {

	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & idleAni;

		ar & moAni;
		ar & lPiAni;
		ar & rPiAni;
		ar & fiAni;
		ar & haAni;

		ar & eyesAni;
		ar &felAni;
		ar &masAni;
		ar &noBreAni;
		ar &breAni;
		ar &thiAni;

		ar & pokeFake;
		ar & handFake;
	}

public:
	AnimationLibrary(Animation* n_args, ...) {

	}
	AnimationLibrary() {

	}

	void saveAnimationLibrary() {
		std::fstream file;
		file.open("data/ani.rave", std::ios::binary | std::fstream::out);

		boost::archive::binary_oarchive archive(file);
		archive << this;
		file.close();
	}

	static AnimationLibrary loadAnimationLibrary() {
		std::fstream file;
		AnimationLibrary* al;
		AnimationLibrary alCopy;
		file.open("data/ani.rave", std::ios::binary | std::fstream::in);
		if (file.is_open()) {
			boost::archive::binary_iarchive archive(file);
			archive >> al;
			alCopy = *al;
		}
		else {
			std::cout << "GAME ANIMATION DATA NOT FOUND";
			al = new AnimationLibrary();
			alCopy = *al;
		}
		file.close();

		return alCopy;
	}

	Animation* moAni;
	Animation* lPiAni;
	Animation* rPiAni;
	Animation* fiAni;
	Animation* haAni;

	Animation* eyesAni;
	Animation* felAni;
	Animation* masAni;
	Animation* noBreAni;
	Animation* breAni;
	Animation* thiAni;

	Animation* idleAni;

	Texture* pokeFake;
	Texture* handFake;

	void clearLibrary() {
		moAni->destroyAnimation();
		idleAni->destroyAnimation();
		pokeFake->clearTexture();
		handFake->clearTexture();

		lPiAni->destroyAnimation();
		rPiAni->destroyAnimation();
		fiAni->destroyAnimation();
		haAni->destroyAnimation();

		eyesAni->destroyAnimation();
		felAni->destroyAnimation();
		masAni->destroyAnimation();
		noBreAni->destroyAnimation();
		breAni->destroyAnimation();
		thiAni->destroyAnimation();
	}

	void configureLibrary(SDL_Renderer* r, SDL_Window* w) {
		moAni->loadFromByteArray(r, w);
		lPiAni->loadFromByteArray(r, w);
		rPiAni->loadFromByteArray(r, w);
		fiAni->loadFromByteArray(r, w);
		haAni->loadFromByteArray(r, w);

		idleAni->loadFromByteArray(r, w);
		pokeFake->loadFromByteArray(r, w);
		handFake->loadFromByteArray(r, w);

		eyesAni->loadFromByteArray(r, w);
		felAni->loadFromByteArray(r, w);
		masAni->loadFromByteArray(r, w);
		noBreAni->loadFromByteArray(r, w);
		breAni->loadFromByteArray(r, w);
		thiAni->loadFromByteArray(r, w);
	}

	void configure1Third(SDL_Renderer* r, SDL_Window* w){
		moAni->loadFromByteArray(r, w);
		lPiAni->loadFromByteArray(r, w);
		rPiAni->loadFromByteArray(r, w);
		fiAni->loadFromByteArray(r, w);
		haAni->loadFromByteArray(r, w);

	}
	void configure2Third(SDL_Renderer* r, SDL_Window* w) {
		idleAni->loadFromByteArray(r, w);
		pokeFake->loadFromByteArray(r, w);
		handFake->loadFromByteArray(r, w);
		eyesAni->loadFromByteArray(r, w);
		felAni->loadFromByteArray(r, w);
	}
	void configure3Third(SDL_Renderer* r, SDL_Window* w) {
		masAni->loadFromByteArray(r, w);
		noBreAni->loadFromByteArray(r, w);
		breAni->loadFromByteArray(r, w);
		thiAni->loadFromByteArray(r, w);
	}
};
