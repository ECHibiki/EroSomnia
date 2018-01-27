#pragma once
#pragma once
#include "BGM.h"

#include <fstream>

#include <boost\archive\binary_iarchive.hpp>
#include <boost\archive\binary_oarchive.hpp>
#include <boost\serialization\shared_ptr.hpp>

class BGMLibrary {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & Track1;
		ar & Track2;
		ar & Track3;
		ar & Track4;
		ar & TrainBG;
	}

public:
	BGMLibrary() {

	}

	void saveBGMLibrary() {
		std::fstream file;
		file.open("data/bgm.mage", std::ios::binary | std::fstream::out);
		boost::archive::binary_oarchive archive(file);
		archive << this;
		std::cout << "SAVED BGM\n";
		file.close();
	}
	static BGMLibrary loadBGMLibrary() {
		std::fstream file;
		BGMLibrary* bl;
		BGMLibrary blCopy;
		file.open("data/bgm.mage", std::ios::binary | std::fstream::in);
		if (file.is_open()) {
			boost::archive::binary_iarchive archive(file);
			archive >> bl;
			blCopy = *bl;
		}
		else {
			std::cout << "BGMLIBRARY NOT FOUND";
			bl = new BGMLibrary();
			blCopy = *bl;
		}
		file.close();
		return blCopy;
	}

	BGM* Track1;
	BGM* Track2;
	BGM* Track3;
	BGM* Track4;
	BGM*TrainBG;

	void configureLibrary() {
		Track1->loadFromByteArray();
		Track2->loadFromByteArray();
		Track3->loadFromByteArray();
		Track4->loadFromByteArray();
		TrainBG->loadFromByteArray();
	}
	void clearLibrary() {
		Track1->clearBGM();
		Track2->clearBGM();
		Track3->clearBGM();
		Track4->clearBGM();
		TrainBG->clearBGM();
	}
};
