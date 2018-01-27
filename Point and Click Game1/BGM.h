#pragma once
#include <SDL_Mixer\SDL_mixer.h>

#include <boost\archive\binary_iarchive.hpp>
#include <boost\archive\binary_oarchive.hpp>

#include <iostream>
class BGM
{
	friend class boost::serialization::access;

	template <typename  Archive>
	void serialize(Archive & ar, const unsigned int ver) {
		ar & byteArrayContainer;
		ar & size;
	}

public:
	BGM(char* path);
	BGM(Mix_Music* mc);
	BGM(Uint8 ab, Uint32 al, int  allo,Uint8 vol);
	BGM(std::string path, bool t) {
		raw = SDL_RWFromFile(path.c_str(), "rb");
		if (raw == nullptr) {
			size = 0;
			return;
		}
		size = raw->size(raw);
		std::cout << size << std::endl;
		byteArray= new uint8_t[size];
		raw->read(raw, byteArray, 1, size);
		track = Mix_LoadMUSType_RW(SDL_RWFromMem(byteArray, size), MUS_WAV, 1);

		for (int i = 0; i < size; i++)
			byteArrayContainer.push_back(byteArray[i]);
	}
	BGM();
	~BGM();

	void play();
	void play(int i);
	void playFadeIn(int i, int ms);
	void clearBGM();

	void loadFromByteArray();

	Mix_Music* track;
private:

	SDL_RWops* raw;
	std::vector<uint8_t> byteArrayContainer;
	uint8_t * byteArray;
	int size;
};



BGM::BGM()
{
}
BGM::BGM(Mix_Music* bgm)
{
	//trackPrivate = boost::shared_ptr<Mix_Music*> (bgm);
	track = bgm;
}
BGM::BGM(Uint8 ab, Uint32 al, int  allo, Uint8 vol) {

}

BGM::~BGM()
{
}

BGM::BGM(char* path) {
	track = Mix_LoadMUS(path);
}

void BGM::playFadeIn(int i, int ms) {
	Mix_FadeInMusic( track, i, ms);
}

void BGM::play(int i) {
	Mix_PlayMusic( track, i);
}
void BGM::play() {
	Mix_PlayMusic( track, -1);
}

void BGM::clearBGM() {
	Mix_FreeMusic(track);
	delete[] byteArray;
	byteArrayContainer.swap(std::vector<unsigned char>());
	size = 0;
}

void BGM::loadFromByteArray() {
	byteArray = new uint8_t[byteArrayContainer.size()];
	int c = 0;
	for each(uint8_t i in byteArrayContainer) {
		byteArray[c++] = i;
	}
	track = Mix_LoadMUSType_RW(SDL_RWFromMem(byteArray, size), MUS_WAV, 1);
	if(track == NULL) 
		track = Mix_LoadMUSType_RW(SDL_RWFromMem(byteArray, size), MUS_MP3, 1);
	byteArrayContainer.swap(std::vector<unsigned char>());
}