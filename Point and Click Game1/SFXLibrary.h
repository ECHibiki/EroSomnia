#pragma once
#pragma once
#include "SFX.h"

#include <fstream>

#include <boost\archive\binary_iarchive.hpp>
#include <boost\archive\binary_oarchive.hpp>
#include <boost\serialization\shared_ptr.hpp>

class SFXLibrary {

	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & ClickSFX;
		ar & NoSFX;
		ar & SelectionSFX;
		ar & SelectionSFX2;
		ar & OpenSFX;
		ar & DerobeSFX;
		ar & RobeSFX;

		ar & IntercomSFX;
		ar & HornSFX;
		ar & PassingSFX;
		ar & bangSFX;
		ar & XingSFX;

		ar &  feSFX;
		ar &  moSFX;
		ar & fiSFX;
		ar & masSFX;
		ar &  moan1SFX;
		ar &  neutSFX;

		ar &  failSFX;
		ar &  heartFastSFX;
		ar &  heartMediumSFX;
		ar &  heartSlowSFX;
		ar &  startSFX;

		ar & pointsSFX;
		ar & errorSFX;
	}

public:
	SFXLibrary() {

	}

	void saveSFXLibrary() {
		std::fstream file;
		file.open("data/sfx.mage", std::ios::binary | std::fstream::out);
		boost::archive::binary_oarchive ar(file);
		std::cout << "SAVED SFX\n";
		ar << this;
		file.close();
	}
	static SFXLibrary loadSFXLibrary() {
		std::fstream file;
		SFXLibrary *sl;
		SFXLibrary slCopy;
		file.open("data/sfx.mage", std::ios::binary | std::fstream::in);
		if (file.is_open()) {
			boost::archive::binary_iarchive archive(file);
			archive >> sl;
			slCopy = *sl;
		}
		else {
			std::cout << "SFXLIBRARY NOT FOUND";
			sl = new SFXLibrary();
			slCopy = *sl;
		}
		file.close();

		return slCopy;
	}

	SFX* ClickSFX;
	SFX* NoSFX;
	SFX* SelectionSFX;
	SFX* SelectionSFX2;
	SFX* OpenSFX;
	SFX* DerobeSFX;
	SFX* RobeSFX;

	SFX* IntercomSFX;
	SFX* HornSFX;
	SFX* PassingSFX;
	SFX* bangSFX;
	SFX* XingSFX;

	SFX* feSFX;
	SFX* moSFX;
	SFX*  fiSFX;
	SFX* masSFX;
	//TO ADD
	SFX* moan1SFX;
	SFX* neutSFX;

	SFX* failSFX;
	SFX* heartFastSFX;
	SFX* heartMediumSFX;
	SFX* heartSlowSFX;
	SFX* startSFX;
	//TO END

	SFX* pointsSFX;
	SFX* errorSFX;

	void configureLibrary() {
		ClickSFX->loadFromByteArray();
		NoSFX->loadFromByteArray();
		SelectionSFX->loadFromByteArray();
		SelectionSFX2->loadFromByteArray();
		OpenSFX->loadFromByteArray();
		DerobeSFX->loadFromByteArray();
		RobeSFX->loadFromByteArray();

		IntercomSFX->loadFromByteArray();
		HornSFX->loadFromByteArray();
		PassingSFX->loadFromByteArray();
		bangSFX->loadFromByteArray();
		XingSFX->loadFromByteArray();

		feSFX->loadFromByteArray();
		moSFX->loadFromByteArray();
		fiSFX->loadFromByteArray();
		masSFX->loadFromByteArray();
		moan1SFX->loadFromByteArray();
		neutSFX->loadFromByteArray();

		failSFX->loadFromByteArray();
		heartFastSFX->loadFromByteArray();
		heartMediumSFX->loadFromByteArray();
		heartSlowSFX->loadFromByteArray();
		startSFX->loadFromByteArray();

		pointsSFX->loadFromByteArray();
		errorSFX->loadFromByteArray();
	}

	void clearLibrary() {
		ClickSFX->clearSFX();
		NoSFX->clearSFX();
		SelectionSFX->clearSFX();
		SelectionSFX2->clearSFX();
		OpenSFX->clearSFX();
		DerobeSFX->clearSFX();
		RobeSFX->clearSFX();

		IntercomSFX->clearSFX();
		HornSFX->clearSFX();
		PassingSFX->clearSFX();
		bangSFX->clearSFX();
		XingSFX->clearSFX();

		feSFX->clearSFX();
		moSFX->clearSFX();
		fiSFX->clearSFX();
		masSFX->clearSFX();
		moan1SFX->clearSFX();
		neutSFX->clearSFX();

		failSFX->clearSFX();
		heartFastSFX->clearSFX();
		heartMediumSFX->clearSFX();
		heartSlowSFX->clearSFX();
		startSFX->clearSFX();

		pointsSFX->clearSFX();
		errorSFX->clearSFX();
	}

	bool test = false;
};