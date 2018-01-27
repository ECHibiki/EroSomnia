#pragma once

#include <fstream>

#include <boost\archive\Text_iarchive.hpp>
#include <boost\archive\Text_oarchive.hpp>


class SaveData
{
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);

public:
	SaveData();
	SaveData(char ulock);
	SaveData(int i, int  j, int  k, int  l, int m);
	~SaveData();

	int gate0;	//-9384 //fresh game
	int gate1;	//95631 // won reg
	int gate2;	//1095651 // won time
	int gate3;  //20276   //won hard
	int gate4;  // 991122 // lost reg

	void saveProfile() {
		std::fstream file;
		file.open("data/save.crow", std::fstream::out);
		boost::archive::text_oarchive archive(file);
		archive << this;
	}
	static void loadProfile(SaveData* sd) {
		std::fstream file;
		file.open("data/save.crow");
		if (file.is_open()) {
			boost::archive::text_iarchive archive(file);
			archive >> sd;
		}
		else {
			std::cout << "USER DATA NOT FOUND";
			*sd = SaveData();
		}
	}
};


SaveData::SaveData()
{
	gate0 = -9384;
	gate1 = -2;
	gate2 = -1;
	gate3 = -1;
	gate4 = -1;
}

SaveData::SaveData(char ulock)
{
	gate0 = -9384;
	gate1 = 95631;
	gate2 = 1095651;
	gate3 = 20276;
	gate4 = 991122;
}

SaveData::SaveData(int zero, int one, int two, int three, int four) : gate0(zero), gate1(one), gate2(two), gate3(three), gate4(four)
{
}

SaveData::~SaveData() {}

template<class Archive>
void SaveData::serialize(Archive & ar, const unsigned int version) {
	ar & gate0;
	ar & gate1;
	ar & gate2;
	ar & gate3;
	ar & gate4;
}
