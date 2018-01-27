#pragma once
#include "graphics.h"
#include "SFX.h"

#include <stdarg.h>

class OpeningItems
{

	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive &ar, const unsigned int vers) {
		ar& StartSFX;
		ar& logoEC;
		ar& logo2;
		ar& white;
	}
public:
	OpeningItems(){
	
	}

	OpeningItems(Texture* n_args, ...) {
	}

	void saveOpeningItems() {
		std::fstream file;
		file.open("data/op.rave", std::ios::binary | std::fstream::out);
		boost::archive::binary_oarchive archive(file);
		archive << this;
		file.close();
	}
	static OpeningItems loadOpeningItems() {
		std::fstream file;
		OpeningItems *oi;
		OpeningItems oiCopy;
		file.open("data/op.rave", std::ios::binary | std::fstream::in);
		if (file.is_open()) {
			boost::archive::binary_iarchive archive(file);
			archive >> oi;
			oiCopy = *oi;
		}
		else {
			std::cout << "Opening items NOT FOUND";
			oi = new OpeningItems();
			oiCopy = *oi;
		}
		file.close();
		return oiCopy;
	} 

	SFX* StartSFX;
	Texture* logoEC;
	Texture* logo2;
	Texture* white;

	void configureLibrary(SDL_Renderer* r, SDL_Window * w) {
		StartSFX->loadFromByteArray();
		logoEC->loadFromByteArray(r, w);
		logo2->loadFromByteArray(r, w);
		white->loadFromByteArray(r, w);

	}
	void clearLibrary() {
		StartSFX->clearSFX();
		logoEC->clearTexture();
		logo2->clearTexture();
		white->clearTexture();

	}
};

