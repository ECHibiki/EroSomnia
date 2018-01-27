#pragma once
#include <SDL2\SDL.h>
#include <SDL_Mixer\SDL_mixer.h>
#include <string>

#include <boost\archive\binary_iarchive.hpp>
#include <boost\archive\binary_oarchive.hpp>
#include <boost/serialization/serialization.hpp>

class SFX
{
	friend class boost::serialization::access;

	template <class Archive>
	void SFX::serialize(Archive & ar, const unsigned int version) {
		ar & volume;
		ar & size;
		ar & byteArrayContainer;
	}

public:
	SFX(std::string path, int vol);
	SFX(Mix_Chunk* mc);
	SFX(Uint8 ab, Uint32 al, int  allo, Uint8 vol);
	SFX() {};
	SFX(std::string path, int vol, bool flag) {
		volume = vol;
		raw = SDL_RWFromFile(path.c_str(), "rb");
		if (raw == nullptr) return;
		size = raw->size(raw);
		std::cout << size << std::endl;
		byteArray = new uint8_t[size];
		raw->read(raw, byteArray, 1, size);
		sfx = Mix_LoadWAV_RW(SDL_RWFromMem(byteArray, size), 1);

		for (int i = 0; i < size; i++)
			byteArrayContainer.push_back(byteArray[i]);
	}
	~SFX();


	void clearSFX();
	void playSFX();
	void playSFX(int n);
	void playSFX(int i, int n);

	void loadFromByteArray();

	Uint8 volume;
	Mix_Chunk* sfx;
private:

	SDL_RWops* raw;
	std::vector<uint8_t> byteArrayContainer;
	uint8_t * byteArray;
	int size;

};

SFX::SFX(std::string path, int vol)
{
	Mix_VolumeChunk(sfx = Mix_LoadWAV(path.c_str()), volume = vol);
}
SFX::SFX(Mix_Chunk* mc)
{
	sfx = mc;
}

SFX::SFX(Uint8 ab, Uint32 al, int  allo, Uint8 vol) {
	sfx->abuf = &ab;
	sfx->alen = al;
	sfx->allocated = allo;
	sfx->volume = vol;
}


SFX::~SFX()
{
}


void SFX::clearSFX() {
	Mix_FreeChunk(sfx);
	sfx = nullptr;
	byteArrayContainer.swap(std::vector<unsigned char>());
	size = 0;
}
void SFX::playSFX() {
	Mix_PlayChannel(-1, sfx, 0);
}
void SFX::playSFX(int n) {
	Mix_PlayChannel(-1, sfx, n);
}
void SFX::playSFX(int i, int n) {
	Mix_PlayChannel(i, sfx, n);
}

void SFX::loadFromByteArray() {
	byteArray = new uint8_t[byteArrayContainer.size()];
	int c = 0;
	for each(uint8_t i in byteArrayContainer) {
		byteArray[c++] = i;
	}
	sfx = Mix_LoadWAV_RW(SDL_RWFromMem(byteArray, size), 1);
	Mix_VolumeChunk(sfx, volume);
	byteArrayContainer.swap(std::vector<unsigned char>());
}
